
#include "guiGateREGISTER.h"
#include <sstream>

using namespace std;

guiGateREGISTER::guiGateREGISTER() {
	guiGate();
	// Default to 0 when creating:
	//NOTE: Does not send this to the core, just updates it
	// on the GUI side.
	renderInfo_numDigitsToShow = 1;
	setLogicParam("CURRENT_VALUE", "0");
	setLogicParam("UNKNOWN_OUTPUTS", "false");
}

void guiGateREGISTER::draw(bool color) {
	// Draw the default lines:
	guiGate::draw(color);

	float diffx = renderInfo_diffx;
	float diffy = renderInfo_diffy;

	diffx /= (double)renderInfo_numDigitsToShow; // set width of each digit

												 //Inner Square for value
	if (color) {
		// Display box
		glBegin(GL_LINE_LOOP);
		glVertex2f(renderInfo_valueBox.begin.x, renderInfo_valueBox.begin.y);
		glVertex2f(renderInfo_valueBox.begin.x, renderInfo_valueBox.end.y);
		glVertex2f(renderInfo_valueBox.end.x, renderInfo_valueBox.end.y);
		glVertex2f(renderInfo_valueBox.end.x, renderInfo_valueBox.begin.y);
		glEnd();

		// Draw the number in red (or blue if inputs are not all sane)
		if (renderInfo_drawBlue) glColor4f(0.3f, 0.3f, 1.0, 1.0);
		else glColor4f(1.0, 0.0, 0.0, 1.0);

		GLfloat lineWidthOld;
		glGetFloatv(GL_LINE_WIDTH, &lineWidthOld);
		glLineWidth(2.0);

		// THESE ARE ALL SEVEN SEGMENTS WITH DIFFERENTIAL COORDS.  USE THEM FOR EACH DIGIT VALUE FOR EACH DIGIT
		//		AND INCREMENT CURRENTDIGIT.  CURRENTDIGIT=0 IS MSB.
		glBegin(GL_LINES);
		for (unsigned int currentDigit = 0; currentDigit < renderInfo_currentValue.size(); currentDigit++) {
			char c = renderInfo_currentValue[currentDigit];
			if (c != '1' && c != '4' && c != 'B' && c != 'D') {
				glVertex2f(renderInfo_valueBox.begin.x + (diffx*currentDigit) + (diffx*0.1875), renderInfo_valueBox.begin.y + (diffy*0.88462)); // TOP
				glVertex2f(renderInfo_valueBox.begin.x + (diffx*currentDigit) + (diffx*0.8125), renderInfo_valueBox.begin.y + (diffy*0.88462));
			}
			if (c != '0' && c != '1' && c != '7' && c != 'C') {
				glVertex2f(renderInfo_valueBox.begin.x + (diffx*currentDigit) + (diffx*0.1875), renderInfo_valueBox.begin.y + (diffy*0.5)); // MID
				glVertex2f(renderInfo_valueBox.begin.x + (diffx*currentDigit) + (diffx*0.8125), renderInfo_valueBox.begin.y + (diffy*0.5));
			}
			if (c != '1' && c != '4' && c != '7' && c != '9' && c != 'A' && c != 'F') {
				glVertex2f(renderInfo_valueBox.begin.x + (diffx*currentDigit) + (diffx*0.1875), renderInfo_valueBox.begin.y + (diffy*0.11538)); // BOTTOM
				glVertex2f(renderInfo_valueBox.begin.x + (diffx*currentDigit) + (diffx*0.8125), renderInfo_valueBox.begin.y + (diffy*0.11538));
			}
			if (c != '1' && c != '2' && c != '3' && c != '7' && c != 'D') {
				glVertex2f(renderInfo_valueBox.begin.x + (diffx*currentDigit) + (diffx*0.1875), renderInfo_valueBox.begin.y + (diffy*0.88462)); // TL
				glVertex2f(renderInfo_valueBox.begin.x + (diffx*currentDigit) + (diffx*0.1875), renderInfo_valueBox.begin.y + (diffy*0.5));
			}
			if (c != '5' && c != '6' && c != 'B' && c != 'C' && c != 'E' && c != 'F') {
				glVertex2f(renderInfo_valueBox.begin.x + (diffx*currentDigit) + (diffx*0.8125), renderInfo_valueBox.begin.y + (diffy*0.88462)); // TR
				glVertex2f(renderInfo_valueBox.begin.x + (diffx*currentDigit) + (diffx*0.8125), renderInfo_valueBox.begin.y + (diffy*0.5));
			}
			if (c != '1' && c != '3' && c != '4' && c != '5' && c != '7' && c != '9') {
				glVertex2f(renderInfo_valueBox.begin.x + (diffx*currentDigit) + (diffx*0.1875), renderInfo_valueBox.begin.y + (diffy*0.11538)); // BL
				glVertex2f(renderInfo_valueBox.begin.x + (diffx*currentDigit) + (diffx*0.1875), renderInfo_valueBox.begin.y + (diffy*0.5));
			}
			if (c != '2' && c != 'C' && c != 'E' && c != 'F') {
				glVertex2f(renderInfo_valueBox.begin.x + (diffx*currentDigit) + (diffx*0.8125), renderInfo_valueBox.begin.y + (diffy*0.11538)); // BR
				glVertex2f(renderInfo_valueBox.begin.x + (diffx*currentDigit) + (diffx*0.8125), renderInfo_valueBox.begin.y + (diffy*0.5));
			}
		}
		glEnd();
		glLineWidth(lineWidthOld);
		glColor4f(0.0, 0.0, 0.0, 1.0);
	}
}

