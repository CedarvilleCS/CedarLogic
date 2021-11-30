
// logic_gate.cpp: implementation of the Gate class.
//
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <cmath>
using namespace std;
#include "logic_gate.h"
#include "logic_circuit.h"



// ***************************** GENERIC GATE ***********************************


Gate::Gate()
{
	ourCircuit = NULL;
	defaultDelay = DEFAULT_GATE_DELAY;
	myGateID = ID_NONE;
	
	// Declare default ENABLE pins, so that any gate can
	// link them to its outputs:
	declareInputBus("ENABLE", 8);
/*	declareInput("ENABLE_0");
	declareInput("ENABLE_1");
	declareInput("ENABLE_2");
	declareInput("ENABLE_3");

	declareInput("ENABLE_4");
	declareInput("ENABLE_5");
	declareInput("ENABLE_6");
	declareInput("ENABLE_7");
*/

}


Gate::~Gate()
{

}


// Update the gate's outputs:
void Gate::updateGate( IDType myID, Circuit * theCircuit )
{
	// Store the Circuit variable in the gate to be used during this call to updateGate():
	ourCircuit = theCircuit;
	this->myGateID = myID;
	
	//******************************************
	//Edit by Joshua Lansford 4/22/07
	//This goes ahead and lists all paramiters
	//that wanted to be listed betwean updateGate
	//class and couldn't
	for( vector<string>::iterator I = changedParamWaitingList.begin();
	    	I != changedParamWaitingList.end(); ++I ){
		listChangedParam( *I );
	}
	changedParamWaitingList.clear();
	//*******************************************
	
	// Call the subclassed gate's function to process the events for this gate:
	this->gateProcess();

	// Handle the enabled/disabled outputs:
	ID_MAP< string, GateOutput >::iterator theOutput = gateOutputMap.begin();
	while( theOutput != gateOutputMap.end() ) {
		string enableIn = (theOutput->second).enableInput;
		if( enableIn != "" ) {
			// If the enable pin is NOT set to 0, then it is enabled!
			// (Interprets HI_Z, CONFLICT, and UNKNOWN as 1.)
			if( getInputState( enableIn ) == ZERO ) {
				setOutputState( theOutput->first, HI_Z );
			}
		}
		theOutput++;
	}
		

	// Update the last state of the edge-triggered inputs:
	ID_SET< string >::iterator eInputs = edgeTriggeredInputs.begin();
	while( eInputs != edgeTriggeredInputs.end() ) {
		edgeTriggeredLastState[ *eInputs ] = getInputState( *eInputs );
		eInputs++;
	}
	
	// Invalidate the circuit pointer, because we are done with it:
	ourCircuit = NULL;
	
	return;
}


// Resend the last event to a (probably newly connected) wire:	
void Gate::resendLastEvent( IDType myID, string destID, Circuit * theCircuit ) {
	if( gateOutputMap.find( destID ) != gateOutputMap.end() ) {
		// If a wire is connected now, and there has been a previous event on this gate, then re-send it to the new wire:
		if( ( gateOutputMap[destID].wireID != ID_NONE ) && ( gateOutputMap[destID].lastEventTime != TIME_NONE ) ) {
			// Re-create the event!
			theCircuit->createEvent(gateOutputMap[destID].lastEventTime, gateOutputMap[destID].wireID, myID, destID, gateOutputMap[destID].lastEventState );
		}
	} else {
		WARNING("Gate::resendLastEvent() - Invalid destID.");
	}
}


// Connect a wire to the input of this gate:
void Gate::connectInput( string myInputID, IDType wireID )
{
	if (this->inputExists(myInputID)) {
		this->gateInputMap[myInputID].wireID = wireID;
	}
	else {
		WARNING("Input " + myInputID + " did not exist!");
	}
}


// Connect a wire to the output of this gate:
void Gate::connectOutput( string myOutputID, IDType wireID )
{
	if (this->outputExists(myOutputID)) {
		GateOutput myGateOutput = gateOutputMap[myOutputID];
		// Hook up the wire:
		myGateOutput.wireID = wireID;

		// Update the outputMap
		this->gateOutputMap[myOutputID] = myGateOutput;
	}
	else {
		WARNING("Output " + myOutputID + " does not exist!");
	}
}

// If input exists return wireID else ID_NONE
IDType Gate::getInputWire(string myInputID) {
	return (inputExists(myInputID)) ? ID_NONE : gateInputMap[myInputID].wireID;
};

// If output exists return wireID else ID_NONE
IDType Gate::getOutputWire(string myOutputID) {
	return (outputExists(myOutputID)) ? ID_NONE : gateOutputMap[myOutputID].wireID;
};

