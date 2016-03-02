/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   LibraryParse: Uses XMLParser to parse library files
*****************************************************************************/

#ifndef LIBRARYPARSE_H_
#define LIBRARYPARSE_H_

#include "XMLParser.h"
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <sstream>

#include "../logic/logic_defaults.h"
#include "float.h"

using namespace std;

struct lgHotspot {
	lgHotspot( string nName = "", bool nIsInput = true, float nX = 0, float nY = 0, bool nIsInverted = false, string nEInput = "" ) : name(nName), isInput(nIsInput), x(nX), y(nY), isInverted(nIsInverted), logicEInput(nEInput) {};
	string name;
	bool isInput;
	float x, y;
	bool isInverted;
	string logicEInput;
};

struct lgLine {
	lgLine( float nX1 = 0, float nY1 = 0, float nX2 = 0, float nY2 = 0 ) : x1(nX1), x2(nX2), y1(nY1), y2(nY2) {}
	float x1, x2, y1, y2;
};

struct lgDlgParam {
	string textLabel; // The label shown to the user when the widget is drawn. The "Visible Name" of the param.

	string name; // The "invisible name" of the param (either GUI or Logic).
	bool isGui; // True if the param is a gui type. False if it is a logic param.
	
	string type; // The "type" of this parameter. Type can be: STRING, INT, BOOL, FLOAT, FILE_IN, FILE_OUT.
	float Rmin, Rmax; // Number types can specify a min and max range for the variable.

	lgDlgParam( string ntextLabel = "", string nname = "", string ntype = "STRING", bool nisGui = true, float nRmin = -FLT_MAX, float nRmax = FLT_MAX ) : textLabel(ntextLabel), name(nname), isGui(nisGui), type(ntype), Rmin(nRmin), Rmax(nRmax) {};
};

struct LibraryGate {
	string gateName; // Note: All gates must have a gateName, to identify them in the library.
	string caption; // Note: Gate's caption will be shown as a tooltip in the library
	string guiType; // Note: If a gate has the default guiType, then guiType = "".
	string logicType; // Note: If a gate has no logic type, then logicType="".
	vector < lgHotspot > hotspots;
	vector < lgLine > shape;
	vector < lgDlgParam > dlgParams; // The parameters to be listed in the "settings dialog".
	map < string, string > guiParams;
	map < string, string > logicParams;
};

class LibraryParse {
public:
	LibraryParse(string);
	LibraryParse();
	~LibraryParse();
	
	void parseFile();

	// Parse the shape object from the mParse file, adding an offset if needed:
	bool parseShapeObject( string type, LibraryGate* newGate, double offX = 0.0, double offY = 0.0 );
	
	// Returns a gate from the library in lgGate.  If the gate does not
	//	exist in the library, returns false, otherwise true.
	bool getGate(string gateName, LibraryGate &lgGate);
	
	// Return the logic type of a particular gate:
	string getGateLogicType( string gateName );
	
	// Return the gui type of a particular gate type:
	string getGateGUIType( string gateName );
	
	string getName() { return libName; };
	
	map < string, map < string, LibraryGate > >* getGateDefs() { return &gates; };

private:
	XMLParser* mParse;
	string fileName;
	string libName;
	
	// Maps library name to a map of gates, which maps to the librarygate struct
	map < string, map < string, LibraryGate > > gates;
};

#endif /*LIBRARYPARSE_H_*/
