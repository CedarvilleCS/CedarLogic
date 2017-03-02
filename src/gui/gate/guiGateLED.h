
#pragma once
#include "guiGate.h"
#include "gui/graphics/LineSegment.h"

class guiGateLED : public guiGate {
public:
	guiGateLED();

	virtual void draw(bool color = true) override;

	virtual void setGUIParam(const std::string &paramName, const std::string &value) override;

protected:
	LineSegment renderInfo_ledBox;
};