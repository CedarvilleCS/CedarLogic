#pragma once

#include "logic/logic_gate.h"

// ******************* Gate_N_INPUT parent Gate *********************
class Gate_N_INPUT : public Gate
{
public:
	// Initialize the gate's interface:
	Gate_N_INPUT();

	// Handle gate events:
	void gateProcess(void) {};

	// Set the parameters:
	bool setParameter(string paramName, string value);

	// Get the parameters:
	string getParameter(string paramName);

protected:
	// The number of input bits:
	unsigned long inBits;
};