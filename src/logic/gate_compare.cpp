
#include "gate_compare.h"

// ******************* Magnitude Comparator Gate *********************
// Compares two input busses by magnitude.
// Outputs "A==B", "A<B", "A>B" depending on results.
// Has "A==B", "A<B", "A>B" inputs to allow cascading comparators.
// "A==B" input defaults HIGH with an unknown input, but all others default LOW.

Gate_COMPARE::Gate_COMPARE() : Gate_N_INPUT() {
	// Declare the inputs:
	declareInput("in_A_equal_B");
	declareInput("in_A_greater_B");
	declareInput("in_A_less_B");

	// Input busses are declared by Gate_N_INPUT and in setParams():
	setParameter("INPUT_BITS", "0");

	// The outputs:
	declareOutput("A_equal_B");
	declareOutput("A_greater_B");
	declareOutput("A_less_B");
}


// Handle gate events:
void Gate_COMPARE::gateProcess( void ) {
	unsigned long inA = bus_to_ulong( getInputBusState("IN") );
	unsigned long inB = bus_to_ulong( getInputBusState("IN_B") );

	StateType equal = ZERO;
	StateType less = ZERO;
	StateType greater = ZERO;

	if( inA == inB ) {
		if( getInputState("in_A_greater_B") == ONE ) {
			greater = ONE;
		} else if( getInputState("in_A_less_B") == ONE ) {
			less = ONE;
		} else if( getInputState("in_A_equal_B") != ZERO ) {
			equal = ONE;
		}
	} else if( inA < inB ) {
		less = ONE;
	} else if( inA > inB ) {
		greater = ONE;
	}
	
	// Set the output values:
	setOutputState("A_equal_B", equal);
	setOutputState("A_less_B", less);
	setOutputState("A_greater_B", greater);
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
