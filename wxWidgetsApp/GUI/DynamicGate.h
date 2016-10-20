/*****************************************************************************
Project: CEDAR Logic Simulator
Added by Colin Broberg, 10/20/16
Purpose: A class to dynamically create gates, primarily for the purpose of
heirarchy.
*****************************************************************************/

#ifndef DYNAMICGATE_H_
#define DYNAMICGATE_H_

#include "LibraryParse.h"

using namespace std;

class DynamicGate {
public:
	int inputs;
	int x, y;
	string logicType;
	DynamicGate(int inputs, string lType, int xLoc, int yLoc);

private:
	calculateShape();
	createGate();
};
