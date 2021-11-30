#pragma once

#include "gate_n_input.h"

// ******************* OR Gate *********************
class Gate_OR : public Gate_N_INPUT
{
public:
	// Initialize the gate's interface:
	Gate_OR();
	
	// Handle gate events:
	void gateProcess( void );
};