#pragma once

#include "logic_gate.h"

// ******************* Driver Gate *********************
// Can be used to drive a bus of n bits to a specific binary number.

class Gate_DRIVER : public Gate
{
public:
	// Initialize the driver gate:
	Gate_DRIVER();

	// Handle gate events:
	void gateProcess( void );

	// Set the current state:
	bool setParameter( string paramName, string value );

	// Get the current state:
	string getParameter( string paramName );

private:
	unsigned long output_num;
	unsigned long outBits;
};