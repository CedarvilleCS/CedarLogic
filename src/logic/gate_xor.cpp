#include "gate_xor.h"

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
