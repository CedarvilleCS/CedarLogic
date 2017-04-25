/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
					 Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.

   GateLibrary: Uses XMLParser to parse library files
*****************************************************************************/

#include "GateLibrary.h"
#include "wx/msgdlg.h"
#include "../MainApp.h"
#include "gui/command/cmdSetParams.h"
#include "gui/command/cmdPasteBlock.h"
#include "quoted.h"
#include "gui/GUICircuit.h"

// Included for sin and cos in <circle> tags:
#include <cmath>

DECLARE_APP(MainApp)

LibraryGateLine::LibraryGateLine(float x1, float y1, float x2, float y2) :
	x1(x1), y1(y1), x2(x2), y2(y2) { }

GateLibrary::GateLibrary(string fileName) {
	fstream x(fileName, ios::in);
	if (!x) {
		// Error loading file, don't bother trying to parse.
		wxString msg;
		ostringstream ossError;
		ossError << "The library file " << fileName << " does not exist.";
		msg.Printf(wxString(ossError.str()));
		wxMessageBox(msg, "Error - Missing File", wxOK | wxICON_ERROR, NULL);

		return;
	}
	mParse = new XMLParser(&x, false);
	this->fileName = fileName;
	parseFile();
	delete mParse;
	numDefinedBlackBoxes = 0;
}

GateLibrary::GateLibrary() {
	return;
}

GateLibrary::~GateLibrary() {
	//delete mParse;
}

void GateLibrary::parseFile() {
	do { // Outer loop to parse all libraries
		// need to throw exception
		if (mParse->readTag() != "library") return;
		mParse->readTag();
		libName = mParse->readTagValue("name");
		mParse->readCloseTag();

		string hsName, hsType;
		float x1, y1;
		char dump;

		do {
			mParse->readTag();
			LibraryGate newGate;
			string temp = mParse->readTag();
			newGate.gateName = mParse->readTagValue(temp);
			mParse->readCloseTag();
			do {
				temp = mParse->readTag();

				if ((temp == "input") || (temp == "output")) {

					string hsType = temp; // The type is determined by the tag name.
					// Assign defaults:
					hsName = "";
					x1 = y1 = 0.0;
					string isInverted = "false";
					string logicEInput = "";
					int busLines = 1;

					do {
						temp = mParse->readTag();
						if (temp == "") break;
						if (temp == "name") {
							hsName = mParse->readTagValue("name");
							mParse->readCloseTag();
						}
						else if (temp == "point") {
							temp = mParse->readTagValue("point");
							istringstream iss(temp);
							iss >> x1 >> dump >> y1;
							mParse->readCloseTag(); //point
						}
						else if (temp == "inverted") {
							isInverted = mParse->readTagValue("inverted");
							mParse->readCloseTag();
						}
						else if (temp == "enable_input") {
							if (hsType == "output") { // Only outputs can have <enable_input> tags.
								logicEInput = mParse->readTagValue("enable_input");
							}
							mParse->readCloseTag();
						}
						else if (temp == "bus") {
							busLines = atoi(mParse->readTagValue("bus").c_str());
							mParse->readCloseTag();
						}

					} while (!mParse->isCloseTag(mParse->getCurrentIndex())); // end input/output

					LibraryGateHotspot hotspot;
					hotspot.name = hsName;
					hotspot.isInput = (hsType == "input");
					hotspot.x = x1;
					hotspot.y = y1;
					hotspot.isInverted = (isInverted == "true");
					hotspot.logicEInput = logicEInput;
					hotspot.busLines = busLines;

					newGate.hotspots.push_back(hotspot);

					mParse->readCloseTag(); //input or output

				}
				else if (temp == "shape") {

					do {
						temp = mParse->readTag();
						if (temp == "") break;
						if (temp == "offset") {
							float offX = 0.0, offY = 0.0;
							temp = mParse->readTag();
							if (temp == "point") {
								temp = mParse->readTagValue("point");
								mParse->readCloseTag();
								istringstream iss(temp);
								iss >> offX >> dump >> offY;
							}
							else {
								//barf
								break;
							}

							do {
								temp = mParse->readTag();
								if (temp == "") break;
								parseShapeObject(temp, &newGate, offX, offY);
							} while (!mParse->isCloseTag(mParse->getCurrentIndex())); // end offset
							mParse->readCloseTag();
						}
						else {
							parseShapeObject(temp, &newGate);
						}
					} while (!mParse->isCloseTag(mParse->getCurrentIndex())); // end shape
					mParse->readCloseTag();

				}
				else if (temp == "param_dlg_data") {

					// Parse the parameters for the params dialog.
					do {
						temp = mParse->readTag();
						if (temp == "") break;
						if (temp == "param") {
							string type = "STRING";
							string textLabel = "";
							string name = "";
							string logicOrGui = "GUI";
							float Rmin = -FLT_MAX, Rmax = FLT_MAX;

							do {
								temp = mParse->readTag();
								if (temp == "") break;
								if (temp == "type") {
									type = mParse->readTagValue("type");
									mParse->readCloseTag();
								}
								else if (temp == "label") {
									textLabel = mParse->readTagValue("label");
									mParse->readCloseTag();
								}
								else if (temp == "varname") {
									temp = mParse->readTagValue("varname");
									istringstream iss(temp);
									iss >> logicOrGui >> name;
									mParse->readCloseTag();
								}
								else if (temp == "range") {
									temp = mParse->readTagValue("range");
									istringstream iss(temp);
									iss >> Rmin >> dump >> Rmax;
									mParse->readCloseTag();
								}
							} while (!mParse->isCloseTag(mParse->getCurrentIndex())); // end param

							LibraryGateDialogParamter param;
							param.textLabel = textLabel;
							param.name = name;
							param.type = type;
							param.isGui = (logicOrGui == "GUI");
							param.Rmin = Rmin;
							param.Rmax = Rmax;

							newGate.dlgParams.push_back(param);
							mParse->readCloseTag();
						}
					} while (!mParse->isCloseTag(mParse->getCurrentIndex())); // end param_dlg_data
					mParse->readCloseTag();

				}
				else if (temp == "gui_type") {
					newGate.guiType = mParse->readTagValue("gui_type");
					mParse->readCloseTag();
				}
				else if (temp == "logic_type") {
					newGate.logicType = mParse->readTagValue("logic_type");
					mParse->readCloseTag();
				}
				else if (temp == "gui_param") {
					string paramName, paramVal;
					istringstream iss(mParse->readTagValue("gui_param"));
					iss >> paramName >> paramVal;
					newGate.guiParams[paramName] = paramVal;
					mParse->readCloseTag();
				}
				else if (temp == "logic_param") {
					string paramName, paramVal;
					istringstream iss(mParse->readTagValue("logic_param"));
					iss >> paramName >> paramVal;
					newGate.logicParams[paramName] = paramVal;
					mParse->readCloseTag();
				}
				else if (temp == "caption") {
					newGate.caption = mParse->readTagValue("caption");
					mParse->readCloseTag();
				}
			} while (!mParse->isCloseTag(mParse->getCurrentIndex())); // end gate
			wxGetApp().gateNameToLibrary[newGate.gateName] = libName;
			wxGetApp().libraries[libName][newGate.gateName] = newGate;
			gates[libName][newGate.gateName] = newGate;
			mParse->readCloseTag(); //gate
		} while (!mParse->isCloseTag(mParse->getCurrentIndex())); // end library
		mParse->readCloseTag(); // clear the close tag
	} while (true); // end file
}

