/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   
*****************************************************************************/

#ifndef LOGIC_GATE_H
#define LOGIC_GATE_H

#include "logic_defaults.h"
#include "logic_event.h"
#include "logic_wire.h"

class Circuit;

#include <cassert>  // KAS 2016
#include <string>
#include <sstream>
using namespace std;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct GateInput {
	IDType wireID;
	bool inverted;
	bool pullup;
	bool pulldown;
	
	GateInput() : wireID(ID_NONE), inverted(false), pullup(false), pulldown(false) {};
};

struct GateOutput {
	IDType wireID;
	StateType lastEventState;
	TimeType lastEventTime;
	bool inverted;

	string enableInput; // An input pin which is mapped as the enable pin for this output.

	GateOutput() : wireID(ID_NONE), lastEventState(UNKNOWN), lastEventTime(TIME_NONE), inverted(false), enableInput("") {};
};


class Gate  
{
public:

	// Update the gate's outputs:
	void updateGate( IDType myID, Circuit * theCircuit );

	// Resend the last event to a (probably newly connected) wire:	
	void resendLastEvent( IDType myID, string outputID, Circuit * theCircuit );

	// Set a gate parameter:
	// (If the parameter change requires the gate to be updated to change its
	// output state, then return "true".)
	virtual bool setParameter( string paramName, string value );
	virtual bool setInputParameter( string inputID, string paramName, string value );
	virtual bool setOutputParameter( string outputID, string paramName, string value );

	// Get the value of a gate parameter:
	virtual string getParameter( string paramName );

	// ********* Standard Gate mutator functions ************

	// Connect a wire to the input of this gate:
	virtual void connectInput( string inputID, IDType wireID );

	// Connect a wire to the output of this gate:
	virtual void connectOutput( string outputID, IDType wireID );

	// Return the wire ID of a connected wire, or ID_NONE:
	virtual IDType getInputWire( string inputID ) {
		return (inputList.find( inputID ) == inputList.end()) ? ID_NONE : inputList[inputID].wireID;
	};
	virtual IDType getOutputWire( string outputID ) {
		return (outputList.find( outputID ) == outputList.end()) ? ID_NONE : outputList[outputID].wireID;
	};

	// Disconnect a wire from the input of this gate:
	// (Returns the wireID of the wire that was connected.)
	virtual IDType disconnectInput( string inputID );

	// Disconnect a wire from the output of this gate:
	// (Returns the wireID of the wire that was connected.)
	virtual IDType disconnectOutput( string outputID );

	// Get the first input of the gate:
	string getFirstConnectedInput( void );

	// Get the first output of the gate that has a wire attached to it:
	string getFirstConnectedOutput( void );

	Gate();
	virtual ~Gate();

protected:
	// Gate "Entity" declaration methods:

	// Register an input for this gate:
	// Possibly declare the input as edge triggered, which will cause it
	// to be tracked to be able to check rising and falling edges.
	void declareInput( string inputID, bool edgeTriggered = false );

	// Return true if an input has been declared. False otherwise.
	bool inputExists( string inputID ) {
		return (inputList.find(inputID) != inputList.end());
	};

	// Register a bus of inputs for this gate. They will be named
	// "busName_0" through "busName_<busWidth-1>". They cannot be
	// set as edge-triggered.
	void declareInputBus( string busName, unsigned long busWidth ) {
		ostringstream oss;
		for( unsigned long i = 0; i < busWidth; i++ ) {
			oss.str("");
			oss.clear();
			oss << busName << "_" << i;
			declareInput( oss.str() );
		}
	};



	// Register an output for this gate :
	void declareOutput( string name );

	// Return true if an input has been declared. False otherwise.
	bool outputExists( string outputID ) {
		return (outputList.find(outputID) != outputList.end());
	};

