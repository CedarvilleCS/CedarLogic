/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   
*****************************************************************************/

// logic_event.h: interface for the Event class.
// Ben Sprague, 10-15-2005
// Tyler Drake, 10-31-2016
// Moved code to source.
// Move existing comparator from global namespace to class.

#ifndef LOGIC_EVENT_H
#define LOGIC_EVENT_H

#include "logic_defaults.h"

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
	
	Event();

	TimeType getCreationTime() const;

	// Sort events by time.
	// Used in logic_circuit event queue.
	bool operator > (const Event &other) const;

private:
	TimeType myCreationTime;

	static TimeType globalCreationTime;
};


#endif // LOGIC_EVENT_H
