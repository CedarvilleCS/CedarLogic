

#pragma once
#include "guiGate.h"

class guiGateTOGGLE : public guiGate {
public:
	guiGateTOGGLE();

	void draw(bool color = true);

	void setGUIParam(std::string paramName, std::string value);

	void setLogicParam(std::string paramName, std::string value);

	// Toggle the output button on and off:
	std::string getState();

	Message_SET_GATE_PARAM* checkClick(GLfloat x, GLfloat y);

protected:
	int renderInfo_outputNum;

	GLLine2f renderInfo_clickBox;
};