	// Register a bus of outputs for this gate. They will be named
	// "busName_0" through "busName_<busWidth-1>". They cannot be
	// set as edge-triggered.
	void declareOutputBus( string busName, unsigned long busWidth ) {
		ostringstream oss;
		for( unsigned long i = 0; i < busWidth; i++ ) {
			oss.str("");
			oss.clear();
			oss << busName << "_" << i;
			declareOutput( oss.str() );
		}
	};

	// Set the input to be automatically inverted:
	void  setInputInverted( string inputID, bool newInv = true ) {
		if(inputList.find(inputID) == inputList.end()) {
			WARNING("Gate::setInputState() - Invalid input name.");
			assert( false );
			return;
		}

		// Set the inverted state:
		this->inputList[inputID].inverted = newInv;
	};

	// Set the input to be pull-up:
	void  setInputPullUp(string inputID, bool newPU = true) {
		if (inputList.find(inputID) == inputList.end()) {
			WARNING("Gate::setInputState() - Invalid input name.");
			assert(false);
			return;
		}

		// Set the pullup state:
		this->inputList[inputID].pullup = newPU;
	};

	// Set the input to be pull-down:
	void  setInputPullDown(string inputID, bool newPD = true) {
		if (inputList.find(inputID) == inputList.end()) {
			WARNING("Gate::setInputState() - Invalid input name.");
			assert(false);
			return;
		}

		// Set the pulldown state:
		this->inputList[inputID].pulldown = newPD;
	};

	// Set the output to be automatically inverted:
	void  setOutputInverted( string outputID, bool newInv = true ) {
		if(outputList.find(outputID) == outputList.end()) {
			WARNING("Gate::setOutputState() - Invalid output name.");
			assert( false );
			return;
		}

		// Set the inverted state:
		this->outputList[outputID].inverted = newInv;
	};


	// Set the output to be automatically inverted:
	void  setOutputEnablePin( string outputID, string inputID ) {
		if(outputList.find(outputID) == outputList.end()) {
			WARNING("Gate::setOutputEnablePin() - Invalid output name.");
			assert( false );
			return;
		}

		if(inputList.find(inputID) == inputList.end()) {
			WARNING("Gate::setOutputEnablePin() - Invalid input name.");
			assert( false );
			return;
		}

		// Set the enable state:
		this->outputList[outputID].enableInput = inputID;
	};

	// A helper function that allows you to convert a bus into a unsigned long:
	// (HI_Z, etc. is interpreted as ZERO.)
	unsigned long bus_to_ulong( vector< StateType > busStates );

	// A helper function that allows you to convert an unsigned long number into a bus:
	vector< StateType > ulong_to_bus( unsigned long number, unsigned long numBits );

	// Gate "Process" activity methods:
	// The process activity that this specific gate will perform. Note that
	// class "Gate" must be subclassed and gateProcess() defined for a proper
	// gate to be created.
	virtual void gateProcess( void ) = 0;
	
	// Get the current time in the simulation:
	TimeType getSimTime( void );
	
	// Check the state of the named input and return it.
	StateType getInputState( string name );
	
	// Get the input states of a bus of inputs named "busName_0" through
	// "busName_x" and return their states as a vector.
	vector< StateType > getInputBusState( string busName );

	// Pedro Casanova (casanova@ujaen.es) 2020/04-10
	// Get the wire states of a bus of output named "busName_0" through
	// "busName_x" and return their states as a vector.
	vector< StateType > Gate::getOutputBusWireState(string busName);

	// Get the types of inputs that are represented.
	vector< bool > groupInputStates( void );
	
	// Compare the "this" state with the "last" state and say if this is a rising or falling edge. 
	bool isRisingEdge( string name ); 
	bool isFallingEdge( string name ); 

	// Send an output event to one of the outputs of this gate. 
	// Compare the last sent event with the newState and decide whether or not to 
	// really send the event. Also, log the last sent event so that it can be 
	// repeated later if necessary. 
	void setOutputState( string outID, StateType newState, TimeType delay = TIME_NONE );
	
