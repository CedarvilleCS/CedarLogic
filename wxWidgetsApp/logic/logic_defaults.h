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


#include "logic_values.h"


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


#include <memory>
#include <map>
#include <set>
#include <vector>


// Pair-associative and associative containers for IDs:
template <typename T, typename U>
using ID_MAP = std::map<T, U>;

template <typename T>
using ID_SET = std::set<T>;


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
