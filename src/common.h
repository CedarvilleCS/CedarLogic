
// Definitions common to both the gui and simulator.

#pragma once
#include <climits>



// The type used for gate and wire id's:
using IDType = unsigned long;

// The ID that represents an unconnected wire or gate:
const IDType ID_NONE = ULONG_MAX;



// Wire state type:
using StateType = unsigned char;

// Wire state possible values:
const StateType ZERO = 0;
const StateType ONE = 1;
const StateType HI_Z = 2;
const StateType CONFLICT = 3;
const StateType UNKNOWN = 4;

// The number of possible states:
const int NUM_STATES = 5;



// The type used to store times:
using TimeType = unsigned long long;

// The time that represents an unitialized time variable:
const TimeType TIME_NONE = ULLONG_MAX;

// Not sure this is necessary...
const TimeType DEFAULT_GATE_DELAY = 1;