/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   
*****************************************************************************/

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



//for the ram gate **** by Joshua Lansford**********
#define DATA_RECORD_HEX 0x00
#define END_OF_FILE_HEX 0x01
//*************************************************

// ***************************** GENERIC GATE ***********************************


Gate::Gate()
{
	ourCircuit = NULL;
	defaultDelay = DEFAULT_GATE_DELAY;
	myID = ID_NONE;
	
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Only one OUTPUT_ENABLE
	// Declare default ENABLE pins, so that any gate can
	// link them to its outputs:
	//declareInputBus("ENABLE", 8);
	
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// To permit OUTPUT_ENABLE
	declareInput("OUTPUT_ENABLE");
	
}


Gate::~Gate()
{

}


// Update the gate's outputs:
// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Added theGUICircuit parameter
void Gate::updateGate( IDType myID, Circuit * theCircuit, GUICircuit * theGUICircuit )
{
	// Store the Circuit variable in the gate to be used during this call to updateGate():
	ourCircuit = theCircuit;
	ourGUICircuit = theGUICircuit;
	this->myID = myID;
	
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
	ID_MAP< string, GateOutput >::iterator theOutput = outputList.begin();
	while( theOutput != outputList.end() ) {
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
	ourGUICircuit = NULL;
	
	return;
}


// Resend the last event to a (probably newly connected) wire:	
void Gate::resendLastEvent( IDType myID, string outputID, Circuit * theCircuit ) {
	if( outputList.find( outputID ) != outputList.end() ) {
		// If a wire is connected now, and there has been a previous event on this gate, then re-send it to the new wire:
		if( ( outputList[outputID].wireID != ID_NONE ) && ( outputList[outputID].lastEventTime != TIME_NONE ) ) {
			// Re-create the event!
			theCircuit->createEvent(outputList[outputID].lastEventTime, outputList[outputID].wireID, myID, outputID, outputList[outputID].lastEventState );
		}
	} else {
		WARNING("Gate::resendLastEvent() - Invalid outputID.");
	}
}


// Connect a wire to the input of this gate:
void Gate::connectInput( string inputID, IDType wireID )
{	
	this->inputList[inputID].wireID = wireID;	
}


// Connect a wire to the output of this gate:
void Gate::connectOutput( string outputID, IDType wireID )
{
	GateOutput myOut;
	// If there was already an output connected on this gate, then
	// copy the old event states over to the new connection. This is because
	// the new wire will need the last event re-sent to it so that it will
	// be activated correctly.
	if( outputList.find( outputID ) != outputList.end() ) {
		myOut = outputList[outputID];
	}



	// Hook up the wire:
	myOut.wireID = wireID;	

	// Save our new output structure in the real output list:
	this->outputList[outputID] = myOut;

}


// Disconnect a wire from the input of this gate:
// (Returns the wireID of the wire that was connected.)
IDType Gate::disconnectInput( string inputID ) {
	IDType wireID = getInputWire( inputID );
	if( wireID != ID_NONE ) {
		// Disconnect the input, but don't remove the connection.
		// (The inverted state and other info must stay.)
		inputList[inputID].wireID = ID_NONE;
	} else {
		WARNING("Gate::disconnectInput() - Invalid input ID.");
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
		outputList[outputID].wireID = ID_NONE;
	} else {
		WARNING("Gate::disconnectOutput() - Invalid output ID.");
	}
	return wireID;
}


// Get the first input of the gate that has a wire attached to it:
string Gate::getFirstConnectedInput( void ) {
	if( !inputList.empty() ) {
		ID_MAP< string, GateInput >::iterator inP = inputList.begin();
		while(inP != inputList.end()) {
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
	if( !outputList.empty() ) {
		ID_MAP< string, GateOutput >::iterator outP = outputList.begin();
		while(outP != outputList.end()) {
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
	if (paramName == "INVERTED") {
		iss >> temp;
		// Set the input inverted state:
		setInputInverted(inputID, (temp == "TRUE"));
		return true;
	} else if (paramName == "PULL_UP") {		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		iss >> temp;
		// Set the input pull-up state:		
		setInputPullUp(inputID, (temp == "TRUE"));
		return true;
	} else if (paramName == "PULL_DOWN") {		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		iss >> temp;
		// Set the input pull-down state:
		setInputPullDown(inputID, (temp == "TRUE"));
		return true;
	}
	else if (paramName == "FORCE_JUNCTION") {	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		iss >> temp;
		// Set the input force-junction state:
		setInputForceJunction(inputID, (temp == "TRUE"));
		return true;
	} else {
		WARNING("Gate::setInputParameter() - Invalid parameter.");
		return false;
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
	} else if (paramName == "E_INPUT") {
		iss >> temp;
		// Set the input inverted state:
		setOutputEnablePin(outputID, temp);
		return true;
	} else {
		WARNING("Gate::setOutputParameter() - Invalid parameter.");
		return false;
	}
}


// Get the value of a gate parameter:
string Gate::getParameter( string paramName ) {
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
void Gate::declareInput(string inputID, bool edgeTriggered) {
	// Touch the item in the list, to make sure that it is created:
	this->inputList[inputID].wireID = ID_NONE;

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
	outputList[ name ].wireID = ID_NONE;
	outputList[ name ].lastEventState = HI_Z; // The GUI assumes HI_Z for all wires to begin with.
	outputList[ name ].lastEventTime = TIME_NONE;
}

// **** Gate "Process" activity methods:
// Note: All of these depend on the ourCircuit pointer to be non-null, and a valid
// myID value, so they can only be called during a call to updateGate().

// Get the current time in the simulation:
TimeType Gate::getSimTime( void ) {	
	_MSGNC(ourCircuit != NULL, "ASSERT END 1")	//####
	assert(ourCircuit != NULL);
	
	return ourCircuit->getSystemTime();
}
	
// Check the state of the named input and return it.
StateType Gate::getInputState( string inputID ) {
	_MSGNC(ourCircuit != NULL, "ASSERT END 2")	//####
	assert(ourCircuit != NULL);
	StateType theState;

	if(inputList.find(inputID) == inputList.end()) {
		WARNING("Gate::getInputState() - Invalid input name.");
		_MSGNC(false, "ASSERT END 3")	//####
		assert( false );
		return ZERO;
	}
	// If the input is connected, get the input value:
	if( inputList[inputID].wireID != ID_NONE ) {
		theState = ourCircuit->getWireState( inputList[inputID].wireID );		
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// Pull-up and Pull-down inputs
		if (theState != CONFLICT)
		{
			// Pedro Casanova (casanova@ujaen.es) 2020/04-12
			// In case HI_Z or UNKNOWN pullup/pulldown must be considered
			if (theState != ZERO && theState != ONE) {
				if (inputList[inputID].pullup)
					theState = ONE;
				else if (inputList[inputID].pulldown)
					theState = ZERO;
			}
		}
	} else {
		// If the input is not connected, then return
		// high-impedance as the "value" for the input.
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// Pull-up and Pull-down inputs
		if (inputList[inputID].pullup)
			theState = ONE;
		else if (inputList[inputID].pulldown)
			theState = ZERO;
		else
			theState = HI_Z;
	}
	// Invert the input if it is set as inverted:
	if (inputList[inputID].inverted) {
		if (theState == ZERO) theState = ONE;
		else if (theState == ONE) theState = ZERO;
	}
	return theState;
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

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// To use bidirectional bus
// Get the wire states of a bus of output named "busName_0" through
// "busName_x" and return their states as a vector.
vector< StateType > Gate::getOutputBusWireState(string busName) {
	unsigned long BUS_MAX_WIDTH = 10000;
	ostringstream pinName;
	vector< StateType > outStates;
	unsigned long i = 0;
	do {
		pinName.str("");
		pinName.clear();
		pinName << busName << "_" << i;		
		if (outputExists(pinName.str())) {
			IDType wireID = getOutputWire(pinName.str());
			if (wireID!=ID_NONE)
				outStates.push_back(ourCircuit->getWireState(wireID));
			else
				outStates.push_back(0);
		}
		i++;
	} while ((outputExists(pinName.str())) && (i < BUS_MAX_WIDTH));
	return outStates;
}

// Get the types of inputs that are represented.
vector< bool > Gate::groupInputStates( void ) {
	_MSGNC(ourCircuit != NULL, "ASSERT END 4")	//####
	assert(ourCircuit != NULL);

	vector< bool > groupedInputs(NUM_STATES, false);

	ID_MAP< string, GateInput >::iterator inputs = inputList.begin();
	while( inputs != inputList.end() ) {

		// Note: Only add the input into the tally if it is connected!
		if( (inputs->second).wireID != ID_NONE ) {
			StateType theState = ourCircuit->getWireState( (inputs->second).wireID );
			groupedInputs[theState] = true;
		}
		else
		{
			// Pedro Casanova (casanova@ujaen.es) 2020/04-12
			// This function is never called
			// For unconnected inputs with pullup or pulldown this must be necesary
			if ((inputs->second).pullup == true)
				groupedInputs[ONE] = true;
			else if ((inputs->second).pulldown == true)
				groupedInputs[ZERO] = true;
		}
		inputs++;
	}
	
	return groupedInputs;
}

	
// Compare the "this" state with the "last" state and say if this is a rising or falling edge. 
bool Gate::isRisingEdge( string name ) {
	_MSGNC(ourCircuit != NULL, "ASSERT END 5")	//####
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
	_MSGNC(ourCircuit != NULL, "ASSERT END 6")	//####
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
	_MSGNC(ourCircuit != NULL, "ASSERT END 7")	//####
	assert( ourCircuit != NULL );

	if(outputList.find(outID) == outputList.end()) {
		WARNING("Gate::setOutputState() - Invalid output name.");
		_MSGNC(false, "ASSERT END 8")	//####
		assert( false );
		return;
	}
	
	if( delay == TIME_NONE ) {
		delay = defaultDelay;
	}

	// The event variables for the event to be thrown:
	TimeType eTime = getSimTime() + delay;
	IDType eWire = outputList[outID].wireID;

	// Set the output state (if the output is inverted, then invert it first):
	StateType eState;
	if( outputList[outID].inverted ) {
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

	if( outputList[outID].enableInput != "" ) {
		// If the enable pin is NOT set to 0, then it is enabled!
		// (Interprets HI_Z, CONFLICT, and UNKNOWN as 1.)
		if( getInputState( outputList[outID].enableInput ) == ZERO ) {
			eState = HI_Z;
		}
	}

	// If the state has changed, then we are interested in this event:
	if( eState != outputList[outID].lastEventState ) {

		// If we have a wire connected, then send the event:
		if( eWire != ID_NONE ) {
			ourCircuit->createEvent( eTime, eWire, myID, outID, eState );
		}
		
		// Store the last-state information to prevent duplicate events,
		// and in case a wire is connected to this output and the event
		// needs to be re-sent:
		outputList[outID].lastEventState = eState;
		outputList[outID].lastEventTime = eTime;
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
		ourCircuit->addUpdateParam( this->myID, paramName );		
	}else{
		changedParamWaitingList.push_back( paramName );	
	}
	
}



// A helper function that allows you to convert a bus into a unsigned long:
// (HI_Z, etc. is interpreted as ZERO.)
unsigned long Gate::bus_to_ulong( vector< StateType > busStates ) {
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
vector< StateType > Gate::ulong_to_bus( unsigned long number, unsigned long numBits ) {
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


// ************************* END GENERIC GATE ***********************************



// ******************************** N-Input GATE ***********************************
// This gate type has no logic inside of it, but it will declare and manage
// the N inputs. This is good for gates of a similar shape.

// Initialize the gate's interface:
Gate_N_INPUT::Gate_N_INPUT() : Gate() {
	// Default of 0 inputs (Must be specified in library file, or no inputs will be made!):
	setParameter("INPUT_BITS", "0");
}


// Set the parameters:
bool Gate_N_INPUT::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "INPUT_BITS" ) {
		iss >> inBits;

		// Declare the address pins!		
		if( inBits > 0 ) {
			declareInputBus( "IN", inBits );
		}

		//NOTE: Don't return "true" from this, because
		// you shouldn't be setting this param during simulation while
		// anything is connected anyhow!
	} else {
		return Gate::setParameter( paramName, value );
	}
	return false;
}


// Set the parameters:
string Gate_N_INPUT::getParameter( string paramName ) {
	ostringstream oss;
	if( paramName == "INPUT_BITS" ) {
		oss << inBits;
		return oss.str();
	} else {
		return Gate::getParameter( paramName );
	}
}


// **************************** END Gate_N_INPUT GATE ***********************************

// ******************************** PASS GATE ***********************************
// This gate simply takes all of the inputs and passes them to the outputs, like
// a buffer. It is useful for creating tri-state buffers and inverters.

// Initialize the gate's interface:
Gate_PASS::Gate_PASS() : Gate_N_INPUT() {
	// Inputs are declared in Gate_N_INPUT();
	// Outputs are declared in setParameter();
	// Default of 1 input (No need to set this in the library file for 1-input gates):
	setParameter("INPUT_BITS", "1");
};
	

// Handle gate events:
void Gate_PASS::gateProcess( void ) {
	// Get the status of all of the inputs:
	vector< StateType > inputStates = getInputBusState("IN");
	vector< StateType > outputStates(inBits, UNKNOWN);
	
	for( unsigned long i = 0; i < inBits; i++ ) {
		// If we have a ONE or ZERO, pass it through:
		if( ( inputStates[i] == ONE ) || ( inputStates[i] == ZERO ) ) {
			outputStates[i] = inputStates[i];
		}
	}

	setOutputBusState("OUT", outputStates);
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// To manage dual outputs
	setOutputBusState("OUTINV", outputStates);
}


// Set the parameters:
bool Gate_PASS::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "INPUT_BITS" ) {
		iss >> inBits;

		// Declare the output pins:
		if( inBits > 0 ) {
			declareOutputBus( "OUT", inBits );
			// Pedro Casanova (casanova@ujaen.es) 2020/04-12
			// Declare the inverted output pins:
			declareOutputBus( "OUTINV", inBits);
			// Make all of the OUTINV pins inverted:
			ostringstream oss;
			for (unsigned long i = 0; i < inBits; i++) {
				oss.str("");
				oss.clear();
				oss << "OUTINV_" << i;
				setOutputInverted(oss.str(), true);
			}
		}

		//NOTE: Don't return "true" from this, because
		// you shouldn't be setting this param during simulation while
		// anything is connected anyhow!
		// Also, allow the Gate_N_INPUT class to change the number of inputs:
	}
	return Gate_N_INPUT::setParameter( paramName, value );
}


// **************************** END PASS GATE ***********************************


// ******************************** OR GATE ***********************************

// Initialize the gate's interface:
Gate_OR::Gate_OR() : Gate_N_INPUT() {
	//NOTE: Inputs are declared by Gate_N_INPUT()

	// Declare the output:
	declareOutput("OUT");
}

// Handle gate events:
void Gate_OR::gateProcess( void ) {
	// Get the status of all of the inputs:
	vector< StateType > inputStates = getInputBusState("IN");
	
	StateType outState = ZERO; // Assume that the output is ZERO first of all.
	for( unsigned long i = 0; i < inBits; i++ ) {
		if( inputStates[i] == ONE ) {
			outState = ONE;
			break; // A single ONE input will force the gate to ONE.
		} else if( inputStates[i] == ZERO ) {
			// A zero does nothing, since we assume zero first.
		} else { // HI_Z, CONFLICT, UNKNOWN
			outState = UNKNOWN;
		}
	}

	setOutputState("OUT", outState);
}

// **************************** END OR GATE ***********************************


// ******************************** AND GATE ***********************************

// Initialize the gate's interface:
Gate_AND::Gate_AND() : Gate_N_INPUT() {
	//NOTE: Inputs are declared by Gate_N_INPUT()

	// Declare the output:
	declareOutput("OUT");

}

// Handle gate events:
void Gate_AND::gateProcess( void ) {
	// Get the status of all of the inputs:
	vector< StateType > inputStates = getInputBusState("IN");
	
	StateType outState = ONE; // Assume that the output is ONE first of all.
	for( unsigned long i = 0; i < inBits; i++ ) {
		if( inputStates[i] == ZERO ) {
			outState = ZERO;
			break; // A single ZERO input will force the gate to ZERO./
		} else if( inputStates[i] == ONE ) {
			// A ONE does nothing, since we assume ONE first.
		} else { // HI_Z, CONFLICT, UNKNOWN
			outState = UNKNOWN;
		}
	}

	setOutputState("OUT", outState);
}

// **************************** END AND GATE ***********************************

// ******************************** PLD_AND GATE ***********************************

// Initialize the gate's interface:
Gate_PLD_AND::Gate_PLD_AND() : Gate_AND() {
	//NOTE: Inputs and outupt are declared by Gate_AND()

	setParameter("FORCE_ZERO","false");

}

// Handle gate events:
void Gate_PLD_AND::gateProcess(void) {
	// Get the status of all of the inputs:
	vector< StateType > inputStates = getInputBusState("IN");

	StateType outState = ONE; // Assume that the output is ONE first of all.
	for (unsigned long i = 0; i < inBits; i++) {
		if (inputStates[i] == ZERO) {
			outState = ZERO;
			break; // A single ZERO input will force the gate to ZERO.
		}
		else if (inputStates[i] == ONE) {
			// A ONE does nothing, since we assume ONE first.
		}
		else { // HI_Z, CONFLICT, UNKNOWN
			outState = UNKNOWN;
		}
	}

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// If FORCE_ZERO output is always ZERO
	if(forceZero)
		outState = ZERO;

	setOutputState("OUT", outState);
}

// Set the parameters:
bool Gate_PLD_AND::setParameter(string paramName, string value) {
	istringstream iss(value);
	if (paramName == "FORCE_ZERO") {		// Pedro Casanova (casanova@ujaen.es) 2020/04-07
		string setVal;
		iss >> setVal;
		forceZero = (setVal == "true");
		return true;
	} else {
		return Gate_AND::setParameter(paramName, value);
	}
	return false;
}

// Get the parameters:
string Gate_PLD_AND::getParameter(string paramName) {
	ostringstream oss;
	if (paramName == "FORCE_ZERO") {
		oss << (forceZero ? "true" : "false");
		return oss.str();
	}
	else {
		return Gate_AND::getParameter(paramName);
	}
}

// **************************** END AND GATE ***********************************

// ************************** EQUIVALENCE GATE *********************************

// Initialize the gate's interface:
Gate_EQUIVALENCE::Gate_EQUIVALENCE() : Gate_N_INPUT() {
	//NOTE: Inputs are declared by Gate_N_INPUT()

	// Declare the output:
	declareOutput("OUT");
}

// Handle gate events:
// This is a 2-inputs XNOR gate
void Gate_EQUIVALENCE::gateProcess( void ) {
	// Get the status of all of the inputs:
	vector< StateType > inputStates = getInputBusState("IN");
	
	StateType outState;
	
	if( (inputStates[0] == ONE && inputStates[1] == ONE)
	  ||(inputStates[0] == ZERO && inputStates[1] == ZERO )){
		outState = ONE;
	}else if( (inputStates[0] == ZERO && inputStates[1] == ONE)
	  ||(inputStates[0] == ONE && inputStates[1] == ZERO )){
	  	outState = ZERO;
	}else{
		outState = UNKNOWN;
	}

	setOutputState("OUT", outState);
}

// ************************ END EQUIVALENCE GATE *******************************


// ******************************** XOR GATE ***********************************

// Initialize the gate's interface:
Gate_XOR::Gate_XOR() : Gate_N_INPUT() {
	//NOTE: Inputs are declared by Gate_N_INPUT()

	// Declare the output:
	declareOutput("OUT");
}

// Handle gate events:
void Gate_XOR::gateProcess( void ) {
	// Get the status of all of the inputs:
	vector< StateType > inputStates = getInputBusState("IN");

	// The XOR operation is basically a parity check.
	// XOR returns TRUE if there are an odd number of 1's.
	StateType outState = ZERO; // Assume ZERO for the output.
	unsigned long numOnes = 0;
	for( unsigned long i = 0; i < inBits; i++ ) {
		// Any unknown-type inputs will cause the output to be unknown:
		if( (inputStates[i] == HI_Z) || (inputStates[i] == CONFLICT) || (inputStates[i] == UNKNOWN) ) {
			outState = UNKNOWN;
			break;
		} else if( inputStates[i] == ONE ) {
			// Tally up the ones:
			numOnes++;
		}
	}
	
	if( outState != UNKNOWN ) {
		// If the number of ONES is odd, then return ONE:
		if( (numOnes % 2) != 0 ) {
			outState = ONE;
		}
	}

	setOutputState("OUT", outState);
}

// **************************** END XOR GATE ***********************************


// ******************************** Register GATE ***********************************

Gate_REGISTER::Gate_REGISTER() : Gate_PASS() {
	// Declare the inputs:
	declareInput("CLOCK", true);
	declareInput("CLOCK_ENABLE");
	declareInput("CLEAR");
	declareInput("SET");
	declareInput("LOAD");

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Control Set - Clear
	declareInput("CONTROL"); // Favors "yes" if not connected!

	// For count reg:
	declareInput("COUNT_ENABLE");
	declareInput("COUNT_UP"); // Favors "up" if not connected!

	// For shift reg:
	declareInput("SHIFT_ENABLE");
	declareInput("SHIFT_LEFT"); // Favors "left" if not connected!
	declareInput("CARRY_IN");

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// For shift and count
	declareInput("COUNT_UP_SHIFT_LEFT"); // Favors "up and "left" if not connected!


	// (Load input bus and the output bus are declared by Gate_PASS):
	declareOutput("CARRY_OUT"); 

// The input state priority goes like this:
// clear, set, load, count_enable, shift_enable, hold
// (So if all of these are set to ONE, the gate will CLEAR and ignore the others. So,
// the input states for the LOAD operation is (load && !clear && !set), even if none of them
// are syncronous. )
// Also, when doing a load operation, un-resolvable inputs are defaulted to 0's.

	// Set the default settings:
	syncSet = true;
	syncClear = true;
	syncLoad = true;
	disableHold = false;
    unknownOutputs = false;
	currentValue = 0;
	maxCount = 0;

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Bidirectional Data
	bidirectionalDATA = false;

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Carry out on overfow, not on max/min
	CoOnOverflow = false;

	// An initialization value, to make REGISTERs initialize more
	// nicely when loading them or making new ones:
	firstGateProcess = true;
}


// Handle gate events:
void Gate_REGISTER::gateProcess( void ) {
	vector< StateType > outBus;
	StateType carryOut = ZERO; // Assume that carry out is reset.	

	// If this is the first time this gate has been simulated,
	// then output the currentValue to the pins:
	if( firstGateProcess ) {
		firstGateProcess = false;
		outBus = ulong_to_bus( currentValue, inBits );
	}

	// Track to see if the current value changes, to know if to send
	// an update message to the GUI:
	unsigned long oldCurrentValue = currentValue;

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Register functions managed by opcode
	// 000: NOP, 001: LOAD, 010: INC, 011: DEC
	// 100: SLL, 101: SRL, 110: ROL, 111:ROR
	vector< StateType > opBus = getInputBusState("OP");
	for (unsigned long i = 0; i < opBus.size(); i++)
		if (opBus[i] != ONE) opBus[i] = ZERO;
	unsigned long OPcode = bus_to_ulong(opBus);

	// Update outBus and currentValue based on the input states.
	if( getInputState("CLEAR") == ONE ) {
		if (getInputState("CONTROL") != ZERO)	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
			if (hasClockEdge(syncClear)) {		// Control Set - Clear and async Clear
				// Clear.
				currentValue = 0;
				if (CoOnOverflow) carryOut = ONE;		//#### Review
			}
	} else if( getInputState("SET") == ONE ) {
		if (getInputState("CONTROL") != ZERO)	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
			if (hasClockEdge(syncSet)) {		// Control Set - Clear and async Set
				// Set.
				vector< StateType > allOnes( inBits, ONE );
				currentValue = bus_to_ulong(allOnes);
				if (CoOnOverflow) carryOut = ONE;		//#### Review
			}
	} else if( getInputState("LOAD") == ONE || OPcode == 1) {
		if (hasClockEdge(syncLoad)) {			// Pedro Casanova (casanova@ujaen.es) 2020/04-12
			// Load.
			vector< StateType > inputBus = getInputBusState("IN");
			for( unsigned long i = 0; i < inputBus.size(); i++ ) {
				if( (inputBus[i] == CONFLICT) || (inputBus[i] == HI_Z) ) {
					inputBus[i] = UNKNOWN;
				}
			}
			currentValue = bus_to_ulong( inputBus );
		}
	} else if( getInputState("COUNT_ENABLE") == ONE || OPcode == 2 || OPcode == 3) {
		// Count.
		if( isRisingEdge("CLOCK") ) {
			// Only count down if count_up is ZERO. This allows
			// HI_Z, CONFLICT, and UNKNOWN to favor counting upwards.
			if (getInputState("COUNT_UP") == ZERO || getInputState("COUNT_UP_SHIFT_LEFT") == ZERO || OPcode == 3) {  // Favors "up" if not connected!
				// Decrement the counter:
				if( (currentValue == 0) || (currentValue > maxCount) ) {
					currentValue = maxCount;
				} else {
					// (currentValue > 0)
					currentValue--;
				}
			} else {
				// Increment the counter:
				currentValue = (currentValue + 1) % (maxCount + 1);
			}
		} 

		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// if CO_ON_OVERFLOW Carry out when count pass fom max to zero or zero to max
		// if not CO_ON_OVERFLOW Carry out when count is zero or max
		// Set the carry out bit, regardless of the clock edge:
		if (getInputState("COUNT_UP") == ZERO || getInputState("COUNT_UP_SHIFT_LEFT") == ZERO || OPcode == 3) {  // Favors "up" if not connected!
			if (!CoOnOverflow) {
				if (currentValue == 0) carryOut = ONE; // Carry out on ZERO count when downcounting and carry out on min/max.
			} else {
				if (currentValue == maxCount) carryOut = ONE; // Carry out on MAX count when downcountingand carry out on overflow.
			}
		}
		else {
			if (!CoOnOverflow) {
				if (currentValue == maxCount) carryOut = ONE; // Carry out on MAX count when upcounting and carry out on min/max.
			} else {
				if (currentValue == 0) carryOut = ONE; // Carry out on ZERO count when upcounting and carry out on overflow.
			}
		}

	} else if( getInputState("SHIFT_ENABLE") == ONE || OPcode == 4 || OPcode == 5) {
		// Shift.
		if (isRisingEdge("CLOCK")) {
			if (getInputState("SHIFT_LEFT") == ZERO || getInputState("COUNT_UP_SHIFT_LEFT") == ZERO || OPcode == 5) { // Favors "left" if not connected!
				// Shift right.
				currentValue >>= 1;

				outBus = ulong_to_bus(currentValue, inBits);

				// Add the input carry if needed:
				if (getInputState("CARRY_IN") == ONE) {
					outBus[inBits - 1] = ONE;
					currentValue = bus_to_ulong(outBus);
				}
			}
			else {
				// Shift left.
				currentValue <<= 1;

				// Add the input carry if needed:
				if (getInputState("CARRY_IN") == ONE) {
					currentValue++;
				}

				// Throw away the extra bits that aren't part of the register,
				// so that when you switch to "right-shift", it doesn't remember
				// more than it should!
				unsigned long mask = 0;
				for (unsigned long i = 0; i < inBits; i++) {
					mask <<= 1;
					mask += 1;
				}
				currentValue = currentValue & mask;
				}
		}

		// Set the carry out bit, regardless of the clock edge:		
		vector< StateType > tempBus = ulong_to_bus(currentValue, inBits);
		if (getInputState("SHIFT_LEFT") == ZERO || getInputState("COUNT_UP_SHIFT_LEFT") == ZERO || OPcode == 5) { // Favors "left" if not connected!
			// Shift right.
			carryOut = tempBus[0];
		}
		else {
			// Shift left.
			carryOut = tempBus[tempBus.size() - 1];
		}
	} 	else if (OPcode == 6 || OPcode == 7) {
		// Rotate.
		if (isRisingEdge("CLOCK")) {
			if (OPcode == 6) {
				// Rotate left.				
				currentValue <<= 1;
				if (currentValue & (1 << inBits)) currentValue++;
			}
			else {
				// Rotate right.
				if (currentValue & 1) currentValue+= (1 << inBits);
				currentValue >>= 1;
			}

			// Throw away the extra bits that aren't part of the register,
			// so that when you switch to "right-shift", it doesn't remember
			// more than it should!
			unsigned long mask = 0;
			for (unsigned long i = 0; i < inBits; i++) {
				mask <<= 1;
				mask += 1;
			}
			currentValue = currentValue & mask;
		}
	} else {
		// If hold is allowed, then keep the current value.
		
		if( disableHold ) {
		// Otherwise, load in what is on the input pins:
			if (hasClockEdge(syncLoad)) {		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
				// Load.
				vector< StateType > inputBus = getInputBusState("IN");
				for( unsigned long i = 0; i < inputBus.size(); i++ ) {
					if( (inputBus[i] == CONFLICT) || (inputBus[i] == HI_Z) ) {
						inputBus[i] = UNKNOWN;
					}
				}
				currentValue = bus_to_ulong( inputBus );
			}
		}
	}

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Only one time is better than in each case how it was
	// It is necesary to permit asynchronous ENABLE
	// Send the value to the bus:
	outBus = ulong_to_bus(currentValue, inBits);

	// Set the output values:
	setOutputState("CARRY_OUT", carryOut);
	
	//********************************
	//Edit by Joshua Lansford 3/15/07
	//While it makes the most sence
	//to let the output of a latch or
	//register to be unknown if
	//it has latched an unknown input,
	//in practicality it is a real pain.
	//When implementing a finite state
	//machine, it is a nusence if
	//the whole thing is in an infinite
	//state of unknowingness
	for( vector< StateType >::iterator I = outBus.begin(); I != outBus.end(); ++I ){
		if( *I != ONE ){
			*I = ZERO;
		}
	}		
	//End of edit**********************
	
	if( outBus.size() != 0 ) {
		setOutputBusState("OUT", outBus);
		setOutputBusState("OUTINV", outBus);
		
		// Check if any of the outputs are "unknown" state, and send that info on
		// to the GUI:
		bool oldUO = unknownOutputs;
		unknownOutputs = false;
		for( unsigned int i = 0; i < outBus.size(); i++ ) {
			if( outBus[i] == UNKNOWN ) {
				unknownOutputs = true;
			}
		}
		// Update the GUI's knowledge of our unknown outputs state, if it has changed:
		if( oldUO != unknownOutputs )
			listChangedParam("UNKNOWN_OUTPUTS");
	}

	// Update the GUI's knowledge of our current value, if it has changed:
	if( currentValue != oldCurrentValue ) {
		listChangedParam("CURRENT_VALUE");
	}

}


// Set the parameters:
bool Gate_REGISTER::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "CURRENT_VALUE" ) {
		iss >> currentValue;
		return true;
	} else if( paramName == "UNKNOWN_OUTPUTS" ) {
		string setVal;
		iss >> setVal;

		unknownOutputs = (setVal == "true");
		return false;
	} else if( paramName == "MAX_COUNT" ) {
		iss >> maxCount;
		return false;
	} else if( paramName == "SYNC_SET" ) {
		string setVal;
		iss >> setVal;

		syncSet = (setVal == "true");
		return false;
	} else if( paramName == "SYNC_CLEAR" ) {
		string setVal;
		iss >> setVal;

		syncClear = (setVal == "true");
		return false;
	} else if( paramName == "SYNC_LOAD" ) {
		string setVal;
		iss >> setVal;

		syncLoad = (setVal == "true");
		return false;
	} else if( paramName == "NO_HOLD" ) {
		string setVal;
		iss >> setVal;

		disableHold = (setVal == "true");
		return false;
	} else if (paramName == "BIDIRECTIONAL_DATA") {			// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		string bidirectionalDATAVal;						// Bidirectional data lines
		iss >> bidirectionalDATAVal;

		bidirectionalDATA = (bidirectionalDATAVal == "true");
		return false;
	} else if (paramName == "CO_ON_OVERFLOW") {				// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		string CoOnOverflowVal;								// Carry out on overfow, not on max/min
		iss >> CoOnOverflowVal;								//## Test (Experimental, not finished)

		CoOnOverflow = (CoOnOverflowVal == "true");
		return false;
	}
	return Gate_PASS::setParameter(paramName, value);

	
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// INPUTS_BITS in Gate_PASS::setParameter to set IN, OUT and OUTINV 
}


// Get the parameters:
string Gate_REGISTER::getParameter( string paramName ) {
	ostringstream oss;
	if( paramName == "CURRENT_VALUE" ) {
		oss << currentValue;
		return oss.str();
	} else if( paramName == "UNKNOWN_OUTPUTS" ) {
		oss << (unknownOutputs ? "true" : "false");
		return oss.str();
	} else if( paramName == "MAX_COUNT" ) {
		oss << maxCount;
		return oss.str();
	} else if( paramName == "SYNC_SET" ) {
		oss << (syncSet ? "true" : "false");
		return oss.str();
	} else if (paramName == "SYNC_CLEAR") {
		oss << (syncClear ? "true" : "false");
		return oss.str();
	} else if (paramName == "SYNC_LOAD") {					// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		oss << (syncLoad ? "true" : "false");				// It was omitted
		return oss.str();
	} else if (paramName == "NO_HOLD") {					// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		oss << (disableHold ? "true" : "false");			// It was omitted
		return oss.str();
	} else if (paramName == "BIDIRECTIONAL_DATA") {			// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		oss << (bidirectionalDATA ? "true" : "false");		// Bidirectional data lines
		return oss.str();
	} else if (paramName == "BIDIRECTIONAL_DATA") {			// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		oss << (CoOnOverflow ? "true" : "false");			// Carry out on overfow, not on max/min
		return oss.str();
	} else {
		return Gate_PASS::getParameter( paramName );
	}
}
// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// syncSignal added to permit set and clear
bool Gate_REGISTER::hasClockEdge(bool syncSignal) {
	return isRisingEdge("CLOCK") && getInputState("CLOCK_ENABLE") != ZERO || !syncSignal;	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
}

// **************************** END Register GATE ***********************************


// ******************************** CLOCK GATE ***********************************


// Initialize the half cycle:
Gate_CLOCK::Gate_CLOCK( TimeType newHalfCycle ) : Gate(), halfCycle(newHalfCycle) {
	theState = ZERO;
	
	// Declare the output:
	declareOutput("CLK");
}


// Handle gate events:
void Gate_CLOCK::gateProcess( void ) {
	
	TimeType now = getSimTime();
	
	if( (halfCycle > 0) && ( now % halfCycle == 0 ) ) {
		if( theState == ZERO ) theState = ONE;
		else theState = ZERO;
	}

	setOutputState( "CLK", theState, 0 );
}


// Set the clock rate:
bool Gate_CLOCK::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "HALF_CYCLE" ) {
		iss >> halfCycle;
		return false;
	} else {
		return Gate::setParameter( paramName, value );
	}
}


// Get the clock rate:
string Gate_CLOCK::getParameter( string paramName ) {
	ostringstream oss;
	if( paramName == "HALF_CYCLE" ) {
		oss << halfCycle;
		return oss.str();
	} else {
		return Gate::getParameter( paramName );
	}
}


// **************************** END CLOCK GATE ***********************************


// ******************************** Pulse GATE ***********************************
// The PULSE gate simply creates a pulse of a specified duration
// in simulation steps. By setting the parameter PULSE, it sets the
// remaining duration of the pulse. Once the duration expires, the
// output will return to 0. If a pulse is still going when another
// PULSE parameter is sent, then the pulse is extended to the normal
// end time of the last pulse.
// NOTE: This is a "polled" gate, so it will always be checked, just
// like Gate_CLOCK.

Gate_PULSE::Gate_PULSE() : Gate() {
	pulseRemaining = 0;
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12	Set the PULSE_WIDTH parameter:
	High_Z = false;
	setParameter("PULSE_WIDTH", "1");
	
	// Declare the output:
	declareOutput("OUT_0");
}


// Handle gate events:
void Gate_PULSE::gateProcess( void ) {
	// The output is ONE if there is pulse remaining, and ZERO otherwise:
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12			Normally Z if High_Z is true
	setOutputState("OUT_0", (pulseRemaining > 0) ? ONE : (High_Z ? HI_Z : ZERO) , 0);

	if( pulseRemaining != 0 ) pulseRemaining--;
}


// Set the parameters:
bool Gate_PULSE::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if (paramName == "PULSE") {
		iss >> pulseRemaining;
		return false; // It's a polled gate, so don't update it otherwise or the pulse count will be wrong.
	} else if (paramName == "PULSE_WIDTH") {
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12	Set the PULSE_WIDTH parameter:
		iss >> pulseWidth;
		if (High_Z)							// Generate a pulse
			pulseRemaining = pulseWidth;
		return false;
	} else 	if (paramName == "HIGH_Z") {
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12	Set the HIGH_Z parameter:
		string setVal;
		iss >> setVal;
		High_Z = (setVal == "true");
		if (High_Z)							// Generate a pulse
			pulseRemaining = pulseWidth;
		return false;
	} else {
		return Gate::setParameter( paramName, value );
	}
}

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Get the parameters:
string Gate_PULSE::getParameter(string paramName) {
	ostringstream oss;
	if (paramName == "PULSE_WIDTH") {
		oss << pulseWidth;
		return oss.str();
	} else if (paramName == "HIGH_Z") {
		oss << (High_Z ? "true" : "false");
		return oss.str();
	} else {
		return Gate::getParameter(paramName);
	}
}

// **************************** END Pulse GATE ***********************************


// ******************************** MUX GATE ***********************************


// Initialize the gate's interface:
Gate_MUX::Gate_MUX() : Gate_N_INPUT() {

	// The control inputs data inputs are declared in setParameter().
	// (Must be set before using this method!)
	setParameter("INPUT_BITS", "0");

	// One output:
	declareOutput("OUT");
}


// Handle gate events:
void Gate_MUX::gateProcess( void ) {
	vector< StateType > selBus = getInputBusState("SEL");
	unsigned long sel = bus_to_ulong( selBus ); //NOTE: The MUX assumes 0 on non-specified input lines (Not UNKNOWN)!
	vector< StateType > inputs = getInputBusState("IN");

	StateType outState = UNKNOWN; // Assume UNKNOWN, in case we select an invalid number.
	if( sel < inputs.size() ) {
		outState = inputs[sel];
	}

	// Muxes can't output HI_Z or CONFLICT!
	if( (outState == HI_Z) || (outState == CONFLICT) ) {
		outState = UNKNOWN;
	}

	setOutputState("OUT", outState);
}


// Set the parameters:
bool Gate_MUX::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "INPUT_BITS" ) {
		iss >> inBits;

		// Declare the selection pins!		
		if( inBits > 0 ) {
			// The number of selection bits is the ceiling of
			// the log base 2 of the number of input bits.
			selBits = (unsigned long)ceil( log((double)inBits) / log(2.0) );
			declareInputBus( "SEL", selBits );
		} else {
			selBits = 0;
		}

		//NOTE: Don't return "true" from this, because
		// you shouldn't be setting this param during simulation while
		// anything is connected anyhow!
		// Also, allow the Gate_N_INPUT class to change the number of inputs:
		return Gate_N_INPUT::setParameter( paramName, value );
	} else {
		return Gate_N_INPUT::setParameter( paramName, value );
	}
	return false;
}


