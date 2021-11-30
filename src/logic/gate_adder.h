#pragma once

#include "gate_pass.h"

// ******************* Full Adder Gate *********************
// Performs an addition of two input busses. Assumes that unknown-type inputs are
// all ZEROs.
// NOTE: Will work with busses of up to 32-bits.

class Gate_ADDER : public Gate_PASS
{
public:
	// Initialize the gate's interface:
	Gate_ADDER();
	
	// Handle gate events:
	void gateProcess( void );

	// Set the parameters:
	bool setParameter( string paramName, string value );
};