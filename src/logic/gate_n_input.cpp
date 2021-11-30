using namespace std;

#include "gate_n_input.h"

// This gate type has no logic inside of it, but it will declare and manage
// the N inputs. This is good for gates of a similar shape.

// Initialize the gate's interface:
Gate_N_INPUT::Gate_N_INPUT() : Gate() {
	// Default of 0 inputs (Must be specified in library file, or no inputs will be made!):
	setParameter("INPUT_BITS", "0");
}


// Set the parameters:
bool Gate_N_INPUT::setParameter(string paramName, string value) {
	istringstream iss(value);
	if (paramName == "INPUT_BITS") {
		iss >> inBits;

		// Declare the address pins!		
		if (inBits > 0) {
			declareInputBus("IN", inBits);
		}

		//NOTE: Don't return "true" from this, because
		// you shouldn't be setting this param during simulation while
		// anything is connected anyhow!
	}
	else {
		return Gate::setParameter(paramName, value);
	}
	return false;
}


// Set the parameters:
string Gate_N_INPUT::getParameter(string paramName) {
	ostringstream oss;
	if (paramName == "INPUT_BITS") {
		oss << inBits;
		return oss.str();
	}
	else {
		return Gate::getParameter(paramName);
	}
}