// **************************** END MUX GATE ***********************************


// ******************************** DECODER GATE ***********************************


// Initialize the gate's interface:
Gate_DECODER::Gate_DECODER() : Gate_N_INPUT() {

	// The control inputs data inputs are declared in setParameter().
	// (Must be set before using this method!)
	setParameter("INPUT_BITS", "0");

	//Josh Edit 4/6/2007
	declareInput("ENABLE");
	
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12       chip 74138 is deprecated

	//Josh Edit 10/3/2007
	//declareInput("ENABLE_B");
	//declareInput("ENABLE_C");

	// One output:
	declareOutput("OUT");
}


// Handle gate events:
void Gate_DECODER::gateProcess( void ) {
	vector< StateType > inBus = getInputBusState("IN");
	unsigned long inNum = bus_to_ulong( inBus ); //NOTE: The DECODER assumes 0 on non-specified input lines (Not UNKNOWN)!

	vector< StateType > outBus( outBits, ZERO ); // All bits are 0, except for the active

	//********************************
	//Edit by Joshua Lansford 6/4/2007
	//Reason: The docoder is used with
	//the Z80 to select ports in order
	//to enable them and disable them.
	//the ENABLE input connected to
	//the inverted /IORQ signal.
	//The problem is that when the chip
	//is disabled, the outputs were 
	//floating. This caused ports to
	//not know if they were selected
	//or not and then to fry the data 
	//lines with unknownness.
	//Change: To fix this I have
	//changed the enable input from
	//ENABLE_0 to just ENABLE. The
	//enable is then manually checked
	//to see if it is active before
	//activating an output.
	
	//*************************************
	//Reedit by Joshua Lansford 10/3/2007
	//Resion: I am creating another decoder
	//implementation that needs three enables.
	//for the same resion as states above,
	//(I can't have the outputs float on a
	//disable), I can't use the default enables.
	//Thus I am creating my own.  I am asumeing
	//that it is OK to declare inputs that
	//are not used on the gui side.
	
	bool enabled = true;
	
	//by testing for ZERO instead of one, we let a floating enable be enabling.
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12       chip 74138 is deprecated
	// ENABLE_B y ENABLE_C for chip 74138
	//if( getInputState("ENABLE") == ZERO || getInputState("ENABLE_B") == ZERO || getInputState("ENABLE_C") == ZERO ){

	if (getInputState("ENABLE") == ZERO) {
	    	enabled = false;
	}
	
	if( enabled && inNum < outBus.size() ) {
	
	//End of edit *********************
	
		outBus[inNum] = ONE;
	}

	setOutputBusState("OUT", outBus);
}


