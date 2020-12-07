/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   LibraryParse: Uses XMLParser to parse library files
*****************************************************************************/

#include "LibraryParse.h"
#include "wx/msgdlg.h"
#include "MainApp.h"

// Included for sin and cos in <circle> tags:
#include <cmath>

DECLARE_APP(MainApp)

LibraryParse::LibraryParse(string fileName) {
	// Pedro Casanova (casanova@ujaen.es) 2020/04-10
	// cl_gatedefs.xml now is in resources
	// You can add a new file (UserLib.xml as default name) with new componentes
	HANDLE hResLib = FindResource(NULL, "CL_GATEDEFS.XML", "BIN");
	DWORD nLenRes = SizeofResource(NULL, (HRSRC)hResLib);
	hResLib = LoadResource(NULL, (HRSRC)hResLib);
	char* ResData = (char*) LockResource(hResLib);
	stringstream XMLLib;
	XMLLib << ResData;
	HANDLE hFileLib = CreateFile(fileName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileLib != INVALID_HANDLE_VALUE)
	{
		DWORD nLenFile = GetFileSize(hFileLib, NULL);
		DWORD nReaded;
		char* FileData = new char[nLenFile];
		ReadFile(hFileLib, FileData, nLenFile, &nReaded, NULL);
		CloseHandle(hFileLib);
		XMLLib << FileData;
	}
	mParse = new XMLParser((fstream*)&XMLLib, false);
	this->fileName = fileName;
	parseFile();
	delete mParse;
}

LibraryParse::LibraryParse() {
	return;
}

LibraryParse::~LibraryParse() {
	//delete mParse;
}

// Added by Colin Broberg 11/16/16 -- need to make this a public function so that I can use it for dynamic gates
void LibraryParse::addGate(string libName, LibraryGate newGate) {
	gates[libName][newGate.gateName] = newGate;
}

