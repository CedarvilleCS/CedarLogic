
#pragma once
#include "guiGate.h"

class guiGateREGISTER : public guiGate {
public:
	guiGateREGISTER();

	virtual void draw(bool color = true) override;

	virtual void setGUIParam(const std::string &paramName, const std::string &value) override;

	virtual void setLogicParam(const std::string &paramName, const std::string &value) override;

private:
	LineSegment renderInfo_valueBox;

	GLdouble renderInfo_diffx;

	GLdouble renderInfo_diffy;

	bool renderInfo_drawBlue;

	int renderInfo_numDigitsToShow;

	std::string renderInfo_currentValue;
};