// Set the parameters:
bool Gate_DECODER::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "INPUT_BITS" ) {
		iss >> inBits;
		
		// Declare the selection pins!		
		if (inBits > 0) {
			// The number of output bits is the power of 2 of the
			// number of input bits.
			// Pedro Casanova (casanova@ujaen.es) 2020/04-12
			// Replaced: pow(inBits, 2.0) by pow(2.0, inBits)  (It was wrong!)
			outBits = (unsigned long)ceil(pow(2.0, inBits));
			declareOutputBus("OUT", outBits);
		}
		else {
			outBits = 0;
		}

		//NOTE: Don't return "true" from this, because
		// you shouldn't be setting this param during simulation while
		// anything is connected anyhow!
		// Also, allow the Gate_N_INPUT class to change the number of inputs:
		return Gate_N_INPUT::setParameter( paramName, value );
	} else {
		return Gate_N_INPUT::setParameter( paramName, value );
	}
	return false;
}


// **************************** END DECODER GATE ***********************************


// **************************** ENCODER GATE ***********************************

// Initialize the gate's interface:
Gate_ENCODER::Gate_ENCODER() : Gate_N_INPUT() {

	// The control inputs data inputs are declared in setParameter().
	// (Must be set before using this method!)
	setParameter("INPUT_BITS", "0");

	declareInput("ENABLE");

	// One output:
	declareOutput("OUT");
	declareOutput("VALID");

	// Pedro Casanova (casanova@ujan.es) 2020/04-12
	// Set Params, by default high for compatibility with PRI_ENCODER
	setParameter("PRIORITY", "high");
}


