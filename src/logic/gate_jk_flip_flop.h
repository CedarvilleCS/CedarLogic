#pragma once

#include "logic_gate.h"

// ******************* JK Flip Flop Gate *********************
//NOTE: For all inputs, UNKNOWN states are interpreted as 0.

class Gate_JKFF : public Gate
{
public:
	// Initialize the gate's interface:
	Gate_JKFF();
	
	// Handle gate events:
	void gateProcess( void );

	// Set the parameters:
	bool setParameter( string paramName, string value );

	// Get the parameters:
	string getParameter( string paramName );

protected:
	StateType currentState;
	bool syncSet, syncClear;
};