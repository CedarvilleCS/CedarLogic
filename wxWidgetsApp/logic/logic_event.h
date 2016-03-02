/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   
*****************************************************************************/

// logic_event.h: interface for the Event class.
// Ben Sprague, 10-15-2005
//////////////////////////////////////////////////////////////////////

#ifndef LOGIC_EVENT_H
#define LOGIC_EVENT_H

#include "logic_defaults.h"
#include <ctime>

// Events are simply scheduled wire input state changes. They are used
// by the circuit and gates to store and activate state changes.

class Event
{
public:
// Junction event data:
	bool isJunctionEvent;
	bool newJunctionState;
	IDType junctionID;

// Wire event data:
	StateType newState;  // The new state that will be caused by this event.
	TimeType eventTime;  // The time at which the event will happen.
	IDType wireID;       // The wire that the event will affect.
	IDType gateID;       // The gate that is having a changed output.
	string gateOutputID; // The gate output that is changing.

	// Set default values for all data members:
	Event() {
		isJunctionEvent = false;
		newJunctionState = false;
		junctionID = ID_NONE;

		newState = UNKNOWN;
		eventTime = TIME_NONE;
		wireID = ID_NONE;
		gateID = ID_NONE;
		gateOutputID = "";
		
		// Tag the creation time, for sorting if there are two at the same simulation time:
		myCreationTime = globalCreationTime++;
	};

	unsigned long long getCreationTime( void ) const {
		return myCreationTime;
	};

private:
	unsigned long long myCreationTime;
	static unsigned long long globalCreationTime;
};


#endif // LOGIC_EVENT_H
