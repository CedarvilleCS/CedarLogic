#include "gate_T.h"
#include "logic_circuit.h"

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
	declareInput( "T_in" );
	declareInput( "T_in2" );
	declareInput( "T_ctrl" );
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
	StateType ctrlValue = getInputState("T_ctrl");
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
	if( inputID != "T_ctrl" ) {
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

	if( (wireID != ID_NONE) && (inputID != "T_ctrl") ) {
		// Unhook the wire from the Junction in the Circuit:
		myCircuit->disconnectJunction( junctionID, wireID );
	}

	return wireID;
}


// **************************** END T GATE ***********************************

