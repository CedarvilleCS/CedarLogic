#include "gate_pass.h"

// ******************************** PASS GATE ***********************************
// This gate simply takes all of the inputs and passes them to the outputs, like
// a buffer. It is useful for creating tri-state buffers and inverters.

// Initialize the gate's interface:
Gate_PASS::Gate_PASS() : Gate_N_INPUT() {
	// Inputs are declared in Gate_N_INPUT();

	// Outputs are declared in setParameter();
	// Default of 1 input (No need to set this in the library file for 1-input gates):
	setParameter("INPUT_BITS", "1");
};


// Handle gate events:
void Gate_PASS::gateProcess(void) {
	// Get the status of all of the inputs:
	vector< StateType > inputStates = getInputBusState("IN");
	vector< StateType > outputStates(inBits, UNKNOWN);

	for (unsigned long i = 0; i < inBits; i++) {
		// If we have a ONE or ZERO, pass it through:
		if ((inputStates[i] == ONE) || (inputStates[i] == ZERO)) {
			outputStates[i] = inputStates[i];
		}
	}

	setOutputBusState("OUT", outputStates);
};


// Set the parameters:
bool Gate_PASS::setParameter(string paramName, string value) {
	istringstream iss(value);
	if (paramName == "INPUT_BITS") {
		iss >> inBits;

		// Declare the output pins:
		if (inBits > 0) {
			declareOutputBus("OUT", inBits);
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


// **************************** END PASS GATE ***********************************