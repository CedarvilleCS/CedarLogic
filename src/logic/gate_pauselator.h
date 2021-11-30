#pragma once

#include "logic_gate.h"

//***************************************************************
//Edit by Joshua Lansford 6/5/2007
//This edit is to create a new gate called the pauseulator.
//This gate has one input and no outputs.  When the input of this
//gate goes high, then it will pause the simulation.  This takes
//avantage of the pauseing hooks that I had to create for the Z80.
class Gate_pauseulator : public Gate
{
public:
	Gate_pauseulator();
	
	void gateProcess( void );

	bool setParameter( string paramName, string value );

	string getParameter( string paramName );
};
//End of edit****************************************************

