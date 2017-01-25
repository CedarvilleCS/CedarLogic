
#pragma once
#include "guiGate.h"
#include "../../gl_text.h"

class guiLabel : public guiGate {
public:
	guiLabel();

	void draw(bool color = true);

	// A convenience function that translates
	// TEXT_HEIGHT parameter into a GLdouble:
	GLdouble getTextHeight();

	// A custom setParam function is required because
	// the object must resize it's bounding box 
	// each time the LABEL_TEXT or TEXT_HEIGHT parameter is set.
	void setGUIParam(std::string paramName, std::string value);

	// Recalculate the label's bounding box:
	void calcBBox();

private:
	gl_text theText;
};