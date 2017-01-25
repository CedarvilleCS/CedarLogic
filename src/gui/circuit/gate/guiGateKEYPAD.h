
#pragma once
#include "guiGate.h"

class guiGateKEYPAD : public guiGate {
public:
	guiGateKEYPAD();

	void draw(bool color = true);

	void setLogicParam(std::string paramName, std::string value);

	// Toggle the output button on and off:
	Message_SET_GATE_PARAM* checkClick(GLfloat x, GLfloat y);

protected:
	GLLine2f renderInfo_valueBox;

	std::string keypadValue;
};