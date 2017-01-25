
#pragma once
#include "guiGate.h"
#include "../../gl_text.h"

class guiTO_FROM : public guiGate {
public:
	guiTO_FROM();

	void draw(bool color = true);

	// Recalculate the gate's bounding box:
	void calcBBox();

	// A custom setParam function is required because
	// the object must resize it's bounding box 
	// each time the JUNCTION_ID parameter is set.
	void setLogicParam(std::string paramName, std::string value);

private:
	gl_text theText;
};