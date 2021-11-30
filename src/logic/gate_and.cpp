#include "gate_and.h"

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
			break; // A single ZERO input will force the gate to ZERO.
		} else if( inputStates[i] == ONE ) {
			// A ONE does nothing, since we assume ONE first.
		} else { // HI_Z, CONFLICT, UNKNOWN
			outState = UNKNOWN;
		}
	}

	setOutputState("OUT", outState);
}

// **************************** END AND GATE ***********************************