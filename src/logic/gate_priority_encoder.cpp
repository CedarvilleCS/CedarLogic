#include "gate_priority_encoder.h"


// **************************** PRIORITY ENCODER GATE ***********************************


// Initialize the gate's interface:
Gate_PRI_ENCODER::Gate_PRI_ENCODER() : Gate_N_INPUT() {

	// The control inputs data inputs are declared in setParameter().
	// (Must be set before using this method!)
	setParameter("INPUT_BITS", "0");

	declareInput("ENABLE");

	// One output:
	declareOutput("OUT");
	declareOutput("VALID");
}


// Handle gate events:
void Gate_PRI_ENCODER::gateProcess(void) {
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
		// Loop through input bits from MSB to LSB to find active
		for (int i = inBus.size()-1; i > 0; i--) {
			if (inBus[i] == ONE) {
				// If this bit is one, then we found MSB for output
				outBus = ulong_to_bus(i, outBusSize);
				break;
			}
		}
		// If input other than zero is recieved, then it's valid
		if (inNum != 0) {
			isValid = true;
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


// Set the parameters:
bool Gate_PRI_ENCODER::setParameter(string paramName, string value) {
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
	}
	else {
		return Gate_N_INPUT::setParameter(paramName, value);
	}
	return false;
}


// **************************** END PRIORITY ENCODER GATE ***********************************//
