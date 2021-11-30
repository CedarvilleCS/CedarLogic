#include "gate_or.h"

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