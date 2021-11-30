#pragma once

#include "gate_pass.h"

// ******************* Register Gate *********************
class Gate_REGISTER : public Gate_PASS
{
public:
	Gate_REGISTER();

	// Handle gate events:
	void gateProcess( void );

	// Set the parameters:
	bool setParameter( string paramName, string value );

	// Get the parameters:
	string getParameter( string paramName );

protected:
	bool syncSet, syncClear, syncLoad, disableHold, unknownOutputs;

	// The maximum count of this counter (maximum value).
	// (BCD is 9, 4-bit binary is 15.)
	unsigned long maxCount;

	unsigned long currentValue;

	// An initialization value, to make REGISTERs initialize more
	// nicely when loading them or making new ones:
	bool firstGateProcess;

	// The clock pin had a triggering edge and clocking is enabled,
	// or the register isn't synched to a clock.
	bool hasClockEdge();
};