bool GateLibrary::parseShapeObject(string type, LibraryGate* newGate, double offX, double offY) {
	float x1, y1, x2, y2;
	char dump;
	string temp;

	if (type == "line") {
		temp = mParse->readTagValue("line");
		mParse->readCloseTag();
		istringstream iss(temp);
		iss >> x1 >> dump >> y1 >> dump >> x2 >> dump >> y2;

		// Apply the offset:
		x1 += offX; x2 += offX;
		y1 += offY; y2 += offY;
		newGate->shape.push_back(LibraryGateLine(x1, y1, x2, y2));
		return true;
	}
	else if (type == "circle") {
		temp = mParse->readTagValue("circle");
		mParse->readCloseTag();
		istringstream iss(temp);

		double radius = 1.0;
		long numSegs = 12;
		iss >> x1 >> dump >> y1 >> dump >> radius >> dump >> numSegs;
		// Apply the offset:
		x1 += offX; y1 += offY;

		// Generate a circle of the defined shape:
		float theX = 0 + x1;
		float theY = 0 + y1;
		float lastX = x1;//         = sin((double)0)*radius + x1;
		float lastY = radius + y1;//= cos((double)0)*radius + y1;

		float degStep = 360.0 / (float)numSegs;
		for (float i = degStep; i <= 360; i += degStep)
		{
			float degInRad = i * 3.1416f / 180.0f;
			theX = sin(degInRad)*radius + x1;
			theY = cos(degInRad)*radius + y1;
			newGate->shape.push_back(LibraryGateLine(lastX, lastY, theX, theY));
			lastX = theX;
			lastY = theY;
		}
		return true;
	}

	return false; // Invalid type.
}

