#pragma once

#include "gate_n_input.h"

// ******************* Decoder Gate *********************
class Gate_DECODER : public Gate_N_INPUT
{
public:
	Gate_DECODER();

	// Handle gate events:
	void gateProcess( void );

	// Set the parameters:
	bool setParameter( string paramName, string value );

protected:
	unsigned long outBits;
};