// Return true if given input exists, else false.
bool Gate::inputExists(string myInputID) const {
	return (gateInputMap.find(myInputID) != gateInputMap.end());
};

// Return true if given input exists, else false.
bool Gate::outputExists(string myOutputID) const {
	return (gateOutputMap.find(myOutputID) != gateOutputMap.end());
}

void  Gate::declareInputBus(string busName, unsigned long busWidth) {
	ostringstream oss;
	for (unsigned long i = 0; i < busWidth; i++) {
		oss.str("");
		oss.clear();
		oss << busName << "_" << i;
		declareInput(oss.str());
	}
};

void Gate::declareOutputBus(string busName, unsigned long busWidth) {
	ostringstream oss;
	for (unsigned long i = 0; i < busWidth; i++) {
		oss.str("");
		oss.clear();
		oss << busName << "_" << i;
		declareOutput(oss.str());
	}
};

// Set the output to be automatically inverted:
void  Gate::setOutputInverted(string outputID, bool newInv) {
	if (gateOutputMap.find(outputID) == gateOutputMap.end()) {
		WARNING("Gate::setOutputState() - Invalid output name.");
		assert(false);
		return;
	}

	// Set the inverted state:
	this->gateOutputMap[outputID].inverted = newInv;
};

// Set the output to be automatically inverted:
void  Gate::setOutputEnablePin(string outputID, string inputID) {
	if (gateOutputMap.find(outputID) == gateOutputMap.end()) {
		WARNING("Gate::setOutputEnablePin() - Invalid output name.");
		assert(false);
		return;
	}

	if (inputExists(inputID)) {
		WARNING("Gate::setOutputEnablePin() - Invalid input name.");
		assert(false);
		return;
	}

	// Set the enable state:
	this->gateOutputMap[outputID].enableInput = inputID;
};

// Disconnect a wire from the input of this gate:
// (Returns the wireID of the wire that was connected.)
IDType Gate::disconnectInput( string inputID ) {
	IDType wireID = getInputWire( inputID );
	if( wireID != ID_NONE ) {
		// Disconnect the input, but don't remove the connection.
		// We want to preserve the value of GateInput.inverted
		gateInputMap[inputID].wireID = ID_NONE;
	} else {
		WARNING("Gate::disconnectInput() - found no wire to disconnect!");
	}
	return wireID;
}


// Disconnect a wire from the output of this gate:
// (Returns the wireID of the wire that was connected.)
IDType Gate::disconnectOutput( string outputID ) {
	IDType wireID = getOutputWire( outputID );
	if( wireID != ID_NONE ) {
		// Leave the output there, because it has "last state" info
		// even if a wire is not connected currently!
		gateOutputMap[outputID].wireID = ID_NONE;
	} else {
		WARNING("Gate::disconnectOutput() - Invalid output ID.");
	}
	return wireID;
}


// Get the first input of the gate that has a wire attached to it:
string Gate::getFirstConnectedInput( void ) {
	if( !gateInputMap.empty() ) {
		ID_MAP< string, GateInput >::iterator inP = gateInputMap.begin();
		while(inP != gateInputMap.end()) {
			if((inP->second).wireID != ID_NONE) {
				return inP->first;
			}
			inP++;
		}
	}
	
	return "";
}


// Get the first output of the gate that has a wire attached to it:
string Gate::getFirstConnectedOutput( void ) {
	if( !gateOutputMap.empty() ) {
		ID_MAP< string, GateOutput >::iterator outP = gateOutputMap.begin();
		while(outP != gateOutputMap.end()) {
			if((outP->second).wireID != ID_NONE) {
				return outP->first;
			}
			outP++;
		}
	}
	
	// If there are none, then return ID_NONE.
	return "";
}


// Set a gate parameter:
bool Gate::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "DEFAULT_DELAY" ) {
		iss >> defaultDelay;
		return false;
	} else {
		WARNING("Gate::setParameter() - Invalid parameter.");
		return false;
	}
}

bool Gate::setInputParameter( string inputID, string paramName, string value ) {
	istringstream iss(value);
	string temp;
	if( paramName == "INVERTED" ) {
		iss >> temp;
		// Set the input inverted state:
		this->gateInputMap[inputID].inverted = (temp == "TRUE");
		return true;
	} else {
		WARNING("Gate::setInputParameter() - Invalid parameter.");
		return false;
	}
}

bool Gate::setOutputParameter( string outputID, string paramName, string value ) {
	istringstream iss(value);
	string temp;
	if( paramName == "INVERTED" ) {
		iss >> temp;
		// Set the input inverted state:
		setOutputInverted( outputID, (temp == "TRUE"));
		return true;
	} else if( paramName == "E_INPUT" ) {
		iss >> temp;
		// Set the input inverted state:
		setOutputEnablePin( outputID, temp );
		return true;
	} else {
		WARNING("Gate::setOutputParameter() - Invalid parameter.");
		return false;
	}
}


