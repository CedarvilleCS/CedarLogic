#pragma once

#include "logic_gate.h"

// ******************* Clock Gate *********************
class Gate_CLOCK : public Gate
{
public:
	// Initialize the clock:
	Gate_CLOCK( TimeType newHalfCycle = 0 );

	// Handle gate events:
	void gateProcess( void );

	// Set the clock rate:
	bool setParameter( string paramName, string value );

	// Get the clock rate:
	string getParameter( string paramName );

private:
	TimeType halfCycle;
	StateType theState;
};