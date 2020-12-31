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

#include "../logic/logic_values.h"
#include "float.h"

using namespace std;

struct lgHotspot {
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// To permit pull-up, pull-down and forceJunction inputs
	lgHotspot(string nName = "", bool nIsInput = true, float nX = 0, float nY = 0, bool nIsInverted = false, bool nIsPullUp = false, bool nIsPullDown = false, bool nForceJunction = false, string nEInput = "", int busLines = 1) : name(nName), isInput(nIsInput), x(nX), y(nY), isInverted(nIsInverted), isPullUp(nIsPullUp), isPullDown(nIsPullDown), ForceJunction(nForceJunction), logicEInput(nEInput), busLines(busLines) {};
	string name;
	bool isInput;
	float x, y;
	bool isInverted;
	bool isPullUp;
	bool isPullDown;
	bool ForceJunction;	
	string logicEInput;
	int busLines;
};

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// parameter w added for linewidth (default=1)
struct lgLine {
	lgLine(float nX1 = 0, float nY1 = 0, float nX2 = 0, float nY2 = 0, int nW = 1) : x1(nX1), x2(nX2), y1(nY1), y2(nY2), w(nW) {}
	float x1, x2, y1, y2;
	int w;
};

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Lines with offset for rotate chars
struct lgOffLine {
	lgOffLine(lgLine nLine = lgLine(), float nX0 = 0, float nY0 = 0) : Line(nLine), x0(nX0), y0(nY0) {
	 x1 = x0 + nLine.x1; y1 = y0 + nLine.y1;
	 x2 = x0 + nLine.x2; y2 = y0 + nLine.y2;
	}
	lgLine Line;
	float x0, y0;
	float x1, y1;
	float x2, y2;
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
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Lines with offset for rotate chars
	vector < lgOffLine > Offshape;
	vector < lgDlgParam > dlgParams; // The parameters to be listed in the "settings dialog".
	map < string, string > guiParams;
	map < string, string > logicParams;
};

class LibraryParse {
public:
	LibraryParse(string);
	LibraryParse();
	virtual ~LibraryParse();
	
	void parseFile();

	// Added by Colin Broberg 11/16/16 -- need to make this a public function so that I can use it for dynamic gates
	void addGate(string libName, LibraryGate newGate);

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// New command to generate text shapes
	bool parseTextObject(LibraryGate* newGate);

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Lines with offset for rotate chars
	bool parseShapeText(XMLParser* Parse, string type, LibraryGate* newGate, int stringType, double cenX = 0.0, double cenY = 0.0, double dX = 0.0, double dY = 0.0, double Scale = 1.0);

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
	stringstream getXMLChar(char ch, bool Negate);
	XMLParser* mParse;
	string fileName;
	string libName;
	
	// Maps library name to a map of gates, which maps to the librarygate struct
	map < string, map < string, LibraryGate > > gates;
};

#endif /*LIBRARYPARSE_H_*/