// Get the value of a gate parameter:
string Gate::getParameter( string paramName ) const {
	ostringstream oss;
	if( paramName == "DEFAULT_DELAY" ) {
		oss << defaultDelay;
	} else {
		WARNING("Gate::getParameter() - Invalid parameter.");
	}
	return oss.str();
}


// ******************** Gate Subclass Use Methods **********************************
// These are used by the subclassed gate types to define what interface and
// process each gate posesses.


// **** Gate "Entity" declaration methods:

// Register an input for this gate:
// Possibly declare the input as edge triggered, which will cause it
// to be tracked to be able to check rising and falling edges.
void Gate::declareInput( string inputID, bool edgeTriggered ) {
	// Touch the item in the list, to make sure that it is created:
	this->gateInputMap[inputID].wireID = ID_NONE;

	if( edgeTriggered ) {
		edgeTriggeredInputs.insert( inputID );
		
		// NOTE: We don't set a last state here, because we don't want
		// the first event to come along to cause a rising or falling edge.
		// The first event to come along (i.e. there is no "last state" information)
		// will not register as either edge.
		// Not: edgeTriggeredLastState[name] = UNKNOWN;
	}
}

// Register an output for this gate:
void Gate::declareOutput( string name ) {
	gateOutputMap[ name ].wireID = ID_NONE;
	gateOutputMap[ name ].lastEventState = HI_Z; // The GUI assumes HI_Z for all wires to begin with.
	gateOutputMap[ name ].lastEventTime = TIME_NONE;
}

// **** Gate "Process" activity methods:
// Note: All of these depend on the ourCircuit pointer to be non-null, and a valid
// myID value, so they can only be called during a call to updateGate().

// Get the current time in the simulation:
TimeType Gate::getSimTime( void ) {
	assert(ourCircuit != NULL);
	
	return ourCircuit->getSystemTime();
}
	
// Check the state of the named input and return it.
StateType Gate::getInputState( string inputID ) {
	assert(ourCircuit != NULL);

	if(inputExists(inputID)) {
		WARNING("Gate::getInputState() - Invalid input name.");
		assert( false );
		return ZERO;
	}

	// If the input is connected, get the input value:
	if( gateInputMap[inputID].wireID != ID_NONE ) {
		StateType theState = ourCircuit->getWireState( gateInputMap[inputID].wireID );
		
		// Invert the input if it is set as inverted:
		if( gateInputMap[inputID].inverted ) {
			if( theState == ZERO ) theState = ONE;
			else if( theState == ONE ) theState = ZERO;
		}
		
		return theState;
	} else {
		// If the input is not connected, then return
		// high-impedance as the "value" for the input.
		return HI_Z;
	}
}

// Get the input states of a bus of inputs named "busName_0" through
// "busName_x" and return their states as a vector.
vector< StateType > Gate::getInputBusState( string busName ) {
	unsigned long BUS_MAX_WIDTH = 10000;
	ostringstream pinName;
	vector< StateType > inStates;
	unsigned long i = 0;
	do {
		pinName.str("");
		pinName.clear();
		pinName << busName << "_" << i;
		if( inputExists( pinName.str() ) ) {
			inStates.push_back( getInputState( pinName.str() ) );
		}
		i++;
	} while( (inputExists( pinName.str() )) && (i < BUS_MAX_WIDTH) );
	return inStates;
}


// Get the types of inputs that are represented.
vector< bool > Gate::groupInputStates( void ) {
	assert(ourCircuit != NULL);

	vector< bool > groupedInputs(NUM_STATES, false);

	ID_MAP< string, GateInput >::iterator inputs = gateInputMap.begin();
	while( inputs != gateInputMap.end() ) {

		// Note: Only add the input into the tally if it is connected!
		if( (inputs->second).wireID != ID_NONE ) {
			StateType theState = ourCircuit->getWireState( (inputs->second).wireID );
			groupedInputs[theState] = true;
		}

		inputs++;
	}
	
	return groupedInputs;
}

	
// Compare the "this" state with the "last" state and say if this is a rising or falling edge. 
bool Gate::isRisingEdge( string name ) {
	assert(ourCircuit != NULL);
	
	if( edgeTriggeredLastState.find( name ) == edgeTriggeredLastState.end() ) {
		// There can be no rising edge on the first time that the gate is simulated!
		return false;
	}
	
	StateType last = edgeTriggeredLastState[ name ];
	StateType now = getInputState( name );

	if( ( now == ONE ) && (last != ONE) ) {
		return true;
	} else {
		return false;
	}
}


