/*****************************************************************************
Project: CEDAR Logic Simulator
Added by Colin Broberg, 10/20/16
Purpose: A class to dynamically create gates, primarily for the purpose of
heirarchy.
*****************************************************************************/

#ifndef DYNAMICGATE_H_
#define DYNAMICGATE_H_

#include <map>
#include <hash_map>
#include <string>
#include <stack>
#include <vector>
#include "wx/cmdproc.h"
#include "gl_defs.h"
#include "wireSegment.h"
#include "GUICanvas.h"  // GateState, WireState
using namespace std;

class DynamicGate {
	public DynamicGate();
};