	// Set the output states of a bus of outputs named "busName_0" through
	// "busName_x" using a vector of states:
	void setOutputBusState( string outID, vector< StateType > newState, TimeType delay = TIME_NONE );

	// List a parameter in the Circuit as having been changed:
	void listChangedParam( string paramName );

protected:
	// The default gate delay used for gates if
	// not specified in the call to setOutputState:
	TimeType defaultDelay;

	// A mapping of gate input IDs to circuit wire IDs, to define
	// the inputs into this gate, along with other input information:
	ID_MAP< string, GateInput > inputList;

	// A mapping of gate output IDs to circuit wire ID and wire input ID pairs, along
	// with the storage of the "last state" information for the gate to avoid sending
	// duplicate events:
	ID_MAP< string, GateOutput > outputList;
	
	// Edge-triggered inputs and state data for them:
	ID_SET< string > edgeTriggeredInputs;
	ID_MAP< string, StateType > edgeTriggeredLastState;
	
	// A temporary pointer to the Circuit object, used for getting wire states, time info,
	// and for sending events from gate outputs:
	Circuit* ourCircuit;
	
	// A temporary ID used during updates, which represents this gate's ID in the Circuit.
	IDType myID;
	
	//Edit by Joshua Lansford
	//because we can not update paramiters
	//when we are loading a file
	//this flag is here so that we know we need to
	//send and update next time we process
	bool flushGuiMemory;
	
	
	//This paramiter makes it so that we can
	//send updates during non-gate processing
	//calls.
	//It makes flushGuiMemory obsolete, but
	//it works, so I won't fix it.
	vector<string> changedParamWaitingList;
};


// ******************* Gate_N_INPUT parent Gate *********************
class Gate_N_INPUT : public Gate
{
public:
	// Initialize the gate's interface:
	Gate_N_INPUT();
	
	// Handle gate events:
	void gateProcess( void ) {};

	// Set the parameters:
	bool setParameter( string paramName, string value );

	// Get the parameters:
	string getParameter( string paramName );

protected:
	// The number of input bits:
	unsigned long inBits;
};


// ******************* PASS Gate *********************
// (It basically takes all inputs and passes them to the outputs.)
class Gate_PASS : public Gate_N_INPUT
{
public:
	// Initialize the gate's interface:
	Gate_PASS();
	
	// Handle gate events:
	void gateProcess( void );

	// Set the parameters:
	bool setParameter( string paramName, string value );
};


// ******************* OR Gate *********************
class Gate_OR : public Gate_N_INPUT
{
public:
	// Initialize the gate's interface:
	Gate_OR();
	
	// Handle gate events:
	void gateProcess( void );
};


// ******************* AND Gate *********************
class Gate_AND : public Gate_N_INPUT
{
public:
	// Initialize the gate's interface:
	Gate_AND();

	// Handle gate events:
	void gateProcess( void );
};

// ****************** EQUIVALENCE Gate **************
class Gate_EQUIVALENCE : public Gate_N_INPUT
{
public:
	// Initialize the gate's interface:
	Gate_EQUIVALENCE();
	
	// Handle gate events:
	void gateProcess( void );
};
	

// ******************* XOR Gate *********************
class Gate_XOR : public Gate_N_INPUT
{
public:
	// Initialize the gate's interface:
	Gate_XOR();
	
	// Handle gate events:
	void gateProcess( void );
};


// ******************* Register Gate *********************
class Gate_REGISTER : public Gate_PASS
{
public:
	Gate_REGISTER();

	// Handle gate events:
	void gateProcess( void );

	// Set the parameters:
	bool setParameter( string paramName, string value );

	// Get the parameters:
	string getParameter( string paramName );

protected:
	bool syncSet, syncClear, syncLoad, disableHold, unknownOutputs;