void LibraryParse::parseFile() {
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

				if ( (temp == "input") || (temp == "output") ) {

					string hsType = temp; // The type is determined by the tag name.
					// Assign defaults:
					hsName = "";
					x1 = y1 = 0.0;
					string isInverted = "false";
					// Pedro Casanova (casanova@ujaen.es) 2020/04-10
					// To permit pull-up and pull-down inputs
					string isPullUp = "false";
					string isPullDown = "false";
					string logicEInput = "";
					int busLines = 1;
					
					do {
						temp = mParse->readTag();
						if (temp == "") break;
						if( temp == "name" ) {
							hsName = mParse->readTagValue("name");
							// Pedro Casanova (casanova@ujaen.es) 2020/04-10
							// Convert to uppercase
							for (unsigned long cnt = 0; cnt < hsName.length(); cnt++)
								hsName[cnt]=toupper(hsName[cnt]);
							if (hsName == "ENABLE_0")
								hsName = "OUTPUT_ENABLE";
							mParse->readCloseTag();
						} else if( temp == "point" ) {
							temp = mParse->readTagValue("point");
							istringstream iss(temp);
							iss >> x1 >> dump >> y1;
							mParse->readCloseTag(); //point
						} else if( temp == "inverted" ) {
							isInverted = mParse->readTagValue("inverted");
							mParse->readCloseTag();
						} else if (temp == "pull_up") {
							// Pedro casanova (casasanova@ujaen.es) 2020/04-10
							// To permit pull-up inputs
							if (hsType == "input") { // Only inputs can have <pull_up> tags.
								isPullUp = mParse->readTagValue("pull_up");
							}
							mParse->readCloseTag();							
						} 	else if (temp == "pull_down") {
							// Pedro casanova (casasanova@ujaen.es) 2020/04-10
							// To permit pull-down inputs
							if (hsType == "input") { // Only inputs can have <pull_down> tags.
								isPullDown = mParse->readTagValue("pull_down");
							}
							mParse->readCloseTag();
						} else if( temp == "enable_input" ) {
							if( hsType == "output" ) { // Only outputs can have <enable_input> tags.
								logicEInput = mParse->readTagValue("enable_input");
							}
							mParse->readCloseTag();
						}
						else if (temp == "bus") {
							busLines = atoi(mParse->readTagValue("bus").c_str());
							mParse->readCloseTag();
						}

					} while (!mParse->isCloseTag(mParse->getCurrentIndex())); // end input/output

					// Pedro casanova (casasanova@ujaen.es) 2020/04-10
					// To permit pull-up and pull-down inputs

					newGate.hotspots.push_back(lgHotspot(hsName, (hsType == "input"), x1, y1, (isInverted == "true"), (isPullUp == "true"), (isPullDown == "true"), logicEInput, busLines));

					mParse->readCloseTag(); //input or output

				} else if (temp == "shape") {

					do {
						temp = mParse->readTag();
						if (temp == "") break;
						if( temp == "offset" ) {
							float offX = 0.0, offY = 0.0;
							temp = mParse->readTag();
							if( temp == "point" ) {
								temp = mParse->readTagValue("point");
								mParse->readCloseTag();
								istringstream iss(temp);
								iss >> offX >> dump >> offY;
							} else {
								//barf
								break;
							}
	
							do {
								temp = mParse->readTag();
								if (temp == "") break;
								parseShapeObject( temp, &newGate, offX, offY );
							} while (!mParse->isCloseTag(mParse->getCurrentIndex())); // end offset
							mParse->readCloseTag();
						} else {
							parseShapeObject( temp, &newGate );
						}
					} while (!mParse->isCloseTag(mParse->getCurrentIndex())); // end shape
					mParse->readCloseTag();

				} else if (temp == "param_dlg_data") {

					// Parse the parameters for the params dialog.
					do {
						temp = mParse->readTag();
						if (temp == "") break;
						if( temp == "param" ) {
							string type = "STRING";
							string textLabel = "";
							string name = "";
							string logicOrGui = "GUI";
							float Rmin = -FLT_MAX, Rmax = FLT_MAX;
	
							do {
								temp = mParse->readTag();
								if (temp == "") break;
								if( temp == "type" ) {
									type = mParse->readTagValue("type");
									mParse->readCloseTag();
								} else if( temp == "label" ) {
									textLabel = mParse->readTagValue("label");
									mParse->readCloseTag();
								} else if( temp == "varname" ) {
									temp = mParse->readTagValue("varname");
									istringstream iss(temp);
									iss >> logicOrGui >> name;
									mParse->readCloseTag();
								} else if( temp == "range" ) {
									temp = mParse->readTagValue("range");
									istringstream iss(temp);
									iss >> Rmin >> dump >> Rmax;
									mParse->readCloseTag();
								}
							} while (!mParse->isCloseTag(mParse->getCurrentIndex())); // end param
							newGate.dlgParams.push_back( lgDlgParam( textLabel, name, type, (logicOrGui == "GUI"), Rmin, Rmax ) );
							mParse->readCloseTag();
						}
					} while (!mParse->isCloseTag(mParse->getCurrentIndex())); // end param_dlg_data
					mParse->readCloseTag();

				} else if (temp == "gui_type") {
					newGate.guiType = mParse->readTagValue("gui_type");
					mParse->readCloseTag();
				} else if (temp == "logic_type") {
					newGate.logicType = mParse->readTagValue("logic_type");
					mParse->readCloseTag();
				} else if (temp == "gui_param") {
					string paramName, paramVal;
					istringstream iss(mParse->readTagValue("gui_param"));
					iss >> paramName >> paramVal;
					newGate.guiParams[paramName] = paramVal;
					mParse->readCloseTag();
				} else if (temp == "logic_param") {					
					string paramName, paramVal;
					istringstream iss(mParse->readTagValue("logic_param"));
					iss >> paramName >> paramVal;
					newGate.logicParams[paramName] = paramVal;
					mParse->readCloseTag();
				} else if (temp == "caption") {
					newGate.caption = mParse->readTagValue("caption");
					// Pedro Casanova (casanova@ujaen.es) 2020/04-10
					//## What's that?
					/*if (newGate.caption == "Inverter" && (time(0) % 1001 == 0)) { // Easter egg, rename inverters once in a while :)
						newGate.caption = "Santa Hat (Inverter)";
					}*/
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

// Parse the shape object from the mParse file, adding an offset if needed:
bool LibraryParse::parseShapeObject( string type, LibraryGate* newGate, double offX, double offY ) {
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
		newGate->shape.push_back(lgLine(x1, y1, x2, y2));
		return true;
	} else if (type == "wideline") {
		// Pedro Casanova (casanova@ujaen.es) 2020/04-10
		// Wide lines for BUSEND
		temp = mParse->readTagValue("wideline");
		mParse->readCloseTag();
		istringstream iss(temp);
		iss >> x1 >> dump >> y1 >> dump >> x2 >> dump >> y2;

		// Apply the offset:
		x1 += offX; x2 += offX;
		y1 += offY; y2 += offY;
		newGate->shape.push_back(lgLine(x1, y1, x2, y2, 4));
		return true;
	} else if( type == "circle" ) {
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

		float degStep = 360.0 / (float) numSegs;
		for (float i=degStep; i <= 360; i += degStep)
		{
			float degInRad = i*DEG2RAD;
			theX = sin(degInRad)*radius + x1;
			theY = cos(degInRad)*radius + y1;
			newGate->shape.push_back( lgLine(lastX, lastY, theX, theY) );
			lastX = theX;
			lastY = theY;
		}
		return true;
	}
	
	return false; // Invalid type.
}

bool LibraryParse::getGate(string gateName, LibraryGate &lgGate) {
	map < string, string >::iterator findGate = wxGetApp().gateNameToLibrary.find(gateName);
	if (findGate == wxGetApp().gateNameToLibrary.end()) return false;
	map < string, LibraryGate >::iterator findVal = gates[findGate->second].find(gateName);
	if (findVal != gates[findGate->second].end()) lgGate = (findVal->second);
	return (findVal != gates[findGate->second].end());
}

// Return the logic type of a particular gate:
string LibraryParse::getGateLogicType( string gateName ) {
	map < string, string >::iterator findGate = wxGetApp().gateNameToLibrary.find(gateName);
	if (findGate == wxGetApp().gateNameToLibrary.end()) return "";
	if ( gates[findGate->second].find(gateName) == gates[findGate->second].end() ) return "";
	return gates[findGate->second][gateName].logicType;
}

// Return the gui type of a particular gate type:
string LibraryParse::getGateGUIType( string gateName ) {
	map < string, string >::iterator findGate = wxGetApp().gateNameToLibrary.find(gateName);
	if (findGate == wxGetApp().gateNameToLibrary.end()) return "";
	if ( gates[findGate->second].find(gateName) == gates[findGate->second].end() ) return "";
	return gates[findGate->second][gateName].guiType;
}
