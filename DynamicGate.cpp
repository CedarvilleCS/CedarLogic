/*****************************************************************************
Project: CEDAR Logic Simulator
Added by Colin Broberg, 10/20/16
Purpose: A class to dynamically create gates for the purpose of heirarchy
*****************************************************************************/

#include "DynamicGate.h"

DynamicGate::DynamicGate(int inputs, string lType, int xLoc, int yLoc) {
	this.inputs = inputs;
	this.outputs = 1;
	this.logicType = lType;
	this.x = xLoc;
	this.y = yLoc;
	libGate = new LibraryGate();
	libGate->caption = "This is a dynamic gate.";
	libGate->guiType = "";
	libGate->logicType = lType;
}

DynamicGate::calculateShape() {
	
}