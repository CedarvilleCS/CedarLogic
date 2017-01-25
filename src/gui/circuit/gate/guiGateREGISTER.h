
#pragma once
#include "guiGate.h"

class guiGateREGISTER : public guiGate {
public:
	guiGateREGISTER();

	void draw(bool color = true);

	void setGUIParam(std::string paramName, std::string value);

	void setLogicParam(std::string paramName, std::string value);

protected:
	GLLine2f renderInfo_valueBox;

	GLdouble renderInfo_diffx;

	GLdouble renderInfo_diffy;

	bool renderInfo_drawBlue;

	int renderInfo_numDigitsToShow;

	std::string renderInfo_currentValue;
};