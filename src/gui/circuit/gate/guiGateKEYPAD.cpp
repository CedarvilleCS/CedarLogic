
#include "guiGateKEYPAD.h"
#include <sstream>
#include <iomanip>
#include "../../thread/Message.h"


using namespace std;

guiGateKEYPAD::guiGateKEYPAD() {
	guiGate();
	// Default to 0 when creating:
	//NOTE: Does not send this to the core, just updates it
	// on the GUI side.
	setLogicParam("OUTPUT_NUM", "0");
	keypadValue = "0";

	// All click boxes are in gui params as a list of type KEYPAD_BOX_<val>
	//	param values are of type "minx,miny,maxx,maxy"
}

void guiGateKEYPAD::draw(bool color) {
	// Position the gate at its x and y coordinates:
	glLoadMatrixd(mModel);

	// Add the rectangle - this is a highlight so needs done before main gate draw:
	glColor4f(0.0, 0.4f, 1.0, 0.3f);

	//Inner Square
	if (color) glRectd(renderInfo_valueBox.begin.x, renderInfo_valueBox.begin.y,
		renderInfo_valueBox.end.x, renderInfo_valueBox.end.y);

	// Set the color back to the old color:
	glColor4f(0.0, 0.0, 0.0, 1.0);

	// Draw the default lines:
	guiGate::draw(color);
}

void guiGateKEYPAD::setLogicParam(const std::string &paramName, const std::string &value) {
	if (paramName == "OUTPUT_NUM" || paramName == "OUTPUT_BITS") {
		istringstream iss(paramName == "OUTPUT_NUM" ? value : lparams["OUTPUT_NUM"]);
		int intVal;
		iss >> intVal;
		ostringstream ossVal, ossParamName;
		// Convert to hex
		for (int i = 2 * sizeof(int) - 1; i >= 0; i--) {
			ossVal << "0123456789ABCDEF"[((intVal >> i * 4) & 0xF)];
		}
		iss.clear(); iss.str(paramName == "OUTPUT_BITS" ? value : lparams["OUTPUT_BITS"]);
		iss >> intVal;
		string currentValue = ossVal.str().substr(ossVal.str().size() - (intVal / 4), (intVal / 4));
		ossParamName << "KEYPAD_BOX_" << currentValue;
		if (gparams.find(ossParamName.str()) == gparams.end()) {
			ossParamName.str("");
			map < string, string >::iterator gparamWalk = gparams.begin();
			while (gparamWalk != gparams.end()) {
				if ((gparamWalk->first).substr(0, 11) != "KEYPAD_BOX_") { gparamWalk++; continue; }
				ossParamName << gparamWalk->first;
				break;
			}
		}
		if (ossParamName.str() != "") {
			string clickBox = getGUIParam(ossParamName.str());
			istringstream iss(clickBox);
			char dump;
			iss >> renderInfo_valueBox.begin.x >> dump >> renderInfo_valueBox.begin.y >>
				dump >> renderInfo_valueBox.end.x >> dump >> renderInfo_valueBox.end.y;
		}
	}

	guiGate::setLogicParam(paramName, value);
}

// Check the click boxes for the keypad and set appropriately:
Message_SET_GATE_PARAM* guiGateKEYPAD::checkClick(GLfloat x, GLfloat y) {
	map < string, string >::iterator gparamWalk = gparams.begin();
	while (gparamWalk != gparams.end()) {
		// Is this a keypad box param?
		if ((gparamWalk->first).substr(0, 11) != "KEYPAD_BOX_") { gparamWalk++; continue; }

		klsBBox keyButton;

		// Get the size of the CLICK square from the parameters:
		string clickBox = getGUIParam(gparamWalk->first);
		istringstream iss(clickBox);
		GLdouble minx = -0.5;
		GLdouble miny = -0.5;
		GLdouble maxx = 0.5;
		GLdouble maxy = 0.5;
		char dump;
		iss >> minx >> dump >> miny >> dump >> maxx >> dump >> maxy;

		keyButton.addPoint(modelToWorld(GLPoint2f(minx, miny)));
		keyButton.addPoint(modelToWorld(GLPoint2f(minx, maxy)));
		keyButton.addPoint(modelToWorld(GLPoint2f(maxx, miny)));
		keyButton.addPoint(modelToWorld(GLPoint2f(maxx, maxy)));

		if (keyButton.contains(GLPoint2f(x, y))) {
			// Retrieve the value of the box
			iss.clear();
			keypadValue = (gparamWalk->first).substr(11, (gparamWalk->first).size() - 11);
			iss.str(keypadValue);
			int keypadIntVal;
			// Convert to decimal (cheap hack)
			iss >> setbase(16) >> keypadIntVal;
			ostringstream ossValue;
			ossValue << keypadIntVal;
			setLogicParam("OUTPUT_NUM", ossValue.str());
			/*			ostringstream oss;
			oss << "SET GATE ID " << getID() << " PARAMETER OUTPUT_NUM " << getLogicParam("OUTPUT_NUM"); */
			return new Message_SET_GATE_PARAM(getID(), "OUTPUT_NUM", getLogicParam("OUTPUT_NUM"));
		}
		gparamWalk++;
	}

	return NULL;
}