// Pedro Casanova (casanova@ujan.es) 2020/04-12
// Now permit none. low and high priority
// Handle gate events:
void Gate_ENCODER::gateProcess(void) {
	vector< StateType > inBus = getInputBusState("IN");
	unsigned long inNum = bus_to_ulong(inBus); //NOTE: The ENCODER assumes 0 on non-specified input lines (Not UNKNOWN)!

	vector< StateType > outBus(outBits, ZERO); // All bits are 0

	int outBusSize = (unsigned long)ceil(log((double)inBits) / log(2.0)); // The size of output will be lg of input size

	bool enabled = true;
	bool isValid = false;

	//by testing for ZERO instead of one, we let a floating enable
	//be enabling.
	if ( getInputState("ENABLE") == ZERO ) {
		enabled = false;
	}

	if (enabled) {
		if (Priority=="none")
		{
			int nHigh = 0;
			for (unsigned int i = 0; i < inBus.size(); i++)
				if (inBus[i] == ONE) nHigh++;
			if (nHigh == 1) {
				isValid = true;
				for (unsigned int i = 0; i < inBus.size(); i++) {
					if (inBus[i] == ONE) {
						outBus = ulong_to_bus(i, outBusSize);
						break;
					}
				}
			}

		}
		else
		{
			if (Priority=="high")
			{
				// Loop through input bits from MSB to LSB to find active
				for (int i = inBus.size() - 1; i >= 0; i--) {
					if (inBus[i] == ONE) {
						// If this bit is one, then we found MSB for output
						outBus = ulong_to_bus(i, outBusSize);
						break;
					}
				}
			}
			else
			{
				// Loop through input bits from LSB to MSB to find active
				for (unsigned int i = 0; i < inBus.size(); i++) {
					if (inBus[i] == ONE) {
						// If this bit is one, then we found MSB for output
						outBus = ulong_to_bus(i, outBusSize);
						break;
					}
				}
			}
			// If input other than zero is recieved, then it's valid
			if (inNum != 0) {
				isValid = true;
			}
		}
	}

	if (isValid) {
		setOutputState("VALID", ONE);
	}
	else {
		setOutputState("VALID", ZERO);
	}

	setOutputBusState("OUT", outBus);
}

// Pedro Casanova (casanova@ujaen.es) 2020/04-07
// Permit none, low and high priority
// Set the parameters:
bool Gate_ENCODER::setParameter(string paramName, string value) {
	istringstream iss(value);
	if (paramName == "INPUT_BITS") {
		iss >> inBits;

		// Declare the selection pins!		
		if (inBits > 0) {
			// The number of output bits is the log base 2 of the
			// number of input bits.
			outBits = (unsigned long)ceil(log((double)inBits) / log(2.0));
			declareOutputBus("OUT", outBits);
		}
		else {
			outBits = 0;
		}

		//NOTE: Don't return "true" from this, because
		// you shouldn't be setting this param during simulation while
		// anything is connected anyhow!
		// Also, allow the Gate_N_INPUT class to change the number of inputs:
		return Gate_N_INPUT::setParameter(paramName, value);
	} else if (paramName == "PRIORITY") {	// Pedro Casanova (casanova@ujaen.es) 2021/01-02
		iss >> Priority;
		return true;
	} else {
		return Gate_N_INPUT::setParameter(paramName, value);
	}
	return false;
}

// Pedro Casanova (casanova@ujaen.es) 2020/04-07
// Get the parameters:
string Gate_ENCODER::getParameter(string paramName) {
	ostringstream oss;
	if (paramName == "PRIORITY") {
		return Priority;
	}
	else {
		return Gate::getParameter(paramName);
	}
}


// **************************** END PRIORITY ENCODER GATE ***********************************//


// ******************************** Driver GATE ***********************************
// Can be used to drive a bus of n bits to a specific binary number.

// Initialize the starting state and the output:
Gate_DRIVER::Gate_DRIVER() : Gate() {
	// The default output number is 0:
	output_num = 0;

	// Default of 0 outputs (Must be specified in library file, or no inputs will be made!):
	setParameter("OUTPUT_BITS", "0");
}


// Handle gate events:
void Gate_DRIVER::gateProcess( void ) {
	// All the driver gate does is throw events IMMEDIATELY
	// whenever the gate has changed state:
	setOutputBusState( "OUT", ulong_to_bus(output_num, outBits), 0 );
}


// Set the toggle state variable:
bool Gate_DRIVER::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "OUTPUT_NUM" ) {
		iss >> output_num;
		return true; // Update the gate during the next step!
	} else if( paramName == "OUTPUT_BITS" ) {
		iss >> outBits;

		// Declare the output pins!		
		if( outBits > 0 ) {
			declareOutputBus( "OUT", outBits );
		}

		//NOTE: Don't return "true" from this, because
		// you shouldn't be setting this param during simulation while
		// anything is connected anyhow!
	} else {
		return Gate::setParameter( paramName, value );
	}
	return false;
}


// Get the toggle state variable:
string Gate_DRIVER::getParameter( string paramName ) {
	ostringstream oss;
	if( paramName == "OUTPUT_NUM" ) {
		oss << output_num;
		return oss.str();
	} else if( paramName == "OUTPUT_BITS" ) {
		oss << outBits;
		return oss.str();
	} else {
		return Gate::getParameter( paramName );
	}
}


// **************************** END Driver GATE ***********************************


// ******************************** Full Adder GATE ***********************************
// Performs an addition of two input busses. Assumes that unknown-type inputs are
// all ZEROs.
// NOTE: Will work with busses of up to 32-bits.

