
#include "guiTO_FROM.h"

const static GLdouble SELECTED_LABEL_INTENSITY = 0.50;
const static GLdouble TO_FROM_TEXT_HEIGHT = 1.5;
const static GLdouble TO_BUFFER = 0.4;
const static GLdouble FROM_BUFFER = 0.0;
const static GLdouble FROM_FIX_SHIFT = 0.0;
const static GLdouble FLIPPED_OFFSET = 0.5;

guiTO_FROM::guiTO_FROM() {
	// Note that I don't set the JUNCTION_ID parameter yet, because
	// that would call setParam() and that would call calcBBox()
	// and that wants to know that the gate's type is, which we don't know yet.

	guiGate();

	// Initialize the text object:
	theText.setSize(TO_FROM_TEXT_HEIGHT);
}

void guiTO_FROM::draw(bool color) {
	// Draw the lines for this gate:
	guiGate::draw();

	// Position the gate at its x and y coordinates:
	glLoadMatrixd(mModel);

	// Pick the color for the text:
	if (selected && color) {
		Color &c = ColorPalette::TextSelected;
		theText.setColor(c.r, c.g, c.b, c.a);
	}
	else {
		Color &c = ColorPalette::Text;
		theText.setColor(c.r, c.g, c.b, c.a);
	}

	//********************************
	//Edit by Joshua Lansford 04/04/07
	//Upside down text on tos and froms
	//isn't that exciting.
	//This will rotate the text around
	//before it is printed
	if (this->getGUIParam("angle") == "180" ||
		this->getGUIParam("angle") == "90") {

		//scoot the label over
		GLbox textBBox = theText.getBoundingBox();
		GLdouble textWidth = textBBox.right - textBBox.left;
		int direction = 0;
		if (getGUIType() == "TO") {
			direction = +1;
		}
		else if (getGUIType() == "FROM") {
			direction = -1;
		}
		glTranslatef(direction * (textWidth + FLIPPED_OFFSET), 0, 0);

		//and spin it around
		glRotatef(180, 0.0, 0.0, 1.0);
	}
	//End of Edit*********************

	// Draw the text:
	theText.draw();
}

// A custom setParam function is required because
// the object must resize it's bounding box 
// each time the JUNCTION_ID parameter is set.
void guiTO_FROM::setLogicParam(const std::string &paramName, const std::string &value) {
	if (paramName == "JUNCTION_ID") {
		guiGate::setLogicParam(paramName, value);

		string labelText = getLogicParam("JUNCTION_ID");
		theText.setText(labelText);
		theText.setSize(TO_FROM_TEXT_HEIGHT);

		//Sets bounding box size
		this->calcBBox();
	}
	else {
		guiGate::setLogicParam(paramName, value);
	}
}

void guiTO_FROM::calcBBox() {

	// Set the gate's bounding box based on the lines:
	guiGate::calcBBox();

	// Get the text's bounding box:	
	GLbox textBBox = theText.getBoundingBox();

	// Adjust the bounding box based on the text's bbox:
	GLdouble textWidth = textBBox.right - textBBox.left;
	if (getGUIType() == "TO") {
		Point bR = modelBBox.getBottomRight();
		bR.x += textWidth;
		modelBBox.addPoint(bR);
		theText.setPosition(TO_BUFFER, TO_FROM_TEXT_HEIGHT / 2 + 0.30);
	}
	else if (getGUIType() == "FROM") {
		Point tL = modelBBox.getTopLeft();
		tL.x -= (textWidth + FROM_BUFFER);
		modelBBox.addPoint(tL);
		theText.setPosition(tL.x + FROM_FIX_SHIFT, TO_FROM_TEXT_HEIGHT / 2 + 0.30);
	}

	// Recalculate the world-space bbox:
	updateBBoxes();
}