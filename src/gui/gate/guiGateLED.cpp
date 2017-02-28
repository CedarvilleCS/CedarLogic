
#include "guiGateLED.h"
#include <sstream>
#include "common.h"
#include "../wire/guiWire.h"

using namespace std;

guiGateLED::guiGateLED() {
	guiGate();

	// Set the default LED box:
	// Format is: "minx miny maxx maxy"
	setGUIParam("LED_BOX", "-0.76,-0.76,0.76,0.76");
}

void guiGateLED::draw(bool color) {
	StateType outputState = HI_Z;

	// Draw the default lines:
	guiGate::draw(color);

	// Get the first connected input in the LED's library description:
	// map i/o name to wire id
	auto theCnk = connections.begin();
	if (theCnk != connections.end()) {
		outputState = (theCnk->second)->getState()[0];
	}

	switch (outputState) {
	case ZERO:
		glColor4f(0.0, 0.0, 0.0, 1.0);
		break;
	case ONE:
		glColor4f(1.0, 0.0, 0.0, 1.0);
		break;
	case HI_Z:
		glColor4f(0.0, 0.78f, 0.0, 1.0);
		break;
	case UNKNOWN:
		glColor4f(0.3f, 0.3f, 1.0, 1.0);
		break;
	case CONFLICT:
		glColor4f(0.0, 1.0, 1.0, 1.0);
		break;
	}

	//Inner Square
	if (color) glRectd(renderInfo_ledBox.begin.x, renderInfo_ledBox.begin.y,
		renderInfo_ledBox.end.x, renderInfo_ledBox.end.y);

	// Set the color back to black:
	glColor4f(0.0, 0.0, 0.0, 1.0);
}

void guiGateLED::setGUIParam(const std::string &paramName, const std::string &value) {
	if (paramName == "LED_BOX") {
		istringstream iss(value);
		char dump;
		iss >> renderInfo_ledBox.begin.x >> dump >> renderInfo_ledBox.begin.y >>
			dump >> renderInfo_ledBox.end.x >> dump >> renderInfo_ledBox.end.y;
	}
	guiGate::setGUIParam(paramName, value);
}