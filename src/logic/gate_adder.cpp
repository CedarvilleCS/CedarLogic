#include "gate_adder.h"



// ******************************** Full Adder GATE ***********************************
// Performs an addition of two input busses. Assumes that unknown-type inputs are
// all ZEROs.
// NOTE: Will work with busses of up to 32-bits.

Gate_ADDER::Gate_ADDER() : Gate_PASS() {
	// Declare the inputs:
	declareInput("carry_in");

	// (Load input bus and the output bus are declared by Gate_PASS and in setParams.):
	setParameter("INPUT_BITS", "0");

	// The outputs:
	declareOutput("carry_out");
	declareOutput("overflow");
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
	if( getInputState("carry_in") == ONE ) sum++;

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
	setOutputState("carry_out", carryOut);
	setOutputState("overflow", overflow);
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

