
#pragma once
#include "guiGate.h"
#include "../graphics/gl_text.h"

class guiTO_FROM : public guiGate {
public:
	guiTO_FROM();

	virtual void draw(bool color = true) override;

	// A custom setParam function is required because
	// the object must resize it's bounding box 
	// each time the JUNCTION_ID parameter is set.
	virtual void setLogicParam(const std::string &paramName, const std::string & value) override;

	// Recalculate the gate's bounding box:
	virtual void calcBBox() override;

private:
	gl_text theText;
};