bool Gate::isFallingEdge( string name ) {
	assert(ourCircuit != NULL);
	
	if( edgeTriggeredLastState.find( name ) == edgeTriggeredLastState.end() ) {
		// There can be no rising edge on the first time that the gate is simulated!
		return false;
	}
	
	StateType last = edgeTriggeredLastState[ name ];
	StateType now = getInputState( name );

	if( ( now == ZERO ) && (last != ZERO) ) {
		return true;
	} else {
		return false;
	}
}


// Send an output event to one of the outputs of this gate. 
// Compare the last sent event with the newState and decide whether or not to 
// really send the event. Also, log the last sent event so that it can be 
// repeated later if necessary. 
void Gate::setOutputState( string outID, StateType newState, TimeType delay ) {
	
	assert( ourCircuit != NULL );

	if(gateOutputMap.find(outID) == gateOutputMap.end()) {
		WARNING("Gate::setOutputState() - Invalid output name.");
		assert( false );
		return;
	}
	
	if( delay == TIME_NONE ) {
		delay = defaultDelay;
	}

	// The event variables for the event to be thrown:
	TimeType eTime = getSimTime() + delay;
	IDType eWire = gateOutputMap[outID].wireID;

	// Set the output state (if the output is inverted, then invert it first):
	StateType eState;
	if( gateOutputMap[outID].inverted ) {
		if( newState == ONE ) {
			eState = ZERO;
		} else if( newState == ZERO ) {
			eState = ONE;
		} else {
			eState = newState;
		}
	} else {
		eState = newState;
	}

	if( gateOutputMap[outID].enableInput != "" ) {
		// If the enable pin is NOT set to 0, then it is enabled!
		// (Interprets HI_Z, CONFLICT, and UNKNOWN as 1.)
		if( getInputState( gateOutputMap[outID].enableInput ) == ZERO ) {
			eState = HI_Z;
		}
	}

	// If the state has changed, then we are interested in this event:
	if( eState != gateOutputMap[outID].lastEventState ) {

		// If we have a wire connected, then send the event:
		if( eWire != ID_NONE ) {
			ourCircuit->createEvent( eTime, eWire, myGateID, outID, eState );
		}
		
		// Store the last-state information to prevent duplicate events,
		// and in case a wire is connected to this output and the event
		// needs to be re-sent:
		gateOutputMap[outID].lastEventState = eState;
		gateOutputMap[outID].lastEventTime = eTime;
	}
}

	
// Set the output states of a bus of outputs named "busName_0" through
// "busName_x" using a vector of states:
void Gate::setOutputBusState( string outID, vector< StateType > newState, TimeType delay ) {
	ostringstream pinName;
	for( unsigned long i = 0; i < newState.size(); i++ ) {
		pinName.str("");
		pinName.clear();
		pinName << outID << "_" << i;
		setOutputState( pinName.str(), newState[i], delay);
	}
}


// List a parameter in the Circuit as having been changed:
void Gate::listChangedParam( string paramName ) {
	//*************************************
	//Edit by Joshua Lansford 4/22/07
	//I am changeing it so that instead of
	//asserting that ourCircuit != NULL,
	//we will just stach paramiters to pass
	//later instead of just forbidding them.
	//This way pop-ups can be updated imediatly
	//instead of waiting until next gate step
	
	if(ourCircuit != NULL){
	
		// Send the update param to the Circuit:
		ourCircuit->addUpdateParam( this->myGateID, paramName );
		
	}else{
		changedParamWaitingList.push_back( paramName );	
	}
	
}


// A helper function that allows you to convert a bus into a unsigned long:
// (HI_Z, etc. is interpreted as ZERO.)
unsigned long Gate::bus_to_ulong( vector< StateType > busStates ) const {
	unsigned long theNumber = 0;

	// Loop from MSB to LSB:
	for( long i = (busStates.size() - 1); i >= 0; i-- ) {
		// Bit-shift to put the number in the right position:
		theNumber <<= 1;

		// Add a 1 into the number if the bus has one:
		if( busStates[i] == ONE ) {
			theNumber++;
		}
	}

	return theNumber;
}


// A helper function that allows you to convert an unsigned long number into a bus:
vector< StateType > Gate::ulong_to_bus( unsigned long number, unsigned long numBits ) const {
	vector< StateType > theBus;
	unsigned long mask = 1;
	
	for( unsigned int i = 0; i < numBits; i++ ) {

		// If that bit of the number is turned on,
		// then output a ONE. Else ZERO:
		if( mask & number ) {
			theBus.push_back( ONE );
		} else {
			theBus.push_back( ZERO );
		}

		// Shift the mask to the next bit:
		mask <<= 1;
	}

	return theBus;
}
