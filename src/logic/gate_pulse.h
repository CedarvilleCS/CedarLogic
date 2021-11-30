#pragma once

#include "logic_gate.h"

// ******************* Pulse Gate *********************
class Gate_PULSE : public Gate
{
public:
	// Initialize the clock:
	Gate_PULSE();

	// Handle gate events:
	void gateProcess( void );

	// Set the pulse:
	bool setParameter( string paramName, string value );
private:
	TimeType pulseRemaining;
};