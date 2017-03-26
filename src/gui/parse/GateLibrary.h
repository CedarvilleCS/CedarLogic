/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   GateLibrary: Uses XMLParser to parse library files
*****************************************************************************/

#pragma once

#include "XMLParser.h"
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <sstream>

#include "common.h"
#include "float.h"

// Plain-Old-Data for gate hotspot information.
// Some of the data is for the simulator, some is for the gui, some is for both.
struct LibraryGateHotspot {
	
	// Used to associate hotspots in gui and simulator.
	std::string name;

	// Whether the hotspot is a simulator gate input.
	bool isInput;
	
	// Whether the simulator hotspot inverts signals passing through it.
	bool isInverted;

	// Not totally sure. Seems unnecessary. TODO
	std::string logicEInput;

	// Number of buslines for the hotspot.
	int busLines;

	// Hotspot x-coord relative to gui gate origin.
	float x;

	// Hotspot y-coord relative to gui gate origin.
	float y;
};

// Plain-Old-Data for gates in the gui.
// Holds data for one line in a gate's visual representation.
struct LibraryGateLine {

	LibraryGateLine(float x1, float y1, float x2, float y2);

	float x1;

	float y1;

	float x2;

	float y2;
};

// Plain-Old-Data for parameter-dialog options.
// Paramaters are used to set gate attributes in the gui and simulator.
// DialogParameters give users an easy way to edit these parameters.
struct LibraryGateDialogParamter {
	
	// The label shown to the user when the widget is drawn. The "Visible Name" of the param.
	std::string textLabel;

	// The "invisible name" of the param (either GUI or Logic).
	std::string name;

	// True if the param is a gui type. False if it is a logic param.
	bool isGui;

	// The "type" of this parameter. Type can be: STRING, INT, BOOL, FLOAT, FILE_IN, FILE_OUT.
	std::string type;

	// Number types can specify a min and max range for the variable.
	float Rmin;
	
	// Number types can specify a min and max range for the variable.
	float Rmax;
};

// Plain-Old-Data for a type of gate.
// Some of the data is for the simulator. Some is for the gui.
struct LibraryGate {

	// All gates must have a gateName, to identify them in the library.
	std::string gateName;

	// Gate's caption will be shown as a tooltip in the library
	std::string caption;

	// guiType is used in GuiCircuit::createGate(..).
	// If a gate has the default guiType, then guiType = "".
	std::string guiType;

	// If a gate has no logic type, then logicType = "".
	std::string logicType;

	// All gate hotspots.
	std::vector<LibraryGateHotspot> hotspots;

	// All gate line segments.
	std::vector<LibraryGateLine> shape;

	// The parameters to be listed in the "settings dialog".
	std::vector<LibraryGateDialogParamter> dlgParams;

	// gui attributes to be set for this type of gate.
	std::map<std::string, std::string> guiParams;

	// simulator attributes to set for this type of gate.
	std::map<std::string, std::string> logicParams;
};

class GateLibrary {
public:
	GateLibrary(std::string);

	GateLibrary();

	virtual ~GateLibrary();
	
	void parseFile();

	// Parse the shape object from the mParse file, adding an offset if needed:
	bool parseShapeObject(std::string type, LibraryGate* newGate, double offX = 0.0, double offY = 0.0 );
	
	// Returns a gate from the library in lgGate.  If the gate does not
	//	exist in the library, returns false, otherwise true.
	bool getGate(std::string gateName, LibraryGate &lgGate);
	
	// Return the logic type of a particular gate:
	std::string getGateLogicType(std::string gateName );
	
	// Return the gui type of a particular gate type:
	std::string getGateGUIType(std::string gateName );
	
	std::string getName() { return libName; };
	
	std::map < std::string, std::map < std::string, LibraryGate > >* getGateDefs() { return &gates; };

	// Take the string normally copied to the clipboard in a copy command,
	// and use it to define a BlackBox gate.
	void defineBlackBox(const std::string &copyText);

private:
	XMLParser* mParse;
	std::string fileName;
	std::string libName;
	int numDefinedBlackBoxes;
	
	// Maps library name to a map of gates, which maps to the librarygate struct
	std::map < std::string, std::map < std::string, LibraryGate > > gates;
};