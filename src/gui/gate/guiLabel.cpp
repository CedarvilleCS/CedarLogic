
#include "guiLabel.h"
#include <sstream>

using namespace std;

static GLdouble SELECTED_LABEL_INTENSITY = 0.50;

guiLabel::guiLabel() {
	guiGate();
	// Set default parameters:
	setGUIParam("LABEL_TEXT", "BLANK");
	setGUIParam("TEXT_HEIGHT", "2.0");
}

void guiLabel::draw(bool color) {
	// Position the gate at its x and y coordinates:
	glLoadMatrixd(mModel);

	// Pick the color for the text:
	if (selected && color) {
		GLdouble c = 1.0 - SELECTED_LABEL_INTENSITY;
		theText.setColor(1.0, c / 4, c / 4, SELECTED_LABEL_INTENSITY);
	}
	else {
		theText.setColor(0.0, 0.0, 0.0, 1.0);
	}

	// Draw the text:
	theText.draw();
}

GLdouble guiLabel::getTextHeight() {

	istringstream iss(gparams["TEXT_HEIGHT"]);
	GLdouble textHeight = 1.0;
	iss >> textHeight;

	return textHeight;
};

// A custom setParam function is required because
// the object must resize it's bounding box 
// each time the LABEL_TEXT or TEXT_HEIGHT parameter is set.
void guiLabel::setGUIParam(const std::string &paramName, const std::string &value) {
	if ((paramName == "LABEL_TEXT") || (paramName == "TEXT_HEIGHT")) {

		std::string setValue = value;

		if (paramName == "TEXT_HEIGHT") {
			// Make the text parameter safe:
			istringstream iss(value);
			GLdouble textHeight = 1.0;
			iss >> textHeight;

			if (textHeight < 0) textHeight = -textHeight;
			if (textHeight < 0.01) textHeight = 0.01;

			ostringstream oss;
			oss << textHeight;
			setValue = oss.str();
		}

		guiGate::setGUIParam(paramName, setValue);

		string labelText = getGUIParam("LABEL_TEXT");
		GLdouble height = getTextHeight();
		theText.setSize(height);
		theText.setText(labelText);

		//Sets bounding box size
		this->calcBBox();
	}
	else {
		guiGate::setGUIParam(paramName, value);
	}
}

void guiLabel::calcBBox(void) {
	GLbox textBBox = theText.getBoundingBox();
	float dx = fabs(textBBox.right - textBBox.left) / 2.;
	float dy = fabs(textBBox.top - textBBox.bottom) / 2.;
	double currentX, currentY; theText.getPosition(currentX, currentY);
	theText.setPosition(-dx, +dy);
	modelBBox.reset();
	modelBBox.addPoint(GLPoint2f(textBBox.left - dx, textBBox.bottom + dy));
	modelBBox.addPoint(GLPoint2f(textBBox.right - dx, textBBox.top + dy));

	// Recalculate the world-space bbox:
	updateBBoxes();
}