	// The maximum count of this counter (maximum value).
	// (BCD is 9, 4-bit binary is 15.)
	unsigned long maxCount;

	unsigned long currentValue;

	// An initialization value, to make REGISTERs initialize more
	// nicely when loading them or making new ones:
	bool firstGateProcess;

	// The clock pin had a triggering edge and clocking is enabled,
	// or the register isn't synched to a clock.
	// Pedro Casanova (casanova@ujaen.es) 2020/04-10
	// syncSignal added to permit set and clear
	bool hasClockEdge(bool syncSignal);

	// Pedro Casanova (casanova@ujaen.es) 2020/04-10
	// Bidirectional data lines
	bool bidirectionalDATA;

};


// ******************* Clock Gate *********************
class Gate_CLOCK : public Gate
{
public:
	// Initialize the clock:
	Gate_CLOCK( TimeType newHalfCycle = 0 );

	// Handle gate events:
	void gateProcess( void );

	// Set the clock rate:
	bool setParameter( string paramName, string value );

	// Get the clock rate:
	string getParameter( string paramName );

private:
	TimeType halfCycle;
	StateType theState;
};


// ******************* Pulse Gate *********************
class Gate_PULSE : public Gate
{
public:
	// Initialize the clock:
	Gate_PULSE();

	// Handle gate events:
	void gateProcess( void );

	// Set the pulse:
	bool setParameter( string paramName, string value );
private:
	TimeType pulseRemaining;
};


// ******************* Mux Gate *********************
class Gate_MUX : public Gate_N_INPUT
{
public:
	Gate_MUX();

	// Handle gate events:
	void gateProcess( void );

	// Set the parameters:
	bool setParameter( string paramName, string value );

protected:
	unsigned long selBits;
};


// ******************* Decoder Gate *********************
class Gate_DECODER : public Gate_N_INPUT
{
public:
	Gate_DECODER();

	// Handle gate events:
	void gateProcess( void );

	// Set the parameters:
	bool setParameter( string paramName, string value );

protected:
	unsigned long outBits;
};

// ******************* Priority Encoder Gate *********************
// Encodes only the highest significant bit value
class Gate_PRI_ENCODER : public Gate_N_INPUT
{
public:
	Gate_PRI_ENCODER();

	// Handle gate events:
	void gateProcess(void);

	// Set the parameters:
	bool setParameter(string paramName, string value);

protected:
	unsigned long outBits;
};


// ******************* Driver Gate *********************
// Can be used to drive a bus of n bits to a specific binary number.

class Gate_DRIVER : public Gate
{
public:
	// Initialize the driver gate:
	Gate_DRIVER();

	// Handle gate events:
	void gateProcess( void );

	// Set the current state:
	bool setParameter( string paramName, string value );

	// Get the current state:
	string getParameter( string paramName );

private:
	unsigned long output_num;
	unsigned long outBits;
};


// ******************* Full Adder Gate *********************
// Performs an addition of two input busses. Assumes that unknown-type inputs are
// all ZEROs.
// NOTE: Will work with busses of up to 32-bits.

class Gate_ADDER : public Gate_PASS
{
public:
	// Initialize the gate's interface:
	Gate_ADDER();
	
	// Handle gate events:
	void gateProcess( void );

	// Set the parameters:
	bool setParameter( string paramName, string value );
};


// ******************* Magnitude Comparator Gate *********************
// Compares two input busses by magnitude.
// Outputs "A==B", "A<B", "A>B" depending on results.
// Has "A==B", "A<B", "A>B" inputs to allow cascading comparators.
// "A==B" input defaults HIGH with an unknown input, but all others default LOW.

class Gate_COMPARE : public Gate_N_INPUT
{
public:
	// Initialize the gate's interface:
	Gate_COMPARE();
	
	// Handle gate events:
	void gateProcess( void );

	// Set the parameters:
	bool setParameter( string paramName, string value );
};


