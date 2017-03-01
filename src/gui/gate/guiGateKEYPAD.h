
#pragma once
#include "guiGate.h"

class guiGateKEYPAD : public guiGate {
public:
	guiGateKEYPAD();

	virtual void draw(bool color = true) override;

	virtual void setLogicParam(const std::string &paramName, const std::string &value) override;

	// Toggle the output button on and off:
	virtual Message_SET_GATE_PARAM* checkClick(GLfloat x, GLfloat y) override;

private:
	LineSegment renderInfo_valueBox;

	std::string keypadValue;
};