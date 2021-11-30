#pragma once
#include "gate_n_input.h"

// ****************** EQUIVALENCE Gate **************
class Gate_EQUIVALENCE : public Gate_N_INPUT
{
public:
	// Initialize the gate's interface:
	Gate_EQUIVALENCE();
	
	// Handle gate events:
	void gateProcess( void );
};