bool GateLibrary::getGate(string gateName, LibraryGate &lgGate) {
	auto findGate = wxGetApp().gateNameToLibrary.find(gateName);
	if (findGate == wxGetApp().gateNameToLibrary.end()) return false;
	auto findVal = gates[findGate->second].find(gateName);
	if (findVal != gates[findGate->second].end()) lgGate = (findVal->second);
	return (findVal != gates[findGate->second].end());
}

string GateLibrary::getGateLogicType(string gateName) {
	auto findGate = wxGetApp().gateNameToLibrary.find(gateName);
	if (findGate == wxGetApp().gateNameToLibrary.end()) return "";
	if (gates[findGate->second].find(gateName) == gates[findGate->second].end()) return "";
	return gates[findGate->second][gateName].logicType;
}

string GateLibrary::getGateGUIType(string gateName) {
	auto findGate = wxGetApp().gateNameToLibrary.find(gateName);
	if (findGate == wxGetApp().gateNameToLibrary.end()) return "";
	if (gates[findGate->second].find(gateName) == gates[findGate->second].end()) return "";
	return gates[findGate->second][gateName].guiType;
}

void GateLibrary::defineBlackBox(const std::string &copyText) {

	// Store text as an escaped, quoted, string.
	std::ostringstream escapedTextStream;
	escapedTextStream << parse::quoted(copyText);
	std::string escapedText = escapedTextStream.str();

	LibraryGate blackBox;

	blackBox.gateName = "BlackBox#" + std::to_string(numDefinedBlackBoxes);
	numDefinedBlackBoxes++;
	blackBox.caption = blackBox.gateName;
	blackBox.guiType = "BlackBox";
	blackBox.logicType = "BLACK_BOX";
	blackBox.guiParams.insert({ "internals", escapedText });

	// Get names for pins.
	std::vector<std::pair<double, std::string>> pinNamesAndRotation;
	std::string tempText = copyText;
	GUICircuit tempCircuit;
	cmdPasteBlock paste(tempText, false, &tempCircuit, nullptr);  // don't actually use this command.
	for (auto *command : paste.getCommands()) {

		if (command->GetName() == "Set Parameter") {

			cmdSetParams *paramSetter = static_cast<cmdSetParams *>(command);

			double rotation = 0.0;
			for (auto &p : paramSetter->getGuiParameterMap()) {
				if (p.first == "ANGLE") {
					rotation = atof(p.second.c_str());
				}
			}
			for (auto &p : paramSetter->getLogicParameterMap()) {

				if (p.first == "JUNCTION_ID") {
					pinNamesAndRotation.push_back({ rotation, p.second });
				}
			}
		}
	}

	// TODO:
	// subroutine takes lefts, rights, tops, bottoms.
	// returns gate size, hotspot positions.
	// hotspots get added.

	// Make shape.
	auto &lines = blackBox.shape;
	float halfSide = (pinNamesAndRotation.size() + 1) / 2.0f;
	lines.push_back({ -halfSide, halfSide, halfSide, halfSide });
	lines.push_back({ -halfSide, -halfSide, halfSide, -halfSide });
	lines.push_back({ -halfSide, halfSide, -halfSide, -halfSide });
	lines.push_back({ halfSide, halfSide, halfSide, -halfSide });
	for (int i = 0; i < (int)pinNamesAndRotation.size(); i++) {

		lines.push_back({ -halfSide - 1, halfSide - 1 - i, -halfSide, halfSide - 1 - i });
	}

	// Add hotspots.
	auto &hotspots = blackBox.hotspots;
	for (int i = 0; i < (int)pinNamesAndRotation.size(); i++) {

		LibraryGateHotspot h;
		h.busLines = 1;
		h.isInput = true;
		h.isInverted = false;
		h.logicEInput = "";
		h.name = pinNamesAndRotation[i].second;
		h.x = -halfSide - 1;
		h.y = halfSide - 1 - i;

		hotspots.push_back(h);
	}

	// Temporary test label.
	gl_text label;
	label.setText("hello");
	label.setRotation(0.0f);
	label.setColor(0.0f, 0.0f, 0.0f, 1.0f);
	label.setSize(0.85f);
	blackBox.labels.push_back(label);

	// Drop into library.
	gates["11 - Black Boxes"][blackBox.gateName] = blackBox;

	// TODO: i hate that we have to do this.
	wxGetApp().gateNameToLibrary[blackBox.gateName] = "11 - Black Boxes";
	wxGetApp().libraries["11 - Black Boxes"][blackBox.gateName] = blackBox;
}