/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   
*****************************************************************************/

// logic_defaults.h: Default standards for the logic engine.
// Ben Sprague, 10-15-2005
// Tyler Drake, 10-03-2016 - replace preprocessor defines with constants.

#ifndef LOGIC_DEFAULTS_H
#define LOGIC_DEFAULTS_H


// Warning message macro:
#ifndef _PRODUCTION_
#define WARNING(message) \
	do { *logiclog << "Warning: " << message << std::endl << std::flush; } while (0);
#else
#define WARNING(message) \
	do { } while (0);
#endif


// Remove logiclog for release builds:
#ifndef _PRODUCTION_
#include <fstream>
extern std::ofstream* logiclog;
#endif


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
const StateType ZERO = 0;
const StateType ONE = 1;
const StateType HI_Z = 2;
const StateType CONFLICT = 3;
const StateType UNKNOWN = 4;


// The time that represents an unitialized time variable:
const TimeType TIME_NONE = ULLONG_MAX;
const TimeType DEFAULT_GATE_DELAY = 1;


// The ID that represents an unconnected wire or gate:
const IDType ID_NONE = ULLONG_MAX;


#include <memory>
#include <map>
#include <set>
#include <vector>


// Pair-associative and associative containers for IDs:
template <typename T, typename U>
using ID_MAP = std::map<T, U>;

template <typename T>
using ID_SET = std::set<T>;

template <typename T>
using EVENT_LIST = std::vector<T>;

template <typename T, typename U>
using WIRE_MAP = std::map<T, U>;


// Forward declare types:
class Gate;
class Wire;
class Junction;


// Gate and wire pointer definitions:
typedef std::shared_ptr<Gate>     GATE_PTR;
typedef std::shared_ptr<Wire>     WIRE_PTR;
typedef std::shared_ptr<Junction> JUNC_PTR;


// Outside classes depend on these... :(
#include <string>
#include <cmath>
using namespace std;


#endif // LOGIC_DEFAULTS_H
