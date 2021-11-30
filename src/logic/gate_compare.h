#pragma once

#include "gate_n_input.h"

// ******************* Magnitude Comparator Gate *********************
// Compares two input busses by magnitude.
// Outputs "A==B", "A<B", "A>B" depending on results.
// Has "A==B", "A<B", "A>B" inputs to allow cascading comparators.
// "A==B" input defaults HIGH with an unknown input, but all others default LOW.

class Gate_COMPARE : public Gate_N_INPUT
{
public:
	// Initialize the gate's interface:
	Gate_COMPARE();
	
	// Handle gate events:
	void gateProcess( void );

	// Set the parameters:
	bool setParameter( string paramName, string value );
};