Gate_ADDER::Gate_ADDER() : Gate_PASS() {
	// Declare the inputs:
	declareInput("CARRY_IN");

	// (Load input bus and the output bus are declared by Gate_PASS and in setParams.):
	setParameter("INPUT_BITS", "0");

	// The outputs:
	declareOutput("CARRY_OUT");
	declareOutput("OVERFLOW");
}


// Handle gate events:
void Gate_ADDER::gateProcess( void ) {
	vector< StateType > inBusA = getInputBusState("IN");
	unsigned long inA = bus_to_ulong( inBusA );

	vector< StateType > inBusB = getInputBusState("IN_B");
	unsigned long inB = bus_to_ulong( inBusB );
	
	// Do the addition:
	unsigned long sum = inA + inB;

	// Add in the carry bit:
	if( getInputState("CARRY_IN") == ONE ) sum++;

	// Convert the sum back to binary (with an extra bit):
	vector< StateType > preOutBus = ulong_to_bus( sum, inBits + 1 );
	vector< StateType > outBus = ulong_to_bus( sum, inBits );

	// Decide if there was a carry output:
	StateType carryOut = preOutBus[inBits];
	if( inBits >= 32 ) {
		// Fix the carry out if we are using 32-bit arithmetic:
		unsigned long long longA = inA;
		unsigned long long longB = inB;
		unsigned long long sum = longA + longB;
		if( sum > 0xFFFFFFFF ) {
			carryOut = ONE;
		}
	}

	// Determine overflow:
	StateType overflow = UNKNOWN;
	StateType lastBitA = (inBusA[inBits-1] == ONE) ? ONE : ZERO;
	StateType lastBitB = (inBusB[inBits-1] == ONE) ? ONE : ZERO;
	StateType lastBitSum = (preOutBus[inBits-1] == ONE) ? ONE : ZERO;
	if( lastBitA != lastBitB ) {
		// Differing input signs. No overflow:
		overflow = ZERO;
	} else {
		if(lastBitSum != lastBitA) {
			// Same input signs, yet different output sign. Overflow!
			overflow = ONE;
		} else {
			overflow = ZERO;
		}
	}

	// Set the output values:
	setOutputState("CARRY_OUT", carryOut);
	setOutputState("OVERFLOW", overflow);
	setOutputBusState("OUT", outBus);
}


// Set the parameters:
bool Gate_ADDER::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "INPUT_BITS" ) {
		iss >> inBits;

		// Declare the second input pins:
		if( inBits > 0 ) {
			declareInputBus( "IN_B", inBits );
		}

		//NOTE: Don't return "true" from this, because
		// you shouldn't be setting this param during simulation while
		// anything is connected anyhow!
		// Also, allow the Gate_PASS class to change the number of inputs:
		return Gate_PASS::setParameter( paramName, value );
	} else {
		return Gate_PASS::setParameter( paramName, value );
	}
	return false;
}


// **************************** END Adder GATE ***********************************


// ******************* Magnitude Comparator Gate *********************
// Compares two input busses by magnitude.
// Outputs "A==B", "A<B", "A>B" depending on results.
// Has "A==B", "A<B", "A>B" inputs to allow cascading comparators.
// "A==B" input defaults HIGH with an unknown input, but all others default LOW.

Gate_COMPARE::Gate_COMPARE() : Gate_N_INPUT() {
	// Declare the inputs:
	declareInput("IN_A_EQUAL_B");
	declareInput("IN_A_GREATER_B");
	declareInput("IN_A_LESS_B");

	// Input busses are declared by Gate_N_INPUT and in setParams():
	setParameter("INPUT_BITS", "0");

	// The outputs:
	declareOutput("A_EQUAL_B");
	declareOutput("A_GREATER_B");
	declareOutput("A_LESS_B");
}


// Handle gate events:
void Gate_COMPARE::gateProcess( void ) {
	unsigned long inA = bus_to_ulong( getInputBusState("IN") );
	unsigned long inB = bus_to_ulong( getInputBusState("IN_B") );

	StateType equal = ZERO;
	StateType less = ZERO;
	StateType greater = ZERO;

	if( inA == inB ) {
		if( getInputState("IN_A_GREATER_B") == ONE ) {
			greater = ONE;
		} else if( getInputState("IN_A_LESS_B") == ONE ) {
			less = ONE;
		} else if( getInputState("IN_A_EQUAL_B") != ZERO ) {
			equal = ONE;
		}
	} else if( inA < inB ) {
		less = ONE;
	} else if( inA > inB ) {
		greater = ONE;
	}
	
	// Set the output values:
	setOutputState("A_EQUAL_B", equal);
	setOutputState("A_LESS_B", less);
	setOutputState("A_GREATER_B", greater);
}


// Set the parameters:
bool Gate_COMPARE::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "INPUT_BITS" ) {
		iss >> inBits;

		// Declare the second input pins:
		if( inBits > 0 ) {
			declareInputBus( "IN_B", inBits );
		}

		//NOTE: Don't return "true" from this, because
		// you shouldn't be setting this param during simulation while
		// anything is connected anyhow!
		// Also, allow the Gate_N_INPUT class to change the number of inputs:
		return Gate_N_INPUT::setParameter( paramName, value );
	} else {
		return Gate_N_INPUT::setParameter( paramName, value );
	}
	return false;
}


// **************************** END Comparator GATE ***********************************


// ******************* JK Flip Flop Gate *********************

Gate_JKFF::Gate_JKFF() : Gate() {
	// Declare the inputs:
	declareInput("CLOCK", true);
	declareInput("J");
	declareInput("K");

	declareInput("SET");
	declareInput("CLEAR");

	// The outputs:
	declareOutput("Q");
	declareOutput("NQ");

	// The default state:
	currentState = ZERO;
	setParameter("SYNC_SET", "false");
	setParameter("SYNC_CLEAR", "false");
}


// Handle gate events:
void Gate_JKFF::gateProcess( void ) {
	// Get the input values (Unknown types are assumed as ZERO!):
	bool J = (getInputState("J") == ONE);
	bool K = (getInputState("K") == ONE);
	bool set = (getInputState("SET") == ONE);
	bool clear = (getInputState("CLEAR") == ONE);

	if( clear ) {
		if( (syncClear && isRisingEdge("CLOCK")) || !syncClear ) {
			currentState = ZERO;
		}
	} else if( set ) {
		if( (syncSet && isRisingEdge("CLOCK")) || !syncSet ) {
			currentState = ONE;
		}
	} else if( isRisingEdge("CLOCK") ) {
		if( !J && !K ) {
			currentState = currentState; // Hold
		} else if( !J && K ) {
			currentState = ZERO; // Reset
		} else if( J && !K ) {
			currentState = ONE; // Set
		} else if( J && K ) {
			currentState = (currentState == ONE) ? ZERO : ONE; // Toggle
		}
	}
	// Set the output values:
	setOutputState("Q", currentState);
	setOutputState("NQ", (currentState == ONE) ? ZERO : ONE);
}



// Set the parameters:
bool Gate_JKFF::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "SYNC_SET" ) {
		string setVal;
		iss >> setVal;

		syncSet = (setVal == "true");
		return false;
	} else if( paramName == "SYNC_CLEAR" ) {
		string setVal;
		iss >> setVal;

		syncClear = (setVal == "true");
		return false;
	} else {
		return Gate::setParameter( paramName, value );
	}
	return false;
}


// Get the parameters:
string Gate_JKFF::getParameter( string paramName ) {
	ostringstream oss;
	if( paramName == "SYNC_SET" ) {
		oss << (syncSet ? "true" : "false");
		return oss.str();
	} else if( paramName == "SYNC_CLEAR" ) {
		oss << (syncClear ? "true" : "false");
		return oss.str();
	} else {
		return Gate::getParameter( paramName );
	}
}


// **************************** END JK Flip Flop GATE ***********************************

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Similar to JKFF
// ******************* T Flip Flop Gate *********************

Gate_TFF::Gate_TFF() : Gate() {
	// Declare the inputs:
	declareInput("CLOCK", true);
	declareInput("T");

	declareInput("SET");
	declareInput("CLEAR");

	// The outputs:
	declareOutput("Q");
	declareOutput("NQ");

	// The default state:
	currentState = ZERO;
	setParameter("SYNC_SET", "false");
	setParameter("SYNC_CLEAR", "false");
}


// Handle gate events:
void Gate_TFF::gateProcess(void) {
	// Get the input values (Unknown types are assumed as ZERO!):
	bool T = (getInputState("T") == ONE);
	bool set = (getInputState("SET") == ONE);
	bool clear = (getInputState("CLEAR") == ONE);

	if (clear) {
		if ((syncClear && isRisingEdge("CLOCK")) || !syncClear) {
			currentState = ZERO;
		}
	}
	else if (set) {
		if ((syncSet && isRisingEdge("CLOCK")) || !syncSet) {
			currentState = ONE;
		}
	} else if (isRisingEdge("CLOCK")) {
		if ( !T ) {
			currentState = currentState; // Hold
		} else if ( T ) {
			currentState = (currentState == ONE) ? ZERO : ONE; // Toggle
		}
	}
	// Set the output values:
	setOutputState("Q", currentState);
	setOutputState("NQ", (currentState == ONE) ? ZERO : ONE);
}



// Set the parameters:
bool Gate_TFF::setParameter(string paramName, string value) {
	istringstream iss(value);
	if (paramName == "SYNC_SET") {
		string setVal;
		iss >> setVal;

		syncSet = (setVal == "true");
		return false;
	} else if (paramName == "SYNC_CLEAR") {
		string setVal;
		iss >> setVal;

		syncClear = (setVal == "true");
		return false;
	} else {
		return Gate::setParameter(paramName, value);
	}
	return false;
}


// Get the parameters:
string Gate_TFF::getParameter(string paramName) {
	ostringstream oss;
	if (paramName == "SYNC_SET") {
		oss << (syncSet ? "true" : "false");
		return oss.str();
	} else if (paramName == "SYNC_CLEAR") {
		oss << (syncClear ? "true" : "false");
		return oss.str();
	} else {
		return Gate::getParameter(paramName);
	}
}


// **************************** END T Flip Flop GATE ***********************************


// ******************************** RAM GATE ***********************************


// Initialize the starting state and the output:
Gate_RAM::Gate_RAM( ) : Gate() {

	// Declare the stationary pins:
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Uppercase
	declareInput( "WRITE_CLOCK", true );
	declareInput( "WRITE_ENABLE" );

	// Pedro Casanova (casanova@ujaen.es) 2021/01-02
	// Added ENABLE input
	declareInput("ENABLE");
	
	// NOTE: None of the other pins are declared in advance!
	// They are created in setParameter, because they depend on the RAM's size!

	// Set the RAM's default size:	
	setParameter( "ADDRESS_BITS", "0" );
	setParameter( "DATA_BITS", "0" );
	
	// Set the default settings:
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Sync and Async Write	
	syncWR = true;
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Bidirectional data lines	
	bidirectionalDATA = false;

	lastRead = (unsigned long)-1;
}


