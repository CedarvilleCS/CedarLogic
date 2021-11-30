#pragma once

#include "gate_n_input.h"

// ******************* AND Gate *********************
class Gate_AND : public Gate_N_INPUT
{
public:
	// Initialize the gate's interface:
	Gate_AND();
	
	// Handle gate events:
	void gateProcess( void );
};
