

#pragma once
#include "guiGate.h"

class guiGateTOGGLE : public guiGate {
public:
	guiGateTOGGLE();

	// Toggle the output button on and off:
	std::string getState();

	virtual void draw(bool color = true) override;

	virtual void setGUIParam(const std::string &paramName, const std::string &value) override;

	virtual void setLogicParam(const std::string & paramName, const std::string & value) override;

	virtual Message_SET_GATE_PARAM* checkClick(GLfloat x, GLfloat y) override;

protected:
	int renderInfo_outputNum;

	GLLine2f renderInfo_clickBox;
};
