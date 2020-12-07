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
	// Pedro Casanova (casanova@ujaen.es) 2020/04-11
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
					// Pedro Casanova (casanova@ujaen.es) 2020/04-11
					// To permit pull-up and pull-down inputs
					string isPullUp = "false";
					string isPullDown = "false";
					string ForceJunction = "false";
					string logicEInput = "";
					int busLines = 1;
					
					do {
						temp = mParse->readTag();
						if (temp == "") break;
						if( temp == "name" ) {
							hsName = mParse->readTagValue("name");
							// Pedro Casanova (casanova@ujaen.es) 2020/04-11
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
							// Pedro casanova (casasanova@ujaen.es) 2020/04-11
							// To permit pull-up inputs
							if (hsType == "input") { // Only inputs can have <pull_up> tags.
								isPullUp = mParse->readTagValue("pull_up");
							}
							mParse->readCloseTag();							
						} else if (temp == "pull_down") {
							// Pedro casanova (casasanova@ujaen.es) 2020/04-11
							// To permit pull-down inputs
							if (hsType == "input") { // Only inputs can have <pull_down> tags.
								isPullDown = mParse->readTagValue("pull_down");
							}
							mParse->readCloseTag();
						} else if (temp == "force_junction") {
							// Pedro casanova (casasanova@ujaen.es) 2020/04-11
							// To force junctions in inputs
							if (hsType == "input") { // Only inputs can have <force_junction> tags.
								ForceJunction = mParse->readTagValue("force_junction");
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

					// Pedro casanova (casasanova@ujaen.es) 2020/04-11
					// To permit pull-up and pull-down inputs and force junctions with only one connection
					newGate.hotspots.push_back(lgHotspot(hsName, (hsType == "input"), x1, y1, (isInverted == "true"), (isPullUp == "true"), (isPullDown == "true"), (ForceJunction == "true"), logicEInput, busLines));

					mParse->readCloseTag(); //input or output

				} else if (temp == "shape") {

					do 
					{
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
						} else if (temp == "text") {		// Text shape
							parseTextObject(&newGate);
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
					// Pedro Casanova (casanova@ujaen.es) 2020/04-11
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

// Pedro Casanova (casanova@ujaen.es) 2020/04-11
// <text> to generate text shapes (only numbers and ucase letters)
bool LibraryParse::parseTextObject(LibraryGate* newGate) {
	string temp;
	char dump;
	double cenX = 0.0, cenY = 0.0;
	double dX = 0.0, dY = 0.0;
	temp = mParse->readTagValue("text");
	mParse->readCloseTag();
	string charCode = "";	
	istringstream iss(temp);
	iss >> cenX >> dump >> cenY >> dump;

	while (true) {
		string charSlice;
		iss >> charSlice;
		charCode += charSlice;
		if (iss.eof())
			break;
		charCode += " ";
	}

	bool Negate = false;
	int stringType = 0;
	double Scale = 1.0;

	for (unsigned int i = 0; i < charCode.size(); i++)
	{
		if ((char)charCode.c_str()[i] != '~' && (char)charCode.c_str()[i] != '_' && (char)charCode.c_str()[i] != '^' && (char)charCode.c_str()[i] != '$')
			dX -= 0.25;
		if ((char)charCode.c_str()[i] == '_' || (char)charCode.c_str()[i] == '^')
			dX += 0.25 * 2.0 / 3.0;													// Substring and Superstring
		if ((char)charCode.c_str()[i] == '$')
			dX += 0; // 0.25 * 2.0 / 3.0;											// Small
	}

	dY = 0.4;

	for (unsigned int i = 0; i < charCode.size(); i++)
	{		
		if ((char)charCode.c_str()[i] == '~') {					// Negate
			Negate = true;
			continue;
		} else if ((char)charCode.c_str()[i] == '_') {			// Substring
			stringType = 1;
			continue;
		} else if ((char)charCode.c_str()[i] == '^') {			// Superstring
			stringType = -1;
			continue;
		} else if ((char)charCode.c_str()[i] == '$') {			// Small
			Scale = 2.0 / 3.0;
			continue;
		}

		stringstream XMLstring = getXMLChar((char)charCode.c_str()[i], Negate);

		XMLParser* charParse = new XMLParser((fstream*)&XMLstring, false);

		do {
			temp = charParse->readTag();
			if (temp == "") break;
			parseShapeText(charParse, temp, newGate, stringType, cenX, cenY, dX, dY, Scale);
		} while (!charParse->isCloseTag(charParse->getCurrentIndex()));

		if (stringType)
			dX += 1.0 / 3.0;
		else
			dX += 1.0 / 2.0 * Scale;

		Negate = false;
		stringType = 0;
		Scale = 1.0;
	}

	return true;
}

// Pedro Casanova (casanova@ujaen.es) 2020/04-11
// Lines with offset for rotate chars
// stringType: -1=superstring, 0=normal, 1=substring
// Scale: 1.0=normal, other=small
bool LibraryParse::parseShapeText(XMLParser* Parse, string type, LibraryGate* newGate, int stringType, double cenX, double cenY, double dX, double dY, double Scale) {
	double x1, y1, x2, y2;
	char dump;
	string temp;

	if (type == "line") {
		temp = Parse->readTagValue("line");
		Parse->readCloseTag();
		istringstream iss(temp);
		iss >> x1 >> dump >> y1 >> dump >> x2 >> dump >> y2;
		if (!stringType) {
			// Normal and small string

			x1 *= Scale;
			y1 *= Scale;
			x2 *= Scale;
			y2 *= Scale;

			double w = x2 - x1;
			double h = y2 - y1;

			x1 += 0.2 * (1 - Scale);
			x2 = x1 + w;
			y1 -= 0.4 * (1 - Scale);
			y2 = y1 + h;
			newGate->Offshape.push_back(lgOffLine(lgLine(x1 + dX + 0.05, y1 + dY, x2 + dX + 0.05, y2 + dY), cenX, cenY));
		} else {
			x1 *= 2.0 / 3.0;
			y1 *= 2.0 / 3.0;
			x2 *= 2.0 / 3.0;
			y2 *= 2.0 / 3.0;
			if (stringType > 0) {
				// Substring
				newGate->Offshape.push_back(lgOffLine(lgLine(x1 + dX + 0.05, y1 + dY - 1.0 / 3.0, x2 + dX + 0.05, y2 + dY - 1.0 / 3.0), cenX, cenY));
			} else {
				// Superstring
				newGate->Offshape.push_back(lgOffLine(lgLine(x1 + dX + 0.05, y1 + dY + 1.0 / 15.0, x2 + dX + 0.05, y2 + dY + 1.0 / 15.0), cenX, cenY));
			}
		}
		return true;
	}
	return false; // Invalid type.
}

stringstream LibraryParse::getXMLChar(char ch, bool Negate) {

	stringstream XMLstring;

	if (Negate) XMLstring << "<line>-0.05,0.1,0.45,0.1</line>";

	switch (ch) {
	case '0':
		XMLstring << "<line>0,0,0.4,0</line><line>0.4,0,0.4,-0.8</line><line>0.4,-0.8,0,-0.8</line><line>0,-0.8,0,0</line>";
		break;
	case '1':
		XMLstring << "<line>0.2,0,0.2,-0.8</line>";
		break;
	case '2':
		XMLstring << "<line>0,0,0.4,0</line><line>0.4,0,0.4,-0.4</line><line>0.4,-0.8,0,-0.8</line><line>0,-0.8,0,-0.4</line><line>0,-0.4,0.4,-0.4</line>";
		break;
	case '3':
		XMLstring << "<line>0,0,0.4,0</line><line>0.4,0,0.4,-0.8</line><line>0.4,-0.4,0,-0.4</line><line>0.4,-0.8,0,-0.8</line>";
		break;
	case '4':
		XMLstring << "<line>0,0,0,-0.4</line><line>0,-0.4,0.4,-0.4</line><line>0.4,0,0.4,-0.8</line>";
		break;
	case '5':
		XMLstring << "<line>0,0,0.4,0</line><line>0,0,0,-0.4</line><line>0,-0.4,0.4,-0.4</line><line>0.4,-0.4,0.4,-0.8</line><line>0.4,-0.8,0,-0.8</line>";
		break;
	case '6':
		XMLstring << "<line>0,0,0,-0.8</line><line>0,0,0.4,0</line><line>0,-0.4,0.4,-0.4</line><line>0,-0.8,0.4,-0.8</line><line>0.4,-0.4,0.4,-0.8</line>";
		break;
	case '7':
		XMLstring << "<line>0,0,0.4,0</line><line>0.4,0,0.4,-0.8</line>";
		break;
	case '8':
		XMLstring << "<line>0,0,0,-0.8</line><line>0,0,0.4,0</line><line>0,-0.4,0.4,-0.4</line><line>0,-0.8,0.4,-0.8</line><line>0.4,0,0.4,-0.8</line>";
		break;
	case '9':
		XMLstring << "<line>0,0,0,-0.4</line><line>0,0,0.4,0</line><line>0,-0.4,0.4,-0.4</line><line>0.4,0,0.4,-0.8</line>";
		break;
	case 'A':
		XMLstring << "<line>0,-0.1,0,-0.8</line><line>0,-0.1,0.1,0</line><line>0.4,-0.1,0.4,-0.8</line><line>0.3,0,0.4,-0.1</line><line>0.1,0,0.3,0</line><line>0,-0.4,0.4,-0.4</line>";
		break;
	case 'B':
		XMLstring << "<line>0,0,0,-0.8</line><line>0,0,0.3,0</line><line>0,-0.4,0.3,-0.4</line><line>0,-0.8,0.3,-0.8</line><line>0.3,0,0.4,-0.1</line><line>0.4,-0.3,0.3,-0.4</line><line>0.3,-0.4,0.4,-0.5</line><line>0.4,-0.7,0.3,-0.8</line><line>0.4,-0.1,0.4,-0.3</line><line>0.4,-0.5,0.4,-0.7</line>";
		break;
	case 'C':
		XMLstring << "<line>0,-0.1,0.1,0</line><line>0.1,0,0.3,0</line><line>0.3,0,0.4,-0.1</line><line>0,-0.7,0.1,-0.8</line><line>0.1,-0.8,0.3,-0.8</line><line>0.3,-0.8,0.4,-0.7</line><line>0,-0.1,0,-0.7</line>";
		break;
	case 'D':
		XMLstring << "<line>0,0,0,-0.8</line><line>0,0,0.2,0</line><line>0.2,0,0.4,-0.2</line><line>0,-0.8,0.2,-0.8</line><line>0.2,-0.8,0.4,-0.6</line><line>0.4,-0.2,0.4,-0.6</line>";
		break;
	case 'E':
		XMLstring << "<line>0,0,0,-0.8</line><line>0,0,0.4,0</line><line>0,-0.4,0.3,-0.4</line><line>0,-0.8,0.4,-0.8</line>";
		break;
	case 'F':
		XMLstring << "<line>0,0,0,-0.8</line><line>0,0,0.4,0</line><line>0,-0.4,0.3,-0.4</line>";
		break;
	case 'G':
		XMLstring << "<line>0,-0.1,0,-0.7</line><line>0,-0.1,0.1,0</line><line>0.1,0,0.3,0</line><line>0.3,0,0.4,-0.1</line><line>0,-0.7,0.1,-0.8</line><line>0.1,-0.8,0.3,-0.8</line><line>0.3,-0.8,0.4,-0.7</line><line>0.4,-0.4,0.4,-0.7</line><line>0.4,-0.4,0.2,-0.4</line>";
		break;
	case 'H':
		XMLstring << "<line>0,0,0,-0.8</line><line>0.4,0,0.4,-0.8</line><line>0,-0.4,0.4,-0.4</line>";
		break;
	case 'I':
		XMLstring << "<line>0,0,0.4,0</line><line>0,-0.8,0.4,-0.8</line><line>0.2,0,0.2,-0.8</line>";
		break;
	case 'J':
		XMLstring << "<line>0,0,0.4,0</line><line>0.2,0,0.2,-0.7</line><line>0.2,-0.7,0.1,-0.8</line><line>0.1,-0.8,0,-0.7</line>";
		break;
	case 'K':
		XMLstring << "<line>0,0,0,-0.8</line><line>0.4,0,0,-0.4</line><line>0,-0.4,0.4,-0.8</line>";
		break;
	case 'L':
		XMLstring << "<line>0,0,0,-0.8</line><line>0,-0.8,0.4,-0.8</line>";
		break;
	case 'M':
		XMLstring << "<line>0,0,0,-0.8</line><line>0.4,0,0.4,-0.8</line><line>0,0,0.2,-0.4</line><line>0.2,-0.4,0.4,0</line>";
		break;
	case 'N':
		XMLstring << "<line>0,0,0,-0.8</line><line>0.4,0,0.4,-0.8</line><line>0,0,0.4,-0.8</line>";
		break;
	case 'O':
		XMLstring << "<line>0,-0.1,0,-0.7</line><line>0,-0.7,0.1,-0.8</line><line>0.1,-0.8,0.3,-0.8</line><line>0.3,-0.8,0.4,-0.7</line><line>0.4,-0.7,0.4,-0.1</line><line>0.4,-0.1,0.3,0</line><line>0.3,0,0.1,0</line><line>0.1,0,0,-0.1</line>";
		break;
	case 'P':
		XMLstring << "<line>0,0,0,-0.8</line><line>0,0,0.3,0</line><line>0,-0.4,0.3,-0.4</line><line>0.3,0,0.4,-0.1</line><line>0.4,-0.1,0.4,-0.3</line><line>0.4,-0.3,0.3,-0.4</line>";
		break;
	case 'Q':
		XMLstring << "<line>0.1,0,0,-0.1</line><line>0,-0.1,0,-0.7</line><line>0,-0.7,0.1,-0.8</line><line>0.1,-0.8,0.3,-0.8</line><line>0.3,-0.8,0.4,-0.7</line><line>0.4,-0.7,0.4,-0.1</line><line>0.4,-0.1,0.3,0</line><line>0.3,0,0.1,0</line><line>0.4,-0.8,0.2,-0.6</line>";
		break;
	case 'R':
		XMLstring << "<line>0,0,0,-0.8</line><line>0,0,0.3,0</line><line>0,-0.4,0.3,-0.4</line><line>0.3,0,0.4,-0.1</line><line>0.4,-0.1,0.4,-0.3</line><line>0.4,-0.3,0.3,-0.4</line><line>0.2,-0.4,0.4,-0.8</line>";
		break;
	case 'S':
		XMLstring << "<line>0.4,0,0.1,0</line><line>0,-0.1,0.1,0</line><line>0,-0.1,0.1,0</line><line>0,-0.1,0,-0.2</line><line>0,-0.2,0.4,-0.6</line><line>0.4,-0.7,0.4,-0.6</line><line>0.4,-0.7,0.3,-0.8</line><line>0.3,-0.8,0,-0.8</line>";
		break;
	case 'T':
		XMLstring << "<line>0,0,0.4,0</line><line>0.2,0,0.2,-0.8</line>";
		break;
	case 'U':
		XMLstring << "<line>0,0,0,-0.7</line><line>0,-0.7,0.1,-0.8</line><line>0.4,0,0.4,-0.7</line><line>0.3,-0.8,0.4,-0.7</line><line>0.1,-0.8,0.3,-0.8</line>";
		break;
	case 'V':
		XMLstring << "<line>0,0,0.2,-0.8</line><line>0.4,0,0.2,-0.8</line>";
		break;
	case 'W':
		XMLstring << "<line>0,0,0.1,-0.8</line><line>0.1,-0.8,0.2,-0.4</line><line>0.3,-0.8,0.2,-0.4</line><line>0.4,0,0.3,-0.8</line>";
		break;
	case 'X':
		XMLstring << "<line>0,0,0.4,-0.8</line><line>0.4,0,0,-0.8</line>";
		break;
	case 'Y':
		XMLstring << "<line>0,0,0.2,-0.4</line><line>0.4,0,0.2,-0.4</line><line>0.2,-0.4,0.2,-0.8</line>";
		break;
	case 'Z':
		XMLstring << "<line>0,0,0.4,0</line><line>0,-0.8,0.4,-0.8</line><line>0,-0.8,0.4,0</line>";
		break;
	case 'a':
		XMLstring << "<line>0,-0.8,0.4,-0.8</line><line>0.4,-0.8,0.4,-0.4</line><line>0.4,-0.4,0,-0.4</line><line>0,-0.8,0,-0.6.</line><line>0,-0.6,0.4,-0.6.</line>";
		break;
	case 'b':
		XMLstring << "<line>0,0,0,-0.8</line><line>0,-0.8,0.4,-0.8</line><line>0.4,-0.8,0.4,-0.4</line><line>0.4,-0.4,0,-0.4.</line>";
		break;
	case 'c':
		XMLstring << "<line>0.4,-0.4,0,-0.4</line><line>0,-0.4,0,-0.8</line><line>0,-0.8,0.4,-0.8</line>";
		break;
	case 'd':
		XMLstring << "<line>0.4,0,0.4,-0.8</line><line>0.4,-0.8,0,-0.8</line><line>0,-0.8,0,-0.4</line><line>0,-0.4,0.4,-0.4</line>";
		break;
	case 'e':
		XMLstring << "<line>0.4,-0.4,0,-0.4</line><line>0,-0.4,0,-0.8</line><line>0,-0.8,0.4,-0.8</line><line>0.4,-0.4,0.4,-0.6</line><line>0.4,-0.6,0,-0.6</line>";
		break;
	case 'f':
		XMLstring << "<line>0.3,0,0.1,0</line><line>0.1,0,0,-0.1</line><line>0,-0.1,0,-0.8</line><line>0.2,-0.4,0,-0.4</line>";
		break;
	case 'g':
		XMLstring << "<line>0,-0.4,0.4,-0.4</line><line>0.4,-0.4,0.4,-0.9</line><line>0.4,-0.7,0,-0.7</line><line>0,-0.7,0,-0.4</line><line>0.4,-0.9,0,-0.9</line>";
		break;
	}

	return XMLstring;

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
		// Pedro Casanova (casanova@ujaen.es) 2020/04-11
		// Wide lines
		temp = mParse->readTagValue("wideline");
		mParse->readCloseTag();
		istringstream iss(temp);
		iss >> x1 >> dump >> y1 >> dump >> x2 >> dump >> y2;

		// Apply the offset:
		x1 += offX; x2 += offX;
		y1 += offY; y2 += offY;
		newGate->shape.push_back(lgLine(x1, y1, x2, y2, 2));
		return true;
	} else if (type == "boldline") {
		// Pedro Casanova (casanova@ujaen.es) 2020/04-11
		// Wide lines
		temp = mParse->readTagValue("boldline");
		mParse->readCloseTag();
		istringstream iss(temp);
		iss >> x1 >> dump >> y1 >> dump >> x2 >> dump >> y2;

		// Apply the offset:
		x1 += offX; x2 += offX;
		y1 += offY; y2 += offY;
		newGate->shape.push_back(lgLine(x1, y1, x2, y2, 3));
		return true;
	} else if (type == "outline") {
		// Pedro Casanova (casanova@ujaen.es) 2020/04-11
		// outlines
		temp = mParse->readTagValue("outline");
		mParse->readCloseTag();
		istringstream iss(temp);
		iss >> x1 >> dump >> y1 >> dump >> x2 >> dump >> y2;

		// Apply the offset:
		x1 += offX; x2 += offX;
		y1 += offY; y2 += offY;
		newGate->shape.push_back(lgLine(x1, y1, x2, y2, 5));
		return true;
	} else if (type == "busline") {
		// Pedro Casanova (casanova@ujaen.es) 2020/04-11
		// bus lines for BUSEND
		temp = mParse->readTagValue("busline");
		mParse->readCloseTag();
		istringstream iss(temp);
		iss >> x1 >> dump >> y1 >> dump >> x2 >> dump >> y2;

		// Apply the offset:
		x1 += offX; x2 += offX;
		y1 += offY; y2 += offY;
		newGate->shape.push_back(lgLine(x1, y1, x2, y2, 10));
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
