/*****************************************************************************
Project: CEDAR Logic Simulator
Added by Colin Broberg, 10/20/16
Purpose: A class to dynamically create gates, primarily for the purpose of
heirarchy.
*****************************************************************************/

#ifndef DYNAMICGATE_H_
#define DYNAMICGATE_H_

#include "LibraryParse.h"
#include "circuit/guiGates.h"
#include "widget/GUICanvas.h"

using namespace std;

class DynamicGate {
public:
	const int MIN_HEIGHT = 5;
	const int MIN_WIDTH = 4;
	int inputs;
	int outputs;
	int x, y;
	string libName = "Dynamic Gates";
	string name;
	string caption;
	string logicType;
	LibraryGate libGate;
	DynamicGate(GUICanvas* canvas, GUICircuit* circuit, unsigned long gid, int in, int xLoc, int yLoc, string lType, 
				string gName = "Black Box", string gCaption = "This is a black box.", string guiType = "");

private:
	void calculateShape();
	void calculateParams();
	void addGateToLibrary();
	void createGate(GUICanvas* canvas, GUICircuit* circuit, unsigned long gid, float xLoc, float yLoc);
};

#endif