void guiGateREGISTER::setGUIParam(const std::string &paramName, const std::string &value) {
	if (paramName == "VALUE_BOX") {
		istringstream iss(value);
		char dump;
		iss >> renderInfo_valueBox.begin.x >> dump >> renderInfo_valueBox.begin.y >>
			dump >> renderInfo_valueBox.end.x >> dump >> renderInfo_valueBox.end.y;
		renderInfo_diffx = renderInfo_valueBox.end.x - renderInfo_valueBox.begin.x;
		renderInfo_diffy = renderInfo_valueBox.end.y - renderInfo_valueBox.begin.y;
	}
	guiGate::setGUIParam(paramName, value);
}

void guiGateREGISTER::setLogicParam(const std::string &paramName, const std::string &value) {
	int intVal;
	if (paramName == "INPUT_BITS") {
		// How many digits should I show? (min of 1)
		istringstream iss(value);
		iss >> renderInfo_numDigitsToShow;
		renderInfo_numDigitsToShow = (int)ceil(((double)renderInfo_numDigitsToShow) / 4.0);
		if (renderInfo_numDigitsToShow == 0) renderInfo_numDigitsToShow = 1;
		if (getLogicParam("CURRENT_VALUE") != "") {
			iss.clear(); iss.str(getLogicParam("CURRENT_VALUE"));
			iss >> intVal;
			ostringstream ossVal;
			for (int i = 2 * sizeof(int) - 1; i >= 0; i--) {
				ossVal << "0123456789ABCDEF"[((intVal >> i * 4) & 0xF)];
			}
			renderInfo_currentValue = ossVal.str().substr(ossVal.str().size() - renderInfo_numDigitsToShow, renderInfo_numDigitsToShow);
		}
	}
	else if (paramName == "UNKNOWN_OUTPUTS") {
		renderInfo_drawBlue = (value == "true");
	}
	else if (paramName == "CURRENT_VALUE") {
		istringstream iss(value);
		iss >> intVal;
		ostringstream ossVal;
		for (int i = 2 * sizeof(int) - 1; i >= 0; i--) {
			ossVal << "0123456789ABCDEF"[((intVal >> i * 4) & 0xF)];
		}
		renderInfo_currentValue = ossVal.str().substr(ossVal.str().size() - renderInfo_numDigitsToShow, renderInfo_numDigitsToShow);
	}
	guiGate::setLogicParam(paramName, value);
}