
#include "guiGateTOGGLE.h"
#include <sstream>
#include "../message/Message.h"

using namespace std;

guiGateTOGGLE::guiGateTOGGLE() {
	guiGate();
	// Default to "off" state when creating a toggle gate:
	//NOTE: Does not send this to the core, just updates it
	// on the GUI side.
	setLogicParam("OUTPUT_NUM", "0");

	// Set the default CLICK box:
	// Format is: "minx miny maxx maxy"
	setGUIParam("CLICK_BOX", "-0.76,-0.76,0.76,0.76");
}

string guiGateTOGGLE::getState() {
	return getLogicParam("TOGGLE_STATE");
}

void guiGateTOGGLE::draw(bool color) {
	// Draw the default lines:
	guiGate::draw(color);

	Color oldColor = ColorPalette::getColor();

	// Add the rectangle:
	if (renderInfo_outputNum == 1) {
		ColorPalette::setColor(ColorPalette::WireHigh);
	}
	else {
		ColorPalette::setColor(ColorPalette::WireLow);
	}

	//Inner Square
	if (color) glRectd(renderInfo_clickBox.begin.x, renderInfo_clickBox.begin.y,
		renderInfo_clickBox.end.x, renderInfo_clickBox.end.y);

	ColorPalette::setColor(oldColor);
}

void guiGateTOGGLE::setGUIParam(const std::string & paramName, const std::string & value) {
	if (paramName == "CLICK_BOX") {
		istringstream iss(value);
		char dump;
		iss >> renderInfo_clickBox.begin.x >> dump >> renderInfo_clickBox.begin.y >>
			dump >> renderInfo_clickBox.end.x >> dump >> renderInfo_clickBox.end.y;
	}
	guiGate::setGUIParam(paramName, value);
}

void guiGateTOGGLE::setLogicParam(const std::string & paramName, const std::string & value) {
	if (paramName == "OUTPUT_NUM") {
		istringstream iss(value);
		iss >> renderInfo_outputNum;
	}
	guiGate::setLogicParam(paramName, value);
}

// Toggle the output button on and off:
Message_SET_GATE_PARAM* guiGateTOGGLE::checkClick(GLfloat x, GLfloat y) {
	klsBBox toggleButton;

	// Get the size of the CLICK square from the parameters:
	string clickBox = getGUIParam("CLICK_BOX");
	istringstream iss(clickBox);
	GLdouble minx = -0.5;
	GLdouble miny = -0.5;
	GLdouble maxx = 0.5;
	GLdouble maxy = 0.5;
	char dump;
	iss >> minx >> dump >> miny >> dump >> maxx >> dump >> maxy;

	toggleButton.addPoint(modelToWorld(GLPoint2f(minx, miny)));
	toggleButton.addPoint(modelToWorld(GLPoint2f(minx, maxy)));
	toggleButton.addPoint(modelToWorld(GLPoint2f(maxx, miny)));
	toggleButton.addPoint(modelToWorld(GLPoint2f(maxx, maxy)));

	if (toggleButton.contains(GLPoint2f(x, y))) {
		setLogicParam("OUTPUT_NUM", (getLogicParam("OUTPUT_NUM") == "0") ? "1" : "0");
		/*		ostringstream oss;
		oss << "SET GATE ID " << getID() << " PARAMETER OUTPUT_NUM " << getLogicParam("OUTPUT_NUM"); */
		return new Message_SET_GATE_PARAM(getID(), "OUTPUT_NUM", getLogicParam("OUTPUT_NUM"));
	}
	else return NULL;
}