// Handle gate events:
void Gate_RAM::gateProcess(void) {

	// Don't do the process unless there are address and data lines declared!
	if ((addressBits == 0) || (dataBits == 0)) return;

	unsigned long address = bus_to_ulong(getInputBusState("ADDRESS"));
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Bidirectional BUS, no DATA_IN pins nedeed
	unsigned long dataIn;
	if (bidirectionalDATA)
		dataIn = bus_to_ulong(getOutputBusWireState("DATA_OUT"));
	else
		dataIn = bus_to_ulong(getInputBusState("DATA_IN"));

	//***********************************************************************
	//Edit by Joshua Lansford 12/31/06
	//Purpose of edit:  When the logic gate loads from a file, it is not
	//   possable to directly notify the gui by paramiters about the new
	//   data.  Thus instead a flag is set and we update the gui now
	if (flushGuiMemory) {
		flushGuiMemory = false;
		listChangedParam("MemoryReset");
		for (map< unsigned long, unsigned long >::iterator I = memory.begin();
			I != memory.end();  ++I) {
			ostringstream virtualPropertyName;
			virtualPropertyName << "Address:";
			virtualPropertyName << I->first; //we just list the address
			listChangedParam(virtualPropertyName.str());
		}
	}
	//End of Edit************************************************************

		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// WRITE_ENABLE Now uppercase
		// Pedro Casanova (casanova@ujaen.es) 2021/01-02
		// Added ENABLE input
	if ((getInputState("WRITE_ENABLE") == ONE) && (getInputState("ENABLE") != ZERO)) {
		// HI_Z all of the data outputs:
		vector< StateType > allHI_Z(dataBits, HI_Z);
		setOutputBusState("DATA_OUT", allHI_Z);
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// WRITE_CLOCK Now Uppercase
		if ((isRisingEdge("WRITE_CLOCK") && syncWR) || !syncWR) {
			// Write to the RAM.
			memory[address] = dataIn;
			ostringstream oss;
			oss << "Wroted to the memory thing: Address = " << address << ", data = " << dataIn;
			WARNING(oss.str());
			//***********************************************************************
			//Edit by Joshua Lansford 12/31/06
			//Purpose of edit:  The Cedar-logic ram gate is being expanded to
			//have a popup that shows the contents of the memory
			//therefore it is necisary for the logic gate to tell the gui gate
			//every time data changes in it.
			ostringstream virtualPropertyName;
			virtualPropertyName << "Address:";
			virtualPropertyName << address;
			listChangedParam(virtualPropertyName.str());
			//End of edit************************************************************
		}
	}
	else {
		// Pedro Casanova (casanova@ujaen.es) 2021/01-02
		// Added ENABLE input
		if (getInputState("ENABLE") != ZERO) {
			// Read from the RAM, and write the data to the outputs.
			vector< StateType > ramReadData = ulong_to_bus(memory[address], dataBits);
			setOutputBusState("DATA_OUT", ramReadData);
		} else {
			// HI_Z all of the data outputs:
			vector< StateType > allHI_Z(dataBits, HI_Z);
			setOutputBusState("DATA_OUT", allHI_Z);
		}
		//***********************************************************************
		//Edit by Joshua Lansford 4/22/06
		//Purpose of edit:  This allerts the pop-up when ever an address has changed
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// To permit ENABLE without BUS, ENABLE_0 now is OUTPUT_ENABLE
		if ((getInputState("OUTPUT_ENABLE") == ONE) && (getInputState("ENABLE") != ZERO)) {
			lastRead = address;
			listChangedParam("lastRead");
		}
		//End of edit******************************************************
	}
}

// Set the parameters:
bool Gate_RAM::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if (paramName == "BIDIRECTIONAL_DATA") {			// Pedro Casanova (casanova@ujaen.es) 2020/04-12 
		string bidirectionalDATAVal;					// Bidirectional data lines
		iss >> bidirectionalDATAVal;

		bidirectionalDATA = (bidirectionalDATAVal == "true");
	} else if (paramName == "SYNC_WR") {				// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		string syncWRVal;								// Sync asnd Async Write
		iss >> syncWRVal;
		
		syncWR = (syncWRVal == "true");;
	} else if( paramName == "ADDRESS_BITS" ) {
		iss >> addressBits;

		// Declare the address pins!		
		if( addressBits > 0 ) {
			declareInputBus( "ADDRESS", addressBits );
		}

		//NOTE: Don't return "true" from this or DATA_BITS, because
		// you shouldn't be setting this param during simulation while
		// anything is connected anyhow!
	} else if( paramName == "DATA_BITS" ) {
		iss >> dataBits;

		// Declare the data input and output pins!
		if( dataBits > 0 ) {
			declareInputBus( "DATA_IN", dataBits );
			declareOutputBus( "DATA_OUT", dataBits );
		}
	} else if (paramName == "CLEAR_MEMORY") {
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// Added Clear Button
		memory.clear();
		flushGuiMemory = true;
		return true;
	} else if( paramName == "WRITE_FILE" ) {
		outputMemoryFile(value);
	} else if( paramName == "READ_FILE" ) {
		if( value.substr( value.length() - 3 ) == "cdm" ){ 
			inputMemoryFile(value);
		//*********************************************
		//Edit by Joshua Lansford. 1/22/06
		//This extends the ram gate so that it can open
		//intel hex files as well.
		//the if that goes with this else was added as
		//well.
		}else{
			inputMemoryFileFromIntelHex( value );
		}
		
		//End of edit**********************************
			
		return true;
	//*******************************************
	//Edit by Joshua Lansford 4/22/07
	//This edit is so that the pop-up can send changes
	//down to the core by editing cells
	} else if( paramName.substr( 0, 8 ) == "Address:" ){
		istringstream addressExtractor( paramName.substr( 8 ) );
		unsigned long addressOfNewData = 0;
		addressExtractor >> addressOfNewData;
		
		unsigned long newData;
		iss >> newData;
		
		if( memory[ addressOfNewData ] != newData ){
			memory[ addressOfNewData ] = newData;
			//now we will re list the param so
			//that the change will bounce back up into
			//the pop-up.
			listChangedParam( paramName );
		}
		return true;
	//********************************************
	
	//********************************
	//Edit by Joshua Lansford 4/22/07
	//This idet is so that the pop-up can know
	//when ever there is a read.
	}else if( paramName == "lastRead" ){
		iss >> lastRead;
	//End of edit******************************
	}else{
		return Gate::setParameter( paramName, value );
	}
	return false;
}


// Set the parameters:
string Gate_RAM::getParameter( string paramName ) {	
	ostringstream oss;
	if (paramName == "BIDIRECTIONAL_DATA") {				// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		oss << (bidirectionalDATA ? "true" : "false");		// Bidirectional data lines
		return oss.str();
	} else if (paramName == "SYNC_WR") {					// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		oss << syncWR;										// Asynchronus Write
		return oss.str();
	} else if( paramName == "ADDRESS_BITS" ) {
		oss << addressBits;
		return oss.str();
	} else if( paramName == "DATA_BITS" ) {
		oss << dataBits;
		return oss.str();
//***********************************************************************
//Edit by Joshua Lansford 12/31/06
//Purpose of edit:  The Cedar-logic ram gate is being expanded to
//have a popup that shows the contents of the memory
//therefore it is necisary for the logic gate to tell the gui gate
//every time data changes in it.
	} else if( paramName.substr( 0, 8 ) == "Address:" ){
		istringstream iss( paramName.substr( 8 ) );
		unsigned long addressOfDataToReturn = 0;
		iss >> addressOfDataToReturn;
		unsigned long dataToReturn = memory[ addressOfDataToReturn ];
		ostringstream oss;
		oss << dataToReturn;
		
		
		return oss.str();
	}else if( paramName == "MemoryReset" ){
		return "true";
//End of edit************************************************************
	//********************************
	//Edit by Joshua Lansford 4/22/07
	//This idet is so that the pop-up can know
	//when ever there is a read.
	}else if( paramName == "lastRead" ){
		oss << lastRead;
		return oss.str();
	//End of edit******************************
	} else {
		return Gate::getParameter( paramName );
	}
}

// Write a file containing the memory data:
void Gate_RAM::outputMemoryFile( string fName ) {
	ofstream oFile( fName.c_str() );
	if(!oFile) {
		WARNING("Gate_RAM::outputMemoryFile() - Couldn't open the memory file for writing.");
		return;
	}
	
	oFile << "# CedarLogic RAM memory file." << endl;
	oFile << "# The file format is:" << endl;
	oFile << "# \"hex_address : hex_data\"" << endl;
	oFile << "#" << endl;
	oFile << "# For example, if the address \"0\" contained the decimal" << endl;
	oFile << "# number \"18\", then the line for that address would be:" << endl;
	oFile << "# \"0 : 12\"" << endl << "#" << endl;
	oFile << "# Note that if a memory location is not represented here," << endl;
	oFile << "# then it is assumed to contain the data value \"0\"" << endl << endl;
	
	// Loop through all of the memory locations that have been written or read and dump the memory data:
	map< unsigned long, unsigned long >::iterator memLoc = memory.begin();
	while( memLoc != memory.end() ) {
		// Format: "hex_address : hex_data"
		// (Do uppercase hex characters.)
		oFile.setf(ios::hex, ios::basefield);
		oFile.setf(ios::uppercase);
		oFile << hex << (memLoc->first) << " : " << hex << (memLoc->second) << endl;
		memLoc++;
	}
	
	oFile.close();
}

// Read a file and load the memory data:
void Gate_RAM::inputMemoryFile( string fName ) {
	ifstream iFile( fName.c_str() );
	if(!iFile) {
		WARNING("Gate_RAM::inputMemoryFile() - Couldn't open the memory file for reading.");
		return;
	}

	// Clear the old memory before loading the new one:
	memory.clear();


//********************************************************
//Edit by Joshua Lansford 12/31/06
//Purpose of edit: Let the gui reset its copy of the 
//     memory too
	flushGuiMemory = true;
//End of edit*********************************************

	string temp;
	istringstream theLine;
	unsigned long address = 0, data = 0;
	char dump;
	while (!iFile.eof()) {
	    getline(iFile, temp, '\n');
	    if (temp[0] != '#') {
	    	// This line is a legitimate parseable line - not a comment line.
	    	
	    	// Try to parse the line:
	    	theLine.clear();

	    	
	    	theLine.str(temp);
			theLine.setf(ios::hex, ios::basefield); // The input data is in hex format
	    	theLine >> address >> dump >> data;
	    	if(!theLine.fail()) {
	    		// If the line parsed correctly, then set the memory value:
	    		memory[address] = data & ((int)pow((float)2, (int)dataBits) - 1);
	    	}
	    }
	}
}

//*************************************************
//Edit by Joshua Lansford 1/22/06
//This will make it so that the logic gate can
//also load Intel Hex files.  This is a format
//which is exported by the zad assembler.
void Gate_RAM::inputMemoryFileFromIntelHex( string fName ){
	ifstream fin( fName.c_str(), ios::in );
	
	bool endOfFile = false;
	char temp = '0';
	int byteCount = 0;
	int nextByte = 0;
	int addressPointer = 0;
	int recordType = 0;
	
	if( fin ){
		//check to make sure the file actually exists before
		//we blow our last data
		memory.clear();
		flushGuiMemory = true;
	}else{
		//TODO: This needs to be able to become visible to the
		//user somehow.  An idea would be to make an error property
		//that gets displayed in an allert box when it changes.
		cout << "Error reading file.  Empty or non-existant?" << endl;
	}
	
	while( !endOfFile && fin ){
		//here we will process a record
		//first we make sure that the first character is a ":"
		fin >> temp;
		if( temp == ':' ){
			byteCount = readInHex( &fin, 2 );
			addressPointer = readInHex( &fin, 4 );
			recordType = readInHex( &fin, 2 );
			
			switch( recordType ){
				case DATA_RECORD_HEX:
					for( int byteNum = 0; byteNum < byteCount; ++byteNum ){
						nextByte = readInHex( &fin, 2 );
						memory[addressPointer + byteNum] = nextByte & ((int)pow((float)2, (int)dataBits) - 1);
					}
				
					break;
				case END_OF_FILE_HEX:
					endOfFile = true;
					break;
				default:
					//TODO: cout doesn't cut the cheeze
					cout << "Unexpected record type: " << recordType << endl;
					endOfFile = true;
					break;
			}
			
			//dump checksum
			readInHex( &fin, 2 );
		}else{
			//TODO: cout doesn't cut the cheeze
			cout << "Error reading file: Expected ':'" << endl;
			endOfFile = true;
		}
	}
	
	fin.close();
}

//helper function that allows reading in hex files
int Gate_RAM::readInHex( ifstream* fin, int numChars ){
	int result = 0;
	char nextChar = '0';
	int charValue = 0;
	for( int i = 0; i < numChars; ++i ){
		(*fin) >> nextChar;
		if( nextChar >= '0' && nextChar <= '9' ){
			charValue = nextChar - '0';
		}else if( nextChar >= 'A' && nextChar <= 'F' ){
			charValue = nextChar - 'A' + 10;
		}else if( nextChar >= 'a' && nextChar <= 'f' ){
			charValue = nextChar - 'a' + 10;
		}else{
			//TODO: cout doesn't cut the cheeze
			cout << "non hex character in stream: " << nextChar << endl;
			charValue = 0;
		}
		result <<= 4;
		result |= charValue;
	}
	return result;
}
	
//End of edit**************************************


// **************************** END RAM GATE ***********************************



// ******************************** Junction GATE ***********************************
// This class uses the circuit's Junctioning capabilities
// to enable and disable a junction and splice the inputs
// into the junction, to allow true to/from nodes.


