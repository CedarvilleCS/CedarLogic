
#include "gate_node.h"

#include "logic_circuit.h"

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

	// Declare the gate inputs and output:
	declareInput( "N_in0" );
	declareInput( "N_in1" );
	declareInput( "N_in2" );
	declareInput( "N_in3" );

	declareInput( "N_in4" );
	declareInput( "N_in5" );
	declareInput( "N_in6" );
	declareInput( "N_in7" );
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

