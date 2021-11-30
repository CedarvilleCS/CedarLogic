#pragma once

#include "gate_n_input.h"

// (It basically takes all inputs and passes them to the outputs.)
class Gate_PASS : public Gate_N_INPUT
{
public:
	// Initialize the gate's interface:
	Gate_PASS();

	// Handle gate events:
	void gateProcess(void);

	// Set the parameters:
	bool setParameter(string paramName, string value);
};
