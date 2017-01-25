
#pragma once
#include "guiGate.h"

class guiGateLED : public guiGate {
public:
	guiGateLED();

	void draw(bool color = true);

	void setGUIParam(std::string paramName, std::string value);

protected:
	GLLine2f renderInfo_ledBox;
};