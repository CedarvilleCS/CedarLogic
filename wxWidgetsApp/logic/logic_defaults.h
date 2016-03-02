/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   
*****************************************************************************/

// logic_defaults.h: Default standards for the logic engine.
// Ben Sprague, 10-15-2005

#ifndef LOGIC_DEFAULTS_H
#define LOGIC_DEFAULTS_H

#include "../GUI/product.h"

// Warning message macro:
#ifndef _PRODUCTION_
#define WARNING(message) \
	*logiclog << "Warning: " << message << endl << flush
#else
#define WARNING(message) \
	;
#endif

#include <fstream>
using namespace std;
extern ofstream* logiclog;

// The number of states in the simulator:
#define NUM_STATES 5

// The states defined in truth tables:
#define ZERO     0
#define ONE      1
#define HI_Z     2
#define UNKNOWN  4
#define CONFLICT 3   // this value was 5, but there are not six states, so the state values must be between 0 and 4   KAS

// Type definitions:

// Wire state type:
typedef unsigned char StateType;

// The type used to store logic time values:
// Note that we don't make any effort to avoid the time counter from over-flowing.
// For this to happen, even if the machine only does 32-bit long long types, 
// the simulation would have to run constantly for 50 days straight
// even if a time step were taken once per milisecond. So we don't worry about it.
//typedef unsigned long long TimeType;
typedef unsigned long long TimeType;

// The time that represents an unitialized time variable:
#define TIME_NONE ULLONG_MAX
#define DEFAULT_GATE_DELAY 1

// The type used for internal wire and gate and input IDs:
// Note that we also don't worry about IDType running out of spaces, because an
// unsigned long can memory reference the entire memory space anyhow. ,'o)
//WARNING: (Unless of course, a loaded file gives you an ID that is near the
// top of unsigned long's range to begin with!)
typedef unsigned long IDType;

// The ID that represents an unconnected wire or gate:
#define ID_NONE ULONG_MAX

// Pair-associative and associative containers for IDs:
#define ID_MAP map
#include<map>

#define ID_SET set
#include<set>

#define EVENT_LIST vector
#include<vector>

#define WIRE_MAP map

// Gate and wire pointer definitions:
#include "counted_pointer.h"

#define GATE_PTR counted_ptr< Gate >
#define WIRE_PTR counted_ptr< Wire >
#define JUNC_PTR counted_ptr< Junction >

// Allow use of STL objects:
#include <string>
using namespace std;

#endif // LOGIC_DEFAULTS_H
