
// Tyler Drake 10-03-2016
// Pulled these definitions from logic_defaults.h.
// These are the only features of logic_defaults used outside of the logic_ files.
// This file is designed to be crazy light-weight.

#ifndef LOGIC_VALUES_H
#define LOGIC_VALUES_H
#include <climits>
// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Macros _MSG y _MSGC
// Included here because this include file is very used
#include "../dbgmsg.h"

// Wire state type:
typedef unsigned char StateType;

// The type used to store logic time values:
// Note that we don't make any effort to avoid the time counter from over-flowing.
// For this to happen, even if the machine only does 32-bit long long types, 
// the simulation would have to run constantly for 50 days straight
// even if a time step were taken once per milisecond. So we don't worry about it.
typedef unsigned long long TimeType;

// The type used for internal wire and gate and input IDs:
// Note that we also don't worry about IDType running out of spaces, because an
// unsigned long long can memory reference the entire memory space anyhow.
// Exception: loaded files may have values near the end of this range.
typedef unsigned long long IDType;


// The number of states in the simulator:
const int NUM_STATES = 5;


// The states defined in truth tables:
const StateType ZERO = 0;				// Black
const StateType ONE = 1;				// Red
const StateType HI_Z = 2;				// Green
const StateType CONFLICT = 3;			// Cyan
const StateType UNKNOWN = 4;			// Blue


// The time that represents an unitialized time variable:
const TimeType TIME_NONE = ULLONG_MAX;
const TimeType DEFAULT_GATE_DELAY = 1;


// The ID that represents an unconnected wire or gate:
const IDType ID_NONE = ULLONG_MAX;

#endif