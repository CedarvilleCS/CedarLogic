#pragma once

#include "gate_n_input.h"

// ******************* Priority Encoder Gate *********************
// Encodes only the highest significant bit value
class Gate_PRI_ENCODER : public Gate_N_INPUT
{
public:
	Gate_PRI_ENCODER();

	// Handle gate events:
	void gateProcess(void);

	// Set the parameters:
	bool setParameter(string paramName, string value);

protected:
	unsigned long outBits;
};