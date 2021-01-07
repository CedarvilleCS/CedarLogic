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
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
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
					// Pedro Casanova (casanova@ujaen.es) 2020/04-12
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
							// Pedro Casanova (casanova@ujaen.es) 2020/04-12
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
					// Pedro Casanova (casanova@ujaen.es) 2020/04-12
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

#define SCALE_NORMAL 1.0			// Scale for normal text
#define SCALE_SUB_SUPER 2.0/3.0		// Scale for substring and superstring
#define SCALE_SMALL 2.0/3.0			// Scale for small text
#define SCALE_SMALLER 1.0/3.0		// Scale for smaller text
#define SCALE_BIG 1.5				// Scale for big text
#define SCALE_BIGGER 2.5			// Scale for bigger text
#define CHAR_HEIGHT 0.8				// Heigh of every digit
#define CHAR_BLANK_H 0.1			// Blank space to the top and the bottom of every digit
#define CHAR_WIDTH 0.4				// Width of every digit
#define CHAR_BLANK_W 0.05			// Blank space to the left and the right of every digit

#define CHAR_HEIGHT_TOTAL (CHAR_HEIGHT + 2 * CHAR_BLANK_H)
#define CHAR_WIDTH_TOTAL (CHAR_WIDTH + 2 * CHAR_BLANK_W)

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// <text> to generate text shapes (only numbers and ucase letters)
bool LibraryParse::parseTextObject(LibraryGate* newGate) {
	string temp;
	char dump;
	double cenX = 0.0, cenY = 0.0;
	double dX = 0.0, dY = 0.0;
	double dX0 = 0;
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
	dY = CHAR_HEIGHT / 2;
	for (unsigned int i = 0; i < charCode.size(); i++)
	{
		char c = (char)charCode.c_str()[i];
		if (c != '~' && c != '_' && c != '^' && c != '$' && c != '&' && c != '@' && c != '%' && c != '!') {
			dX -= CHAR_WIDTH_TOTAL / 2 * Scale;
			Scale = SCALE_NORMAL;
		} else {
			switch (c) {
			case '_':
			case '^':
				// Substring and Superstring
				Scale = SCALE_SUB_SUPER;
				break;
			case '$':
				// Small
				Scale = SCALE_SMALL;
				break;
			case '&':
				// Smaller
				Scale = SCALE_SMALLER;
				break;
			case '@':
				// Big
				Scale = SCALE_BIG;
				break;
			case '%':
				// Bigger
				Scale = SCALE_BIGGER;
				break;
			case '!':
				if (dX<dX0)
					dX0 = dX;
				dX = 0;
				dY += CHAR_HEIGHT_TOTAL/2.0 * Scale;
				break;
			}
		}
	}
	if (dX0!=0)
		dX = dX0;
	Scale = SCALE_NORMAL;
	for (unsigned int i = 0; i < charCode.size(); i++)
	{
		switch (charCode.c_str()[i]) {
		case '~':
			// Negate
			Negate = true;
			continue;
		case '_':
			// Substring
			stringType = 1;
			Scale = SCALE_SUB_SUPER;
			continue;
		case '^':
			// Superstring
			stringType = -1;
			Scale = SCALE_SUB_SUPER;
			continue;
		case '$':
			// Small
			Scale = SCALE_SMALL;
			continue;
		case '&':
			// Smaller
			Scale = SCALE_SMALLER;
			continue;
		case '@':
			// Big
			Scale = SCALE_BIG;
			continue;
		case '%':
			// Bigger
			Scale = SCALE_BIGGER;
			continue;
		case '!':
			// New Line
			dX = dX0;
			dY -= CHAR_HEIGHT_TOTAL * Scale;
			continue;
		}

		stringstream XMLstring = getXMLChar((char)charCode.c_str()[i], Negate);

		XMLParser* charParse = new XMLParser((fstream*)&XMLstring, false);

		do {
			temp = charParse->readTag();
			if (temp == "") break;
			parseShapeText(charParse, temp, newGate, stringType, cenX, cenY, dX, dY, Scale);
		} while (!charParse->isCloseTag(charParse->getCurrentIndex()));

		dX += CHAR_WIDTH_TOTAL * Scale;

		Negate = false;
		stringType = 0;
		Scale = SCALE_NORMAL;
	}

	return true;
}

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Lines with offset for rotate chars
// stringType: -1=superstring, 0=normal, 1=substring
// Scale: 1=normal, 2/3=sub and super, 2/3=small, 1.5=big, 2.5=bigger
bool LibraryParse::parseShapeText(XMLParser* Parse, string type, LibraryGate* newGate, int stringType, double cenX, double cenY, double dX, double dY, double Scale) {
	double x1, y1, x2, y2;
	double factor;
	char dump;
	string temp;

	if (type == "line") {
		temp = Parse->readTagValue("line");
		Parse->readCloseTag();
		istringstream iss(temp);
		iss >> x1 >> dump >> y1 >> dump >> x2 >> dump >> y2;

		x1 += CHAR_BLANK_W;
		x2 += CHAR_BLANK_W;

		x1 *= Scale;
		y1 *= Scale;
		x2 *= Scale;
		y2 *= Scale;

		switch (stringType) {
		case 0:
			factor = -CHAR_HEIGHT / 2;
			break;
		case 1:
			// Substring
			factor = -1.0;
			break;
		case -1:
			// Superstring
			factor = 0.2;
			break;
		}

		y1 += (1 - Scale) * factor;
		y2 += (1 - Scale) * factor;

		newGate->Offshape.push_back(lgOffLine(lgLine(x1 + dX, y1 + dY, x2 + dX, y2 + dY), cenX, cenY));

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
		XMLstring << "<line>0,-.5,.1,-.4</line><line>.1,-.4,.3,-.4</line><line>.3,-.4,.4,-.5</line><line>.4,-.5,.4,-.8</line><line>.4,-.6,.1,-.6</line><line>.1,-.6,0,-.7</line><line>0,-.7,.1,-.8</line><line>.1,-.8,.3,-.8</line><line>.3,-.8,.4,-.7</line>";
		break;
	case 'b':
		XMLstring << "<line>0,0,0,-.8</line><line>0,-.5,.1,-.4</line><line>.1,-.4,.3,-.4</line><line>.3,-.4,.4,-.5</line><line>.4,-.5,.4,-.7</line><line>.4,-.7,.3,-.8</line><line>.3,-.8,.1,-.8</line><line>.1,-.8,0,-.7</line>";
		break;
	case 'c':
		XMLstring << "<line>.4,-.5,.3,-.4</line><line>.3,-.4,.1,-.4</line><line>.1,-.4,0,-.5</line><line>0,-.5,0,-.7</line><line>0,-.7,.1,-.8</line><line>.1,-.8,.3,-.8</line><line>.3,-.8,.4,-.7</line>";
		break;
	case 'd':
		XMLstring << "<line>.4,-.5,.3,-.4</line><line>.3,-.4,.1,-.4</line><line>.1,-.4,0,-.5</line><line>0,-.5,0,-.7</line><line>0,-.7,.1,-.8</line><line>.1,-.8,.3,-.8</line><line>.3,-.8,.4,-.7</line><line>.4,0,.4,-.8</line>";
		break;
	case 'e':
		XMLstring << "<line>.0,-.6,.3,-.6</line><line>.3,-.6,.4,-.5</line><line>.4,-.5,.3,-.4</line><line>.3,-.4,.1,-.4</line><line>.1,-.4,0,-.5</line><line>0,-.5,0,-.7</line><line>0,-.7,.1,-.8</line><line>.1,-.8,.3,-.8</line><line>.3,-.8,.4,-.7</line>";
		break;
	case 'f':
		XMLstring << "<line>0.4,-.1,0.3,0</line><line>0.3,0,0.1,0</line><line>0.1,0,0,-0.1</line><line>0,-0.1,0,-0.8</line><line>0.2,-0.4,0,-0.4</line>";
		break;
	case 'g':
		XMLstring << "<line>.4,-.7,.1,-.7</line><line>.1,-.7,0,-.6</line><line>0,-.6,0,-.5</line><line>0,-.5,.1,-.4</line><line>.1,-.4,.3,-.4</line><line>.3,-.4,.4,-.5</line><line>.4,-.5,.4,-.8</line><line>.4,-.8,.3,-.9</line><line>.3,-.9,.1,-.9</line><line>.1,-.9,0,-.8</line>";
		break;
	case 'h':
		XMLstring << "<line>0,0,0,-.8</line><line>0,-.5,.1,-.4</line><line>.1,-.4,.3,-.4</line><line>.3,-.4,.4,-.5</line><line>.4,-.5,.4,-.8</line>";
		break;
	case 'i':
		XMLstring << "<line>.2,-.4,.2,-.7</line><line>.2,-.7,.3,-.8</line><line>.3,-.8,.4,-.7</line><line>.19,-.29,.19,-.31</line><line>.19,-.31,.21,-.31</line><line>.21,-.31,.21,-.29</line><line>.21,-.29,.19,-.29</line>";
		break;
	case 'j':
		XMLstring << "<line>.2,-.4,.2,-.8</line><line>.2,-.8,.1,-.9</line><line>.1,-.9,0,-.8</line><line>.19,-.29,.19,-.31</line><line>.19,-.31,.21,-.31</line><line>.21,-.31,.21,-.29</line><line>.21,-.29,.19,-.29</line>";
		break;
	case 'k':
		XMLstring << "<line>0,0,0,-.8</line><line>0,-.4,.3,-.8</line><line>0,-.4,.2,-.2</line>";
		break;
	case 'l':
		XMLstring << "<line>.1,0,.1,-.7</line><line>.1,-.7,.2,-.8</line><line>.2,-.8,.3,-.8</line><line>.3,-.8,.4,-.7</line>";
		break;
	case 'm':
		XMLstring << "<line>0,-.8,0,-.4</line><line>0,-.5,.1,-.4</line><line>.1,-.4,.2,-.5</line><line>.2,-.5,.2,-.8</line><line>.2,-.5,.3,-.4</line><line>.3,-.4,.4,-.5</line><line>.4,-.5,.4,-.8</line>";
		break;
	case 'n':
		XMLstring << "<line>0,-.4,0,-.8</line><line>0,-.5,.1,-.4</line><line>.1,-.4,.3,-.4</line><line>.3,-.4,.4,-.5</line><line>.4,-.5,.4,-.8</line>";
		break;
	case 'o':
		XMLstring << "<line>0,-.5,.1,-.4</line><line>.1,-.4,.3,-.4</line><line>.3,-.4,.4,-.5</line><line>.4,-.5,.4,-.7</line><line>.4,-.7,.3,-.8</line><line>.3,-.8,.1,-.8</line><line>.1,-.8,0,-.7</line><line>0,-.7,0,-.5</line>";
		break;
	case 'p':
		XMLstring << "<line>0,-.9,0,-.4</line><line>0,-.5,.1,-.4</line><line>.1,-.4,.3,-.4</line><line>.3,-.4,.4,-.5</line><line>.4,-.5,.4,-.6</line><line>.4,-.6,.3,-.7</line><line>.3,-.7,.1,-.7</line><line>.1,-.7,0,-.6</line>";
		break;
	case 'q':
		XMLstring << "<line>.4,-.5,.3,-.4</line><line>.3,-.4,.1,-.4</line><line>.1,-.4,0,-.5</line><line>0,-.5,0,-.6</line><line>0,-.6,.1,-.7</line><line>.1,-.7,.3,-.7</line><line>.3,-.7,.4,-.6</line><line>.4,-.4,.4,-.9</line>";
		break;
	case 'r':
		XMLstring << "<line>0,-.8,0,-.4</line><line>0,-.5,.1,-.4</line><line>.1,-.4,.3,-.4</line><line>.3,-.4,.4,-.5</line>";
		break;
	case 's':
		XMLstring << "<line>.4,-.4,.1,-.4</line><line>.1,-.4,0,-.5</line><line>0,-.5,.1,-.6</line><line>.1,-.6,.3,-.6</line><line>.3,-.6,.4,-.7</line><line>.4,-.7,.3,-.8</line><line>.3,-.8,0,-.8</line>";
		break;
	case 't':
		XMLstring << "<line>.1,0,.1,-.7</line><line>.1,-.7,.2,-.8</line><line>.2,-.8,.3,-.8</line><line>.3,-.8,.4,-.7</line><line>0,-.2,.2,-.2</line>";
		break;
	case 'u':
		XMLstring << "<line>0,-.4,0,-.7</line><line>0,-.7,.1,-.8</line><line>.1,-.8,.3,-.8</line><line>.3,-.8,.4,-.7</line><line>.4,-.4,.4,-.8</line>";
		break;
	case 'v':
		XMLstring << "<line>0,-.4,.2,-.8</line><line>.2,-.8,.4,-.4</line>";
		break;
	case 'w':
		XMLstring << "<line>0,-.4,.1,-.8</line><line>.1,-.8,.2,-.6</line><line>.2,-.6,.3,-.8</line><line>.3,-.8,.4,-.4</line>";
		break;
	case 'x':
		XMLstring << "<line>0,-.4,.4,-.8</line><line>0,-.8,.4,-.4</line>";
		break;
	case 'y':
		XMLstring << "<line>0,-.9,.1,-.9</line><line>.1,-.9,.4,-.4</line><line>0,-.4,.2,-.7</line>";
		break;
	case 'z':
		XMLstring << "<line>0,-.4,.4,-.4</line><line>.4,-.4,0,-.8</line><line>0,-.8,.4,-.8</line>";
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
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
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
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
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
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
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
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
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
