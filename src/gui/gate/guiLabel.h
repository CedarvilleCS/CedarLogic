
#pragma once
#include "guiGate.h"
#include "../graphics/gl_text.h"

class guiLabel : public guiGate {
public:
	guiLabel();

	virtual void draw(bool color = true) override;

	// A convenience function that translates
	// TEXT_HEIGHT parameter into a GLdouble:
	GLdouble getTextHeight();

	// A custom setParam function is required because
	// the object must resize it's bounding box 
	// each time the LABEL_TEXT or TEXT_HEIGHT parameter is set.
	virtual void setGUIParam(const std::string & paramName, const std::string & value) override;

	// Recalculate the label's bounding box:
	virtual void calcBBox() override;

private:
	gl_text theText;
};