// ******************* JK Flip Flop Gate *********************
//NOTE: For all inputs, UNKNOWN states are interpreted as 0.

class Gate_JKFF : public Gate
{
public:
	// Initialize the gate's interface:
	Gate_JKFF();
	
	// Handle gate events:
	void gateProcess( void );

	// Set the parameters:
	bool setParameter( string paramName, string value );

	// Get the parameters:
	string getParameter( string paramName );

protected:
	StateType currentState;
	bool syncSet, syncClear;
};

// Pedro Casanova (casanova@ujaen.es) 2020/04-10
// Similar to JKFF
// ******************* T Flip Flop Gate *********************
//NOTE: For all inputs, UNKNOWN states are interpreted as 0.

class Gate_TFF : public Gate
{
public:
	// Initialize the gate's interface:
	Gate_TFF();

	// Handle gate events:
	void gateProcess( void );

	// Set the parameters:
	bool setParameter( string paramName, string value );

	// Get the parameters:
	string getParameter( string paramName );

protected:
	StateType currentState;
	bool syncSet, syncClear;
};

// ***************** n-bit by n-bit RAM Gate *******************
class Gate_RAM : public Gate
{
public:
	// Initialize the ram:
	Gate_RAM();

	// Handle gate events:
	void gateProcess( void );

	// Set the parameters:
	bool setParameter( string paramName, string value );

	// Get the parameters:
	string getParameter( string paramName );

	// Write a file containing the memory data:
	void outputMemoryFile( string fName );

	// Read a file and load the memory data:
	void inputMemoryFile( string fName );
	
//*************************************************
//Edit by Joshua Lansford 1/22/06
//This will make it so that the logic gate can
//also load Intel Hex files.  This is a format
//which is exported by the zad assembler.
	void inputMemoryFileFromIntelHex( string fName );
	
	//a helper function
	int readInHex( ifstream* fin, int numChars );
//End of edit**************************************

protected:
	unsigned long dataBits;
	unsigned long addressBits;
	// Pedro Casanova (casanova@ujaen.es) 2020/04-10
	// Sync and Async write
	bool syncWR;

	// Pedro Casanova (casanova@ujaen.es) 2020/04-10
	// Bidirectional data lines
	bool bidirectionalDATA;

	map< unsigned long, unsigned long > memory;
	
	//This is the last location that a read has
	//taken place from.
	unsigned long lastRead;
};


// ******************* Junction Gate *********************
// This class uses the circuit's Junctioning capabilities
// to enable and disable a junction and splice the inputs
// into the junction, to allow true to/from nodes.
class Gate_JUNCTION : public Gate
{
public:
	// Initialize the junction gate:
	// Note: Because this gate does some really funky stuff,
	// it needs a pointer to the circuit to manipulate the Junction
	// objects.
	Gate_JUNCTION( Circuit *newCircuit );

	// Remove this junction's claim on the junction ID:
	virtual ~Gate_JUNCTION();

	// Handle gate events:
	void gateProcess( void );

	// Set the junction's ID:
	bool setParameter( string paramName, string value );

	// Get the junction's ID:
	string getParameter( string paramName );

	// Connect a wire to the input of this gate:
	void connectInput( string inputID, IDType wireID );

	// Disconnect a wire from the input of this gate:
	// (Returns the wireID of the wire that was connected.)
	IDType disconnectInput( string inputID );

private:
	string myID;
	Circuit * myCircuit;

	// All the wires hooked up to this particular junction gate:
	// (Needs the counted wire deal in case you hook up a wire twice,
	// and then unhook it once. We don't want to remove it from the set yet then.)
	ID_SET< IDType > myWires;
	ID_MAP< IDType, unsigned long > myWireCounts;

	// The shared junction IDs and usage counters:
//	static ID_MAP< string, IDType > junctionIDs;
//	static ID_MAP< string, unsigned long > junctionUseCounter;
};


// ******************* T Gate *********************
// This class uses the circuit's Junctioning capabilities
// to enable and disable a junction and splice the inputs
// into the junction, to allow T-gates.
// Note: All of the connections are INPUTS! (That way, they default to HI_Z.)
// Input 0 = T-Gate input
// Input 1 = T-Gate input2/output
// Input 2 = Control input

class Gate_T : public Gate
{
public:
	// Initialize the gate:
	// Note: Because this gate does some really funky stuff,
	// it needs a pointer to the circuit to manipulate the Junction
	// objects.
	Gate_T( Circuit *newCircuit );
	
	// Destroy the gate, and remove the Junction object from the
	// Circuit:
	virtual ~Gate_T();

	// Handle gate events:
	void gateProcess( void );

	// Connect a wire to the input of this gate:
	void connectInput( string inputID, IDType wireID );

	// Disconnect a wire from the input of this gate:
	// (Returns the wireID of the wire that was connected.)
	IDType disconnectInput( string inputID );

private:
	Circuit * myCircuit;

	// The junctionID of the junction that this t-gate controls:
	IDType junctionID;

	// The last state of the junction:
	bool juncLastState;
};


// ******************* NODE Gate *********************
// This class uses the circuit's Junctioning capabilities
// to splice the inputs into the junction, to allow nodes.
// Note: All of the connections are INPUTS! (That way, they default to HI_Z.)
// Input 0-7 = NODE gate input/output

class Gate_NODE : public Gate
{
public:
	// Initialize the gate:
	// Note: Because this gate does some really funky stuff,
	// it needs a pointer to the circuit to manipulate the Junction
	// objects.
	Gate_NODE( Circuit *newCircuit );
	
	// Destroy the gate, and remove the Junction object from the
	// Circuit:
	virtual ~Gate_NODE();

	// Handle gate events:
	void gateProcess( void );

	// Connect a wire to the input of this gate:
	void connectInput( string inputID, IDType wireID );

	// Disconnect a wire from the input of this gate:
	// (Returns the wireID of the wire that was connected.)
	IDType disconnectInput( string inputID );

private:
	Circuit * myCircuit;

	// The junctionID of the junction that this t-gate controls:
	IDType junctionID;
};



// ******************* BUSEND Gate *********************
// This gate is basically a bunch of junctions.
// It's used as a transition between gui buses and logic wires.
class Gate_BUSEND : public Gate
{
public:
	// Initialize the gate:
	// Note: Because this gate does some really funky stuff,
	// it needs a pointer to the circuit to manipulate the Junction
	// objects.
	Gate_BUSEND(Circuit *newCircuit);

	// Destroy the gate, and remove the Junction objects from the
	// Circuit:
	virtual ~Gate_BUSEND();

	// Handle gate events:
	void gateProcess();

	// Connect a wire to the input of this gate:
	void connectInput(string inputID, IDType wireID);

	// Disconnect a wire from the input of this gate:
	// (Returns the wireID of the wire that was connected.)
	IDType disconnectInput(string inputID);

	// Set the parameters:
	bool setParameter(string paramName, string value);

	// Get the parameters:
	string getParameter(string paramName);

private:
	Circuit * myCircuit;

	// The bus end is just a set of junctions.
	std::vector<IDType> junctionIDs;

	int busWidth;
};

//***************************************************************
//Edit by Joshua Lansford 6/5/2007
//This edit is to create a new gate called the pauseulator.
//This gate has one input and no outputs.  When the input of this
//gate goes high, then it will pause the simulation.  This takes
//avantage of the pauseing hooks that I had to create for the Z80.
class Gate_pauseulator : public Gate
{
public:
	Gate_pauseulator();
	
	
	void gateProcess( void );

	bool setParameter( string paramName, string value );

	string getParameter( string paramName );
};
//End of edit****************************************************




#endif // LOGIC_GATE_H
