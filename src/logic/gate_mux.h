#pragma once

#include "gate_n_input.h"

// ******************* Mux Gate *********************
class Gate_MUX : public Gate_N_INPUT
{
public:
	Gate_MUX();

	// Handle gate events:
	void gateProcess( void );

	// Set the parameters:
	bool setParameter( string paramName, string value );

protected:
	unsigned long selBits;
};
