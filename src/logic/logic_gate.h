#pragma once

#include "logic_defaults.h"
#include "logic_event.h"
#include "logic_wire.h"

class Circuit;

#include <cassert>
#include <string>
#include <sstream>
using namespace std;


struct GateInput {
	IDType wireID;
	bool inverted;
	
	GateInput() : wireID(ID_NONE), inverted(false) {};
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

	// Used to indicate if changes require an update (clock edge?) to show up in output.
	typedef bool requiresUpdate;
	typedef string error;

	// Update the gate's outputs:
	void updateGate( IDType myID, Circuit * theCircuit );

	// Resend the last event to a (probably newly connected) wire:	
	void resendLastEvent( IDType myID, string destID, Circuit * theCircuit );

	// Set a gate parameter, returns boolean "change requires update to change output state"
	virtual requiresUpdate setParameter( string paramName, string value );

	// Set an input parameter, returns boolean "change requires update to change output state"
	virtual requiresUpdate setInputParameter( string myInputID, string paramName, string value );

	// Set an output parameter, returns boolean "change requires update to change output state"
	virtual requiresUpdate setOutputParameter( string myOutputID, string paramName, string value );

	// Get the value of a gate parameter:
	virtual string getParameter( string paramName ) const;

	// Connect a wire to the input of this gate:
	virtual void connectInput( string myInputID, IDType wireID );

	// Connect a wire to the output of this gate:
	virtual void connectOutput( string myOutputID, IDType wireID );

	// If input exists return wireID else ID_NONE
	virtual IDType getInputWire(string myInputID);

	// If output exists return wireID else ID_NONE
	virtual IDType getOutputWire(string myOutputID);

	// Disconnect a wire from the given input
	// Returns the disconnected wire's wireID
	virtual IDType disconnectInput( string inputID );

	// Disconnect a wire from the given output
	// Returns the disconnected wire's wireID
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

	// Return true if given input exists, else false.
	bool inputExists(string myInputID) const;

	// Register a bus of inputs for this gate. They will be named
	// "busName_0" through "busName_<busWidth-1>". They cannot be
	// set as edge-triggered.
	void declareInputBus(string busName, unsigned long busWidth);

	// Register an output for this gate :
	void declareOutput( string name );

	// Return true if given input exists, else false.
	bool outputExists(string myOutputID) const;

	// Register a bus of outputs for this gate. They will be named
	// "busName_0" through "busName_<busWidth-1>". They cannot be
	// set as edge-triggered.
	void declareOutputBus(string busName, unsigned long busWidth);

	// Set the output to be automatically inverted:
	void  setOutputInverted(string outputID, bool newInv = true);

	// Set the output to be automatically inverted:
	void  setOutputEnablePin(string outputID, string inputID);

	// A helper function that allows you to convert a bus into a unsigned long:
	// (HI_Z, etc. is interpreted as ZERO.)
	unsigned long bus_to_ulong( vector< StateType > busStates ) const;

	// A helper function that allows you to convert an unsigned long number into a bus:
	vector< StateType > ulong_to_bus( unsigned long number, unsigned long numBits ) const;

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

	// Get the types of inputs that are represented.
	vector< bool > groupInputStates( void );
	
	// Compare the "this" state with the "last" state and say if this is a rising edge. 
	bool isRisingEdge( string name );

	// Compare the "this" state with the "last" state and say if this is a falling edge. 
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

	// The default gate delay used for gates if
	// not specified in the call to setOutputState:
	TimeType defaultDelay;

	// Gate's InputID (string) -> Gate::GateInput (struct) map
	ID_MAP< string, GateInput > gateInputMap;

	// Gate's OutputID (string) -> Gate::GateOutput (struct) map
	ID_MAP< string, GateOutput > gateOutputMap;
	
	// Edge-triggered inputs and state data for them:
	ID_SET< string > edgeTriggeredInputs;
	ID_MAP< string, StateType > edgeTriggeredLastState;
	
	// A temporary pointer to the Circuit object, used for getting wire states, time info,
	// and for sending events from gate outputs:
	Circuit* ourCircuit;
	
	// This gate's ID in the Circuit.
	IDType myGateID;

	// Because we can not update parameters when we are loading a file
	// this flag is here so that we know we need to send an update next time we process
	bool flushGuiMemory;
	
	// This paramiter makes it so that we can send updates during non-gate processing calls.
	// It makes flushGuiMemory obsolete, but it works, so I won't fix it.
	vector<string> changedParamWaitingList;
};
