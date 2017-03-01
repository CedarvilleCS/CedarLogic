
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

	Color oldColor = ColorPalette::getColor();

	switch (outputState) {
	case ZERO:
		ColorPalette::setColor(ColorPalette::WireLow);
		break;
	case ONE:
		ColorPalette::setColor(ColorPalette::WireHigh);
		break;
	case HI_Z:
		ColorPalette::setColor(ColorPalette::WireHiZ);
		break;
	case UNKNOWN:
		ColorPalette::setColor(ColorPalette::WireUnknown);
		break;
	case CONFLICT:
		ColorPalette::setColor(ColorPalette::WireUnknown);
		break;
	}

	//Inner Square
	if (color) glRectd(renderInfo_ledBox.begin.x, renderInfo_ledBox.begin.y,
		renderInfo_ledBox.end.x, renderInfo_ledBox.end.y);

	ColorPalette::setColor(oldColor);
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