// This is the mapping of junction states, and how often each is used (# of gates):
//ID_MAP< string, IDType > Gate_JUNCTION::junctionIDs;
//ID_MAP< string, unsigned long > Gate_JUNCTION::junctionUseCounter;


// Initialize the starting state and the output:
Gate_JUNCTION::Gate_JUNCTION( Circuit *newCircuit ) : Gate() {

	// Keep the circuit pointer, to use to access the Junctions
	myCircuit = newCircuit;

	// The only attributes that this gate has is what wires
	// are hooked up, and what its name is. Set the name
	// using "setParameter" so that the new Junction will be
	// created if needed:
	this->setParameter( "JUNCTION_ID", "NONE" );

/*	declareInput( "IN1", 0 );
	declareOutput( "OUT1", 0 );
	
	this->isFrom = isFrom;
*/
}


// Remove this junction's claim on the junction ID:
Gate_JUNCTION::~Gate_JUNCTION() {
	if( !((*(myCircuit->getJunctionIDs())).empty() ) && ((*(myCircuit->getJunctionIDs())).find( myID ) != (*(myCircuit->getJunctionIDs())).end()) ) {
		(*(myCircuit->getJunctionUseCounter()))[myID] -= 1;
		// Unhook this gate's wires from the old junction:
		ID_SET< IDType >::iterator thisWire = myWires.begin();
		while( thisWire != myWires.end() ) {
			myCircuit->disconnectJunction( (*(myCircuit->getJunctionIDs()))[myID], *thisWire );
			thisWire++;
		}

		// If that junction is no longer used, then erase it:
		if( (*(myCircuit->getJunctionUseCounter()))[myID] == 0 ) {
			// Erase the junction from the Circuit:
			myCircuit->deleteJunction( (*(myCircuit->getJunctionIDs()))[myID] );

			// Erase the junction from the junction maps:				
			(*(myCircuit->getJunctionIDs())).erase( myID );
			(*(myCircuit->getJunctionUseCounter())).erase( myID );
		}

	}
}


// Handle gate events:
void Gate_JUNCTION::gateProcess( void ) {
	// Do nothing, 'cause the Junction object does all the work for us!
}


// Set the junction's ID:
bool Gate_JUNCTION::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "JUNCTION_ID" ) {
		string myOldID = myID;

		// Read in the new ID:
		myID = value; // (We want to include whitespace in them, too.)
//		iss >> myID;

		// (Note that the first time that this is called is from the
		// constructor, which calls it with the string "NONE" as the new
		// id, and myID is uninitialized, which means it contains "" already.)

		// If we didn't change the junction ID, then we're done:
		if( myOldID == myID ) return false;
		// Decrement the old junction id counter:
		if( !((*(myCircuit->getJunctionIDs())).empty() ) && ((*(myCircuit->getJunctionIDs())).find( myOldID ) != (*(myCircuit->getJunctionIDs())).end()) ) {
			(*(myCircuit->getJunctionUseCounter()))[myOldID] -= 1;
			// Unhook this gate's wires from the old junction:
			ID_SET< IDType >::iterator thisWire = myWires.begin();
			while( thisWire != myWires.end() ) {
				myCircuit->disconnectJunction( (*(myCircuit->getJunctionIDs()))[myOldID], *thisWire );
				thisWire++;
			}

			// If that junction is no longer used, then erase it:
			if( (*(myCircuit->getJunctionUseCounter()))[myOldID] == 0 ) {
				// Erase the junction from the Circuit:
				myCircuit->deleteJunction( (*(myCircuit->getJunctionIDs()))[myOldID] );

				// Erase the junction from the junction maps:				
				(*(myCircuit->getJunctionIDs())).erase( myOldID );
				(*(myCircuit->getJunctionUseCounter())).erase( myOldID );
			}

		}

		// If the junction does not already exist, then create it. Otherwise,
		// simply increment the "use counter":
		if( (*(myCircuit->getJunctionIDs())).find( myID ) == (*(myCircuit->getJunctionIDs())).end() ) {
			// Create the new junction in the circuit:
			(*(myCircuit->getJunctionIDs()))[myID] = myCircuit->newJunction();

			// Add the use counter:
			(*(myCircuit->getJunctionUseCounter()))[myID] = 1;
		} else {
			(*(myCircuit->getJunctionUseCounter()))[myID] += 1;
		}

		// Add this gate's wires to the newly assigned junction:
		ID_SET< IDType >::iterator thisWire = myWires.begin();
		while( thisWire != myWires.end() ) {
			myCircuit->connectJunction( (*(myCircuit->getJunctionIDs()))[myID], *thisWire );
			thisWire++;
		}

		return false; // gateProcess() doesn't do anything anyway!
	} else {
		return Gate::setParameter( paramName, value );
	}
}


// Set the junction's ID:
string Gate_JUNCTION::getParameter( string paramName ) {
	ostringstream oss;
	if( paramName == "JUNCTION_ID" ) {
		oss << myID;
		return oss.str();
	} else {
		return Gate::getParameter( paramName );
	}
}


// Connect a wire to the input of this gate:
void Gate_JUNCTION::connectInput( string inputID, IDType wireID ) {
	Gate::connectInput( inputID, wireID );

	// Connect the wire to the junction in the Circuit:	
	myCircuit->connectJunction( (*(myCircuit->getJunctionIDs()))[myID], wireID );

	// Track this wire, so that it can move to a new junction if
	// our name changes:
	myWires.insert( wireID );
	if( myWireCounts.find( wireID ) == myWireCounts.end() ) {
		myWireCounts[wireID] = 1;
	} else {
		myWireCounts[wireID] += 1;
	}

//TODO: Decide whether or not it is a good idea to allow inputs
// to be connected to the gate, or if they should all be outputs, to avoid
// the wire having a dangling wireInput object.
}


// Disconnect a wire from the input of this gate:
// (Returns the wireID of the wire that was connected.)
IDType Gate_JUNCTION::disconnectInput( string inputID ) {
	IDType wireID = ID_NONE;
	
	// Call the gate's method:
	wireID = Gate::disconnectInput( inputID );

	if( wireID != ID_NONE ) {
		// Unhook the wire from the Junction in the Circuit:
		myCircuit->disconnectJunction( (*(myCircuit->getJunctionIDs()))[myID], wireID );
	}

	// Erase the wire from our tracking list, so that we won't keep it anymore:
	if( myWireCounts[wireID] == 1 ) {
		myWires.erase( wireID );
		myWireCounts.erase( wireID );
	}

	return wireID;
}


// **************************** END Junction GATE ***********************************



// ******************************** T GATE ***********************************
// This class uses the circuit's Junctioning capabilities
// to enable and disable a junction and splice the inputs
// into the junction, to allow T-gates.
// Note: All of the connections are INPUTS! (That way, they default to HI_Z.)
// Input 0 = T-Gate input
// Input 1 = T-Gate input2/output
// Input 2 = Control input


// Initialize the starting state and the output:
Gate_T::Gate_T( Circuit *newCircuit ) : Gate() {

	// Keep the circuit pointer, to use to access the Junctions
	myCircuit = newCircuit;

	// Create the Junction object in the Circuit:
	junctionID = myCircuit->newJunction();

	// The Junction starts out disconnected:
	juncLastState = false;
	myCircuit->setJunctionState( junctionID, juncLastState );

	// Declare the gate inputs and output:
	declareInput( "T_IN" );
	declareInput( "T_IN2" );
	declareInput( "T_CTRL" );
}


// Destroy the gate, and remove the Junction object from the
// Circuit:
Gate_T::~Gate_T() {
//NOTE: This doesn't crash the system when the Circuit object is destroyed,
//      because Circuit::~Circuit() always explicitly destroys all gates.
	myCircuit->deleteJunction( junctionID );
}


// Handle gate events:
void Gate_T::gateProcess( void ) {
	// The new state to set the junction to:
	// (The junction is set to FALSE unless
	// the control input is 1.)
	bool juncNewState = false;

	// Check the control input to determine the output:
	StateType ctrlValue = getInputState("T_CTRL");
	if( ctrlValue == ONE ) {
		juncNewState = true;
	}

	// If the junction state changed, then update it:
	if( juncNewState != juncLastState ) {
		juncLastState = juncNewState;

		// Use the default delay if the delay is not specified.
		TimeType delay = defaultDelay;

		// The event variables for the event to be thrown:
		TimeType eTime = getSimTime() + delay;
		myCircuit->createJunctionEvent( eTime, junctionID, juncNewState );
	}
}


// Connect a wire to the input of this gate:
void Gate_T::connectInput( string inputID, IDType wireID ) {
	Gate::connectInput( inputID, wireID );

	// If it's the T-gate input, then hook it to the junction also:
	if( inputID != "T_CTRL" ) {
		// Connect the wire to the junction in the Circuit:
		myCircuit->connectJunction( junctionID, wireID );
	}
}


// Disconnect a wire from the input of this gate:
// (Returns the wireID of the wire that was connected.)
IDType Gate_T::disconnectInput( string inputID ) {
	IDType wireID = ID_NONE;
	
	// Call the gate's method:
	wireID = Gate::disconnectInput( inputID );

	if( (wireID != ID_NONE) && (inputID != "T_CTRL") ) {
		// Unhook the wire from the Junction in the Circuit:
		myCircuit->disconnectJunction( junctionID, wireID );
	}

	return wireID;
}


// **************************** END T GATE ***********************************


// **************************** NODE Gate ***********************************
// This class uses the circuit's Junctioning capabilities
// to splice the inputs into the junction, to allow nodes.
// Note: All of the connections are INPUTS! (That way, they default to HI_Z.)
// Input 0-7 = NODE gate input/outputs


// Initialize the starting state and the output:
Gate_NODE::Gate_NODE( Circuit *newCircuit ) : Gate() {

	// Keep the circuit pointer, to use to access the Junctions
	myCircuit = newCircuit;

	// Create the Junction object in the Circuit:
	junctionID = myCircuit->newJunction();

	// The Junction is always connected:
	myCircuit->setJunctionState( junctionID, true );

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Not necesary at all
	// Declare the gate inputs and output:
	/*
	declareInput( "N_IN0" );
	declareInput( "N_IN1" );
	declareInput( "N_IN2" );
	declareInput( "N_IN3" );

	declareInput( "N_IN4" );
	declareInput( "N_IN5" );
	declareInput( "N_IN6" );
	declareInput( "N_IN7" );
	*/
}


// Destroy the gate, and remove the Junction object from the
// Circuit:
Gate_NODE::~Gate_NODE() {
//NOTE: This doesn't crash the system when the Circuit object is destroyed,
//      because Circuit::~Circuit() always explicitly destroys all gates.
	myCircuit->deleteJunction( junctionID );
}


// Handle gate events:
void Gate_NODE::gateProcess( void ) {
	// The Junction handles all of the processing for the Gate_NODE.
}


// Connect a wire to the input of this gate:
void Gate_NODE::connectInput( string inputID, IDType wireID ) {
	Gate::connectInput( inputID, wireID );

	// Connect the wire to the junction in the Circuit:
	myCircuit->connectJunction( junctionID, wireID );
}


// Disconnect a wire from the input of this gate:
// (Returns the wireID of the wire that was connected.)
IDType Gate_NODE::disconnectInput( string inputID ) {
	IDType wireID = ID_NONE;
	
	// Call the gate's method:
	wireID = Gate::disconnectInput( inputID );

	if( wireID != ID_NONE ) {
		// Unhook the wire from the Junction in the Circuit:
		myCircuit->disconnectJunction( junctionID, wireID );
	}

	return wireID;
}


// **************************** END NODE GATE ***********************************

// **************************** BUSEND Gate ***********************************


// Initialize the starting state and the output:
Gate_BUSEND::Gate_BUSEND(Circuit *newCircuit) : Gate() {

	// Keep the circuit pointer, to use to access the Junctions
	myCircuit = newCircuit;
	busWidth = 0;
}

Gate_BUSEND::~Gate_BUSEND() {
	for (IDType id : junctionIDs) {
		myCircuit->deleteJunction(id);
	}
}

void Gate_BUSEND::gateProcess() { }

void Gate_BUSEND::connectInput(string inputID, IDType wireID) {
	Gate::connectInput(inputID, wireID);
	
	// Grab the bus line from the back of the inputID.
	int id = atoi(inputID.substr(inputID.find('_') + 1).c_str());

	// Connect the wire to the junction in the Circuit:
	myCircuit->connectJunction(junctionIDs[id], wireID);
}

IDType Gate_BUSEND::disconnectInput(string inputID) {
	IDType wireID = ID_NONE;

	// Call the gate's method:
	wireID = Gate::disconnectInput(inputID);

	// Grab the bus line from the back of the inputID.
	int id = atoi(inputID.substr(inputID.find('_') + 1).c_str());

	if (wireID != ID_NONE) {
		// Unhook the wire from the Junction in the Circuit:
		myCircuit->disconnectJunction(junctionIDs[id], wireID);
	}

	return wireID;
}

bool Gate_BUSEND::setParameter(string paramName, string value) {

	istringstream iss(value);
	if (paramName == "INPUT_BITS") {

		if (busWidth != 0) {
			return false;
		}

		iss >> busWidth;

		// Declare the address pins!		
		if (busWidth > 0) {

			// Declare inputs and outputs for connect/disconnect routines.
			declareInputBus("IN", busWidth);
			// Pedro Casanova (casanova@ujaen.es) 2020/04-12
			// Add second connection point to buses
			declareInputBus("OUT", busWidth);		// For compatibility
			declareInputBus("CNA", busWidth);		// Connection A
			declareInputBus("CNB", busWidth);		// Connection B

			// Create internal junctions.
			for (int i = 0; i < busWidth; i++) {
				junctionIDs.push_back(myCircuit->newJunction());
				myCircuit->setJunctionState(junctionIDs[i], true);
			}
		}

		//NOTE: Don't return "true" from this, because
		// you shouldn't be setting this param during simulation while
		// anything is connected anyhow!
	}
	else {
		return Gate::setParameter(paramName, value);
	}
	return false;
}

string Gate_BUSEND::getParameter(string paramName) {

	ostringstream oss;
	if (paramName == "INPUT_BITS") {
		oss << busWidth;
		return oss.str();
	}
	else {
		return Gate::getParameter(paramName);
	}
}


// **************************** END BUSEND GATE ***********************************


//***************************************************************
//Edit by Joshua Lansford 6/5/2007
//This edit is to create a new gate called the pauseulator.
//This gate has one input and no outputs.  When the input of this
//gate goes high, then it will pause the simulation.  This takes
//avantage of the pauseing hooks that I had to create for the Z80.
Gate_pauseulator::Gate_pauseulator() : Gate(){
	declareInput( "SIGNAL", true );	
}

void Gate_pauseulator::gateProcess( void ) {
	if( isRisingEdge( "SIGNAL" ) ){
		listChangedParam( "PAUSE_SIM" );
	}
}

bool Gate_pauseulator::setParameter( string paramName, string value ) {
	//this is here to catch PAUSE_SIM so that when we load
	//and PAUSE_SIM gets thrown at us from the file,
	//we will pretend to do something with it.
	return false;
}

string Gate_pauseulator::getParameter( string paramName ) {
	//the only param that the system might we wanting is
	//PAUSE_SIM, so we will return "TRUE" because we only
	//flag it when it is true.
	return "TRUE";
}


//End of edit****************************************************


// ******************************** FSM GATE ***********************************
// Pedro Casanova (casanova@ujaen.es) 2021/01-02
// Implement Finite State Machines
Gate_FSM_SYNC::Gate_FSM_SYNC() : Gate_PASS() {
	// Declare the inputs:
	declareInput("CLOCK", true);
	declareInput("CLEAR");

	setParameter("OUTPUT_BITS", "1");

	// Set the default settings:
	setParameter("CURRENT_STATE", "");
	asyncrhonous = false;
	resetState = "";
	currentOutput = "";
	return;
}

// Handle gate events:
void Gate_FSM_SYNC::gateProcess(void) {
	vector< StateType > outBus;
	vector< StateType > inputBus = getInputBusState("IN");
	string oldCurrentState = currentState;

	string oldOutputValue = currentOutput;

	if (currentState == "")
		currentState = resetState;
	for (unsigned long i = 0; i < inputBus.size(); i++)
		if ((inputBus[i] == CONFLICT) || (inputBus[i] == HI_Z))	inputBus[i] = UNKNOWN;
	unsigned long currentInputs = 0;
	if (inBits>0)
		currentInputs = bus_to_ulong(inputBus);
	unsigned long outputVal = 0;
	if (states.size()>0) {
		if (getInputState("CLEAR") == ONE)				// Reset
			currentState = resetState;
		else {
			string newState;
			if (asyncrhonous)											// Asynchronous
				newState = nextState[currentState][currentInputs];
			else if (isRisingEdge("CLOCK"))								// Synchronous
				newState = nextState[currentState][currentInputs];

			for(unsigned long i=0;i<states.size();i++)
				if (states[i] == newState) {
					currentState = newState;
					break;
				}
		}
		currentOutput = outputs[currentState][currentInputs];
		for (unsigned long i = 0; i < currentOutput.length(); i++)
			if (i<outBits)
				if (currentOutput.c_str()[currentOutput.length() - i - 1] == '1')
					outputVal += pow(2, i);
		outBus = ulong_to_bus(outputVal, outBits);
	} else {
		for (unsigned long i = 0; i < outBus.size(); i++)
			outBus[i] = UNKNOWN;
	}

	if (currentOutput != oldOutputValue) {
		currentOutput = oldOutputValue;
		if (outBus.size() != 0) setOutputBusState("OUT", outBus);
	}

	if (currentState != oldCurrentState) listChangedParam("CURRENT_STATE");
}

// Set the parameters:
bool Gate_FSM_SYNC::setParameter(string paramName, string value) {
	istringstream iss(value);
	if (paramName == "CLEAR_FSM") {
		states.clear();
		outputs.clear();
		nextState.clear();
		resetState = "";
		currentState = "";
		currentOutput = "";
		return true;
	}
	if (paramName == "CURRENT_STATE") {
		iss >> currentState;
		return true;
	}
	if (paramName == "OUTPUT_BITS") {
		iss >> outBits;
		
		// Declare the output pins!		
		if (outBits > 0) {
			declareOutputBus("OUT", outBits);
		}
		return true;
	}
	if (paramName.substr(0, 6) == "State:")
	{
		if (value!="")
			procState(paramName, value);
		return true;
	}
	return Gate_PASS::setParameter(paramName, value);
}

// Get the parameters:
string Gate_FSM_SYNC::getParameter(string paramName) {
	ostringstream oss;
	if (paramName == "CURRENT_STATE") {
		oss << currentState;
		return oss.str();
	}
	else if (paramName == "OUTPUT_BITS") {
		oss << outBits;
		return oss.str();
	}
	else {
		return Gate_PASS::getParameter(paramName);
	}
}

// Process state param string
void Gate_FSM_SYNC::procState(string paramName, string value) {
	int pSlash;
	int pArrow;
	bool Moore = false;
	bool noInputs = false;

	string nState = paramName.substr(6);

	istringstream iss(value);
	string state;
	iss >> state;
	string defOutputs = "";
	string defNextState = state;
	pSlash = state.find('/');
	pArrow = state.find('-');
	if (pSlash == -1) {
		for (unsigned long i = 0; i < outBits; i++)
			defOutputs += "0";
	} else {
		Moore = true;		
		if (pArrow == -1) {
			defOutputs = state.substr(pSlash + 1);
			defNextState = state.substr(0, pSlash);
		} else {
			noInputs = true;
			defOutputs = state.substr(pSlash + 1, pArrow - pSlash - 1);
			defNextState = state.substr(pArrow + 1);
		}
		state = state.substr(0, pSlash);
	}

	if (nState=="0") resetState = state;

	states.push_back(state);

	vector<string> vect;
	string ceros = "";

	for (unsigned long i=0;i<pow(2,inBits);i++)
		vect.push_back(defOutputs);
	outputs[state] = vect;
	for (unsigned long i = 0; i < pow(2, inBits); i++)
		vect[i]= defNextState;
	nextState[state] = vect;

	if (!noInputs)
		while (!iss.eof()) {
			string trans;
			iss >> trans;
			pArrow = trans.find('-');
			pSlash = trans.find('/');
			string inputValue = trans.substr(0, pArrow);
			for (unsigned int inputVal = 0; inputVal < pow(2, inputValue.length()); inputVal++)
			{
				string binValue="";
				unsigned int val= inputVal;
				for (long i = inputValue.length() - 1; i >= 0; i--)
				{
					if (val >= pow(2, i)) {
						val -= pow(2, i);
						binValue = binValue+ "1";
					}
					else
						binValue = binValue + "0";
					if (inputValue[inputValue.length()-i-1] == 'X')
						binValue[binValue.length()-1] = 'X';
				}
				if (binValue == inputValue) {
					if (Moore) {
						nextState[state][inputVal] = trans.substr(pArrow + 1);
					}
					else {
						outputs[state][inputVal] = trans.substr(pSlash + 1);
						nextState[state][inputVal] = trans.substr(pArrow + 1, pSlash - pArrow - 1);
					}
				}
			}
		}
}

// **************************** END FSM GATE ***********************************

// ******************************** CMB GATE ***********************************
// Pedro Casanova (casanova@ujaen.es) 2021/01-02
// Implement Combinational Blocks
Gate_CMB::Gate_CMB() : Gate_PASS() {

	setParameter("OUTPUT_BITS", "1");

	return;
}

// Handle gate events:
void Gate_CMB::gateProcess(void) {
	vector< StateType > outBus;
	vector< StateType > inputBus = getInputBusState("IN");


	unsigned long inputValue = bus_to_ulong(inputBus);
	unsigned long outputValue = outputs[inputValue];
	outBus = ulong_to_bus(outputValue, outBits);

	if (outBus.size() != 0) setOutputBusState("OUT", outBus);
}

// Set the parameters:
bool Gate_CMB::setParameter(string paramName, string value) {
	istringstream iss(value);
	if (paramName == "OUTPUT_BITS") {
		iss >> outBits;

		// Declare the output pins!		
		if (outBits > 0) {
			declareOutputBus("OUT", outBits);
		}
		return true;
	}
	if (paramName.substr(0, 9) == "Function:")
	{		
		if (value != "")
			procFunction(paramName, value);
		return true;
	}
	return Gate_PASS::setParameter(paramName, value);
}

// Get the parameters:
string Gate_CMB::getParameter(string paramName) {
	ostringstream oss;
	if (paramName == "OUTPUT_BITS") {
		oss << outBits;
		return oss.str();
	}
	else {
		return Gate_PASS::getParameter(paramName);
	}
}

// Process state param string
void Gate_CMB::procFunction(string paramName, string value) {
	int pEqual =value.find('=');
	unsigned int nFunction = atoi(value.substr(1,pEqual-1).c_str());
	value = value.substr(pEqual + 1);

	bool sum = true;
	if (value.substr(0) == "0" || value.substr(0, 1) == "S") {
		for (unsigned int i = 0; i < pow(2, inBits); i++)
			outputs[i] = outputs[i] & ~((unsigned int)pow(2, nFunction));
	} else if (value.substr(0) == "1" || value.substr(0, 1) == "P") {
		sum = false;
		for (unsigned int i = 0; i < pow(2, inBits); i++)
			outputs[i] = outputs[i] | (unsigned int)pow(2, nFunction);
	} else {
			for (unsigned int i = 0; i < value.length(); i++) {
				char valHex;
				if (chkDigits(value.substr(value.length()-i-1, 1)))
					valHex = value[value.length() - i - 1] - '0';
				else
					valHex = value[value.length() - i - 1] - 'A' + 10;
				for (int j = 3; j >= 0; j--) {
					if (valHex >= pow(2, j)) {
						//_MSG("1")
						valHex -= pow(2, j);
						outputs[4*i+j]= outputs[4*i+j] | (unsigned int)pow(2, nFunction);
					} else {
						outputs[4 * i + j] = outputs[4 * i + j] & ~(unsigned int)pow(2, nFunction);
					}
				}
			}
			return;
	}
	
	if ((value.substr(0) == "0" || value.substr(0) == "1") && (value.length() == 1)) return;

	value = value.substr(2, value.length() - 3);	// Remove "()"

	istringstream iss(value);
	while (!iss.eof()) {
		unsigned int term;
		char dump;
		iss >> term >> dump;		
		if (sum)
			outputs[term] = outputs[term] | (unsigned int)pow(2, nFunction);
		else
			outputs[term] = outputs[term] & ~((unsigned int)pow(2, nFunction));

	}

}

// **************************** END CMB GATE ***********************************





