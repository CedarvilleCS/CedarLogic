#pragma once

#include "gate_n_input.h"

// ******************* XOR Gate *********************
class Gate_XOR : public Gate_N_INPUT
{
public:
	// Initialize the gate's interface:
	Gate_XOR();
	
	// Handle gate events:
	void gateProcess( void );
};