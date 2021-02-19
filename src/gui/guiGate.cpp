/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   
   
   guiGate: GUI representation of gate objects
*****************************************************************************/

#include "guiGate.h"
#include <iomanip>
#include "wx/wx.h"
#include "MainApp.h"
#include "klsCollisionChecker.h"
#include "paramDialog.h"
#include "guiWire.h"

DECLARE_APP(MainApp)

guiGate::guiGate() : klsCollisionObject(COLL_GATE) {
	myX = 1.0;
	myY = 1.0;
	selected = false;
	gparams["angle"] = "0";
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	gparams["mirror"] = "false";
}

guiGate::~guiGate(){
	// Destry the hotspots:
	
	map< string, gateHotspot* >::iterator hs = hotspots.begin();
	while( hs != hotspots.end() ) {
		// delete (hs->second);
		hotspots.erase(hs);
		hs = hotspots.begin(); 
	}
}

// Run through my connections and update the merges
void guiGate::updateConnectionMerges() {
	map < string, guiWire* >::iterator connWalk = connections.begin();
	while (connWalk != connections.end()) {
		(connWalk->second)->endSegDrag();
		connWalk++;
	}
}

string guiGate::getLogicType() {
	return wxGetApp().libParser.getGateLogicType( libGateName );
};

string guiGate::getGUIType() {
	return wxGetApp().libParser.getGateGUIType( libGateName );
};


// Update the position matrices, and
// update the world-space bounding box and hotspots.
// This is called once whenever the gate's position
// or angle changes.
void guiGate::updateBBoxes( bool noUpdateWires ) {
	// Get the translation vars:
	float x, y;
	this->getGLcoords( x, y );

	// Get the angle vars:
	istringstream iss(gparams["angle"]);
	GLfloat angle;
	iss >> angle;

	glMatrixMode(GL_MODELVIEW);

	// Set up the forward matrix:
	glLoadIdentity();
	glTranslatef(x, y, 0);
	glRotatef( angle, 0, 0, 1);

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// "mirror" GUI param 
	if (gparams["mirror"] == "true")
		glScalef(1, -1, 1);

	// Read the forward matrix into the member variable:
	glGetDoublev( GL_MODELVIEW_MATRIX, mModel );
	glLoadIdentity();

	// Update all of the hotspots' world coordinates:
	map< string, gateHotspot* >::iterator hs = hotspots.begin();
	while( hs != hotspots.end() ) {
		(hs->second)->worldLocation = modelToWorld( (hs->second)->modelLocation );
		(hs->second)->calcBBox();
		hs++;
	}

	// Convert bbox to world-space:
	klsBBox worldBBox;
	worldBBox.addPoint( modelToWorld( modelBBox.getTopLeft()     ) );
	worldBBox.addPoint( modelToWorld( modelBBox.getTopRight()    ) );
	worldBBox.addPoint( modelToWorld( modelBBox.getBottomLeft()  ) );
	worldBBox.addPoint( modelToWorld( modelBBox.getBottomRight() ) );
	this->setBBox(worldBBox);

	// Update the connected wires' shapes to accomidate the new gate position:
	map < string, guiWire* >::iterator connWalk = connections.begin();
	while (!noUpdateWires && connWalk != connections.end()) {
		(connWalk->second)->updateConnectionPos( this->getID(), connWalk->first );
		connWalk++;
	}
}

void guiGate::finalizeWirePlacements() {
	updateConnectionMerges();
}

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Added Rotate
// Convert model->world coordinates:
GLPoint2f guiGate::modelToWorld( GLPoint2f c, bool Rotate ) {

	// Perform a matrix-vector multiply to get the point coordinates in world-space:

	GLfloat x = mModel[12];
	GLfloat y = mModel[13];

	if (Rotate) {
		// Perform a matrix-vector multiply to get the point coordinates in world-space:
		x += c.x * mModel[0] + c.y * mModel[4];
		y += c.x * mModel[1] + c.y * mModel[5];
	} else {
		x += c.x;
		y += c.y;
	}

	return GLPoint2f( x, y );
}

void guiGate::addConnection(string c, guiWire* obj) {
	connections[c] = obj;
}

void guiGate::removeConnection(string c, int &obj) {
	if (connections.find(c) == connections.end()) return;
	obj = connections[c]->getID();
	connections.erase(c);
}

bool guiGate::isConnected(string c) {
	return (connections.find(c) != connections.end());
}

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Added drawPalette to do not draw wide outlines in palette
void guiGate::draw(bool color, bool drawPalette) {

	GLint oldStipple = 0; // The old line stipple pattern, if needed.
	GLint oldRepeat = 0;  // The old line stipple repeat pattern, if needed.
	GLboolean lineStipple = false; // The old line stipple enable flag, if needed.

	// Position the gate at its x and y coordinates:
	glLoadMatrixd(mModel);


	if (selected && color) {
		// Store the old line stipple pattern:
		lineStipple = glIsEnabled(GL_LINE_STIPPLE);
		glGetIntegerv(GL_LINE_STIPPLE_PATTERN, &oldStipple);
		glGetIntegerv(GL_LINE_STIPPLE_REPEAT, &oldRepeat);

		// Draw the gate with dotted lines:
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0x9999);
	}

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Now use lines instead of vertices
	// Draw fine, wide, out and bus lines	

	// Draw the gate:
	glLineWidth(1);
	glBegin(GL_LINES);
	for (unsigned int i = 0; i < lines.size(); i++)
		if (lines[i].w == 1) {
			glVertex2f(lines[i].x1, lines[i].y1);
			glVertex2f(lines[i].x2, lines[i].y2);
		}
	glEnd();

	// Draw the widelines:
	glLineWidth(2);
	glBegin(GL_LINES);
	for (unsigned int i = 0; i < lines.size(); i++)
		if (lines[i].w == 2) {
			glVertex2f(lines[i].x1, lines[i].y1);
			glVertex2f(lines[i].x2, lines[i].y2);
		}
	glEnd();

	// Draw the boldlines:
	glLineWidth(3);
	glBegin(GL_LINES);
	for (unsigned int i = 0; i < lines.size(); i++)
		if (lines[i].w == 3) {
			glVertex2f(lines[i].x1, lines[i].y1);
			glVertex2f(lines[i].x2, lines[i].y2);
		}
	glEnd();

	// Draw the outlines:
	if (!wxGetApp().appSettings.wideOutline || drawPalette)
		glLineWidth(1);
	else
		glLineWidth(2);
	glBegin(GL_LINES);
	for (unsigned int i = 0; i < lines.size(); i++)
		if (lines[i].w == 5) {
			glVertex2f(lines[i].x1, lines[i].y1);
			glVertex2f(lines[i].x2, lines[i].y2);
		}
	glEnd();

	glLineWidth(1);

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Uncommnet to show cross
	// Put 0,0 cross
	/*{
		GLfloat Color[4];
		glGetFloatv(GL_CURRENT_COLOR, Color);
		glColor4f(0.0, 1.0, 1.0, 1.0);
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex2f(wxGetApp().appSettings.wireConnRadius, wxGetApp().appSettings.wireConnRadius);
		glVertex2f(-wxGetApp().appSettings.wireConnRadius, -wxGetApp().appSettings.wireConnRadius);
		glVertex2f(-wxGetApp().appSettings.wireConnRadius, wxGetApp().appSettings.wireConnRadius);
		glVertex2f(wxGetApp().appSettings.wireConnRadius, -wxGetApp().appSettings.wireConnRadius);
		glEnd();
		glColor4f(Color[0], Color[1], Color[2], Color[3]);
	}
	// */

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Uncommnet to show offset cross
	// Put offset cross	
	/*{
		GLfloat Color[4];
		glGetFloatv(GL_CURRENT_COLOR, Color);
		glColor4f(1.0, 0.0, 1.0, 1.0);
		glLineWidth(1);
		glBegin(GL_LINES);
		for (unsigned int i = 0; i < textLines.size(); i++) {
			float cenX = textLines[i].x0;
			float cenY = textLines[i].y0;

			glVertex2f(cenX, cenY + wxGetApp().appSettings.wireConnRadius);
			glVertex2f(cenX, cenY - wxGetApp().appSettings.wireConnRadius);
			glVertex2f(cenX + wxGetApp().appSettings.wireConnRadius, cenY);
			glVertex2f(cenX - wxGetApp().appSettings.wireConnRadius, cenY);

		}
		glEnd();
		glColor4f(Color[0], Color[1], Color[2], Color[3]);
	}
	// */

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Draw text
	istringstream iss(gparams["angle"]);
	GLfloat angle;
	iss >> angle;
	bool mirror = false;
	if (gparams.find("mirror") != gparams.end())
		mirror = gparams["mirror"] == "true" ? true : false;

	glBegin(GL_LINES);
	for (unsigned int i = 0; i < textLines.size(); i++)
	{
		float x0 = textLines[i].x0;
		float y0 = textLines[i].y0;
		float x1 = textLines[i].Line.x1;
		float y1 = textLines[i].Line.y1;
		float x2 = textLines[i].Line.x2;
		float y2 = textLines[i].Line.y2;

		textLines[i].x1 = x0 + (x1 * cos(angle*DEG2RAD) - y1 * sin(angle*DEG2RAD)) * ((angle == 90 || angle == 270) ? -1 : 1);
		textLines[i].y1 = y0 + (x1 * sin(angle*DEG2RAD) + y1 * cos(angle*DEG2RAD)) * ((angle == 90 || angle == 270) ? -1 : 1) * (mirror ? -1 : 1);
		textLines[i].x2 = x0 + (x2 * cos(angle*DEG2RAD) - y2 * sin(angle*DEG2RAD)) * ((angle == 90 || angle == 270) ? -1 : 1);
		textLines[i].y2 = y0 + (x2 * sin(angle*DEG2RAD) + y2 * cos(angle*DEG2RAD)) * ((angle == 90 || angle == 270) ? -1 : 1) * (mirror ? -1 : 1);

		glVertex2f(textLines[i].x1, textLines[i].y1);
		glVertex2f(textLines[i].x2, textLines[i].y2);
	}

	glEnd();

	// Reset the stipple parameters:
	if( selected && color ) {	
		// Reset the line pattern:
		if( !lineStipple ) {
			glDisable( GL_LINE_STIPPLE );
		}
		glLineStipple( oldRepeat, oldStipple );
	}
}

void guiGate::setGLcoords( float x, float y, bool noUpdateWires ) {
	this->myX = x;
	this->myY = y;

	// Update the matrices and bounding box:
	updateBBoxes(noUpdateWires);
}


void guiGate::getGLcoords( float &x, float &y ) {
	x = this->myX;
	y = this->myY;
}


// Shift the gate by x and y, relative to its current location:
void guiGate::translateGLcoords( float x, float y ) {
	setGLcoords( this->myX + x, this->myY + y );
}


// Draw this gate as unselected:
void guiGate::unselect( void ) {
	selected = false;
}
	
// Draw this gate as selected from now until unselect() is
// called, if the coordinate passed to it is within
// this gate's bounding box in GL coordinates.
// Return true if this gate is selected.
bool guiGate::clickSelect( GLfloat x, GLfloat y ) {
	if( this->getBBox().contains( GLPoint2f( x, y ) ) ) {
		selected = true;
		return true;
	} else {
		return false;
	}
}

// Insert a line in the line list.
// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Parameter w added for linewidth (default=1)
// Now use lines instead of vertices
void guiGate::insertLine( float x1, float y1, float x2, float y2, int w) {
	lines.push_back(lgLine(x1, y1, x2, y2, w));
}

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Lines with offset for rotate chars
void guiGate::insertTextLine(float x0, float y0, float x1, float y1, float x2, float y2, int w) {
	textLines.push_back(lgOffLine(lgLine(x1, y1, x2, y2, w),x0,y0));
}


// Recalculate the bounding box, based on the lines that are included already:
// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Now use lines instead of vertices
void guiGate::calcBBox( void ) {
	modelBBox.reset();
	
	for (unsigned int i = 0; i < lines.size(); i++) {
		modelBBox.addPoint(GLPoint2f(lines[i].x1, lines[i].y1));
		modelBBox.addPoint(GLPoint2f(lines[i].x2, lines[i].y2));
	}

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Add text lines
	// Must be review
	for (unsigned int i = 0; i < textLines.size(); i++) {
		modelBBox.addPoint(GLPoint2f(textLines[i].x1, textLines[i].y1));
		modelBBox.addPoint(GLPoint2f(textLines[i].x2, textLines[i].y2));
	}

	// Recalculate the world-space bbox:
	updateBBoxes();
}


// Insert a hotspot in the hotspot list.
void guiGate::insertHotspot( float x1, float y1, string connection, int busLines) {
	if (hotspots.find(connection) != hotspots.end()) return; // error: hotspot already exists
	
	gateHotspot* newHS = new gateHotspot( connection );
	newHS->modelLocation = GLPoint2f( x1, y1 );
	newHS->setBusLines(busLines);

	// Add the hs to the gate's struct:
	hotspots[connection] = newHS;
	
	// Add the hs to the gate's sub-object list:
	this->insertSubObject( newHS );
	
	// Update the hotspot's world-space bbox:
	updateBBoxes();
}


// Check if any of the hotspots of this gate are within the delta
// of the world coordinates sX and sY. delta is in gl coords.
string guiGate::checkHotspots( GLfloat x, GLfloat y, GLfloat delta ) {
	// Set up the mouse as a collision object:
	klsCollisionObject mouse( COLL_MOUSEBOX );
	klsBBox mBox;
	mBox.addPoint( GLPoint2f( x, y ) );
	mBox.extendTop( delta );
	mBox.extendBottom( delta );
	mBox.extendLeft( delta );
	mBox.extendRight( delta );
	mouse.setBBox( mBox );

	// Check if any hotspots hit the mouse:
	CollisionGroup results = this->checkSubsToObj( &mouse );
	CollisionGroup::iterator rs = results.begin();
	while( rs != results.end() ) {
		// If there were any hotspots that hit, then return
		// the first one to the caller:
		if( (*rs)->getType() == COLL_GATE_HOTSPOT ) {
			return ((gateHotspot*) *rs)->name;
		}

		rs++;
	}

	return "";
}


void guiGate::getHotspotCoords(string hsName, float &x, float &y) {

	if( hotspots.find(hsName) == hotspots.end() ) {
		//TODO: Couldn't find hotspot, so give a useful warning.
		return;
	}

	GLPoint2f hs = hotspots[hsName]->getLocation();
	x = hs.x;
	y = hs.y;
	return;
}


std::string guiGate::getHotspotPal(const std::string &hotspot) {

	GLPoint2f coords;
	getHotspotCoords(hotspot, coords.x, coords.y);

	//looping looking for another hotspot with the same location
	map<string, GLPoint2f> hotspotList = getHotspotList();

	for (const auto &possiblePal : hotspotList) {
		if (possiblePal.first != hotspot && possiblePal.second == coords) {
			return possiblePal.first;
		}
	}
	return "";
}

bool guiGate::isVerticalHotspot( string hsName ) {
	// Pedro Casanova (casanova@ujaen.es) 2021/01-02
	// This is a problem when draging WIRE gates and in @@_GATES and @@_CIRCUIT !!!!
	float x, y;
	float w, h;
	getHotspotCoords( hsName, x, y );
	w = getBBox().getRight() - getBBox().getLeft();
	h = getBBox().getTop() - getBBox().getBottom();
	if (h < 0.5) return false;
	return ( min( getBBox().getTop()-y, y-getBBox().getBottom() ) < min( getBBox().getRight()-x, x-getBBox().getLeft() ) );
}

void guiGate::saveGate(XMLParser* xparse) {
	float x, y;
	this->getGLcoords( x, y );

	xparse->openTag("gate");
	xparse->openTag("ID");
	ostringstream oss;
	oss << gateID;
	xparse->writeTag("ID", oss.str());
	xparse->closeTag("ID");
	xparse->openTag("type");
	xparse->writeTag("type", libGateName);
	xparse->closeTag("type");	
	oss.str("");
	xparse->openTag("position");
	oss << x << "," << y;
	xparse->writeTag("position", oss.str());
	xparse->closeTag("position");
	map< string, guiWire* >::iterator pC = connections.begin();
	while (pC != connections.end()) {
		xparse->openTag((isInput[pC->first] ? "input" : "output"));
		xparse->openTag("ID");
		xparse->writeTag("ID", pC->first);
		xparse->closeTag("ID");
		oss.str("");

		for (IDType thisId : pC->second->getIDs()) {
			oss << thisId << " ";
		}
		
		xparse->writeTag((isInput[pC->first] ? "input" : "output"), oss.str());
		xparse->closeTag((isInput[pC->first] ? "input" : "output"));

		pC++;
	}
	map< string, string >::iterator pParams = gparams.begin();
	while (pParams != gparams.end()) {
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// Avoid to save some gparams, they are obtained from the library
		if (pParams->first == "angle" && (pParams->second == "0" || pParams->second == "0.0")) { pParams++; continue; }
		if (pParams->first == "mirror" && pParams->second != "true") { pParams++; continue; }
		if (pParams->first == "HIDE_DISPLAY" && pParams->second != "true") { pParams++; continue; }
		if (pParams->first == "BCD" && pParams->second != "true") { pParams++; continue; }		
		if (pParams->first == "ORIGINAL_NAME" && pParams->second == "") { pParams++; continue; }		
		if (pParams->first == "CROSS_POINT") { pParams++; continue; }
		if (pParams->first == "LED_BOX") { pParams++; continue; }
		if (pParams->first == "VALUE_BOX") { pParams++; continue; }		
		if (pParams->first == "CLICK_BOX") { pParams++; continue; }
		if (pParams->first == "LENGTH") { pParams++; continue; }
		if (pParams->first.substr(0, 11) == "KEYPAD_BOX_") { pParams++; continue; }
	
		xparse->openTag("gparam");
		oss.str("");
		oss << pParams->first << " " << pParams->second;
		xparse->writeTag("gparam", oss.str());
		xparse->closeTag("gparam");		
		pParams++;
	}
	pParams = lparams.begin();
	LibraryGate lg = wxGetApp().libraries[getLibraryName()][getLibraryGateName()];
	while (pParams != lparams.end()) {
		bool found = false;
		for (unsigned int i = 0; i < lg.dlgParams.size() && !found; i++) {
			if (lg.dlgParams[i].isGui) continue;
			if ((lg.dlgParams[i].type == "FILE_IN" || lg.dlgParams[i].type == "FILE_OUT") &&
				lg.dlgParams[i].name == pParams->first) found = true;
		}
		if (found) { pParams++; continue; }
		// Pedro Casanova (casanova@ujaen.es) 2021-01
		// Avoid to save some lparams, they are obtained from the library
		if (pParams->first == "DEFAULT_DELAY" && pParams->second == "1") { pParams++; continue; }
		// Pedro Casanova (casanova@ujaen.es) 2021/01-02
		// State: param saved in saveGateTypeSpecifics to short them		
		if (pParams->first.substr(0, 6) == "State:") { pParams++; continue; }
		if (pParams->first.substr(0, 9) == "Function:") { pParams++; continue; }
		if (pParams->first == "CURRENT_STATE" && pParams->second == "") { pParams++; continue; }
		if (pParams->first == "CLEAR_FSM") { pParams++; continue; }

		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// Avoid to save some lparams, they are obtained from the library
		// if (pParams->first == "xxx") { pParams++; continue; }

		xparse->openTag("lparam");
		oss.str("");
		oss << pParams->first << " " << pParams->second;
		xparse->writeTag("lparam", oss.str());
		xparse->closeTag("lparam");		
		pParams++;
	}
	
	//*********************************
	//Edit by Joshua Lansford 6/06/2007
	//I want the ram files to save their contents to file
	//with the circuit.  However, I don't want to send
	//an entire page of data up to the gui each time
	//an entry changes.
	//This way the ram gate can intelegently save what
	//it wants to into the file.
	//Also any other gate that wishes too, can also
	//save specific stuff.
	this->saveGateTypeSpecifics( xparse );
	//End of edit***********************
	
	xparse->closeTag("gate");	
}

void guiGate::doParamsDialog(void* gCircuit, wxCommandProcessor* wxcmd) {
	if (wxGetApp().libraries[libName][libGateName].dlgParams.size() == 0) return;
	paramDialog myDialog("Parameters", gCircuit, this, wxcmd);
	myDialog.SetFocus();
	myDialog.ShowModal();
}

// Pedro Casanova (casanova@ujaen.es) 2021/01-02
// Show gate propierties
void guiGate::doPropsDialog() {
	ostringstream oss;	
	if (this->getLibraryGateName() == "@@_NOT_FOUND" && gparams.find("ORIGINAL_NAME") != gparams.end())
		oss << "Component not found\nOriginal name:\t" << this->getGUIParam("ORIGINAL_NAME") << "\n";
	else
	{
		string Prop;
		oss << "Library:\t\t" << getLibraryName() << "\n";
		oss << "Name:\t\t" << getLibraryGateName() << "\n";
		oss << "Caption:\t\t" << wxGetApp().libraries[getLibraryName()][getLibraryGateName()].caption << "\n";
		Prop = this->getLogicType();
		if (Prop == "") Prop = "-";
		oss << "Logic type:\t" << Prop << "\n";
		Prop = this->getGUIType();
		if (Prop == "") Prop = "-";
		oss << "GUI type:\t\t" << Prop << "\n";

		oss << "HOTSPOTS:\n";
		vector <lgHotspot> hotspot = wxGetApp().libraries[getLibraryName()][getLibraryGateName()].hotspots;
		for (unsigned int i = 0; i < hotspot.size(); i++) {
			oss << "\t" << (hotspot[i].isInput ? "INPUT" : "OUTPUT") << "\t" << hotspot[i].name;
			oss << "\t" << (hotspot[i].isInverted ? "INVERTED" : "") << "\n";
		}
		oss << "PARAMS:\n";
		map <string, string> lParams = wxGetApp().libraries[getLibraryName()][getLibraryGateName()].logicParams;
		map < string, string >::iterator lpwalk = lParams.begin();
		while (lpwalk != lParams.end()) {
			oss << "\tLOGIC\t" << lpwalk->first << " : " << lpwalk->second << "\n";
			lpwalk++;
		}

		map <string, string> gParams = wxGetApp().libraries[getLibraryName()][getLibraryGateName()].guiParams;
		map < string, string >::iterator gpwalk = gParams.begin();
		while (gpwalk != gParams.end()) {
			oss << "\tGUI\t" << gpwalk->first << " : " << gpwalk->second << "\n";
			gpwalk++;
		}

	}
	wxMessageBox(oss.str(), "Properties");
}

void guiGate::setGUIParam(string paramName, string value) {
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// "mirror" GUI param 
	if (paramName == "mirror") {
			if (value != "true" && value != "false")
				return;
	}

	gparams[paramName] = value;

	if (paramName == "angle" || paramName == "mirror") {
		// Update the matrices and bounding box:
		updateConnectionMerges();
		draw();
		calcBBox();
	}
}

// *********************** guiGateBUSEND *************************

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
void guiGateBUSEND::draw(bool color, bool drawPalette) {

	guiGate::draw(color, drawPalette);

	if (!drawPalette) {
		if (color) {
			bool conflict = false;
			bool unknown = false;
			bool hiz = false;
			float redness = 0;
			map< string, guiWire* >::iterator theCnk = connections.begin();
			if (theCnk != connections.end()) {
				for (int i = 0; i < (int)theCnk->second->getState().size(); i++) {
					switch ((theCnk->second)->getState()[i]) {
					case ZERO:
						break;
					case ONE:
						redness += pow(2, i);					// For buses red scale
						break;
					case HI_Z:
						hiz = true;
						break;
					case UNKNOWN:
						unknown = true;
						break;
					case CONFLICT:
						conflict = true;
						break;
					}
				}
				redness /= pow(2, theCnk->second->getState().size()) - 1;			// For buses red scale
			}
			else
				hiz = true;
			
			if (conflict) {		// cyan
				glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
			}
			else if (unknown) {	// blue
				glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
			}
			else if (hiz) {		// green
				glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
			}
			else {				// red or black
				glColor4f(redness, 0.0f, 0.0f, 1.0f);		// For buses red scale
			}
		}
	}

	// Draw the bus lines:
	glLineWidth(4);
	glBegin(GL_LINES);
	for (unsigned int i = 0; i < lines.size(); i++)
		if (lines[i].w == 10) {
			glVertex2f(lines[i].x1, lines[i].y1);
			glVertex2f(lines[i].x2, lines[i].y2);
		}
	glEnd();
	glLineWidth(1);

	// Set the color back to black:
	glColor4f(0, 0, 0, 1.0);
}


// *********************** guiGatePLD *************************
// Pedro Casanova (casanova@ujaen.es) 2020/04-12
guiGatePLD::guiGatePLD() {
	guiGate();

	setGUIParam("CROSS_JUNCTION", "true");
	setGUIParam("CROSS_POINT", "0,0");
}

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Added drawPalette to do not draw wide outlines in palette
void guiGatePLD::draw(bool color, bool drawPalette) {

	guiGate::draw(color, drawPalette);

	// Set the color back to the old color:
	glColor4f(0.0, 0.0, 0.0, 1.0);
	map <string, string>* logicParams = getAllLogicParams();
	if (logicParams->find("FORCE_ZERO") != logicParams->end())
		if (logicParams->find("FORCE_ZERO")->second == "true")
		{
			glLineWidth(2);
			glBegin(GL_LINES);
			glVertex2f(renderInfo_crossPoint.x + wxGetApp().appSettings.wireConnRadius, renderInfo_crossPoint.y + wxGetApp().appSettings.wireConnRadius);
			glVertex2f(renderInfo_crossPoint.x - wxGetApp().appSettings.wireConnRadius, renderInfo_crossPoint.y - wxGetApp().appSettings.wireConnRadius);
			glVertex2f(renderInfo_crossPoint.x - wxGetApp().appSettings.wireConnRadius, renderInfo_crossPoint.y + wxGetApp().appSettings.wireConnRadius);
			glVertex2f(renderInfo_crossPoint.x + wxGetApp().appSettings.wireConnRadius, renderInfo_crossPoint.y - wxGetApp().appSettings.wireConnRadius);
			glBegin(GL_LINES);
			glEnd();
			glLineWidth(1);
		}
}

void guiGatePLD::setGUIParam(string paramName, string value) {
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// <CROSS_JUNCTION> cross junctions in inputs for PLD devices (LAND & LOR gates)
	if (paramName == "CROSS_JUNCTION")
		if (this->getLogicType() == "PLD_AND" || this->getLogicType() == "OR")
		{
			if (value != "true" && value != "false")
				return;
		}
		else
			return;

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// <CROSS_POINT> point to draw cross junctions in gate for PLD devices (LAND gates)
	if (paramName == "CROSS_POINT") {
		if (this->getLogicType() == "PLD_AND")
		{
			istringstream iss(value);
			char dump;
			iss >> renderInfo_crossPoint.x >> dump >> renderInfo_crossPoint.y;
		}
		else
			return;
	}

	if (paramName == "CROSS_JUNCTION" || paramName == "CROSS_POINT") {
		gparams[paramName] = value;
		// Update the matrices and bounding box:
		updateConnectionMerges();
		updateBBoxes();
		return;
	}

	guiGate::setGUIParam(paramName, value);
}

// *********************** guiGateTOGGLE *************************

guiGateTOGGLE::guiGateTOGGLE() {
	guiGate();
	// Default to "off" state when creating a toggle gate:
	//NOTE: Does not send this to the core, just updates it
	// on the GUI side.
	setLogicParam( "OUTPUT_NUM", "0" );
	
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Set the default CLICK box:
	renderInfo_clickBox.begin.x = 0;
	renderInfo_clickBox.begin.y = 0;
	renderInfo_clickBox.end.x = 0;
	renderInfo_clickBox.end.y = 0;
}

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Added drawPalette to do not draw wide outlines in palette
void guiGateTOGGLE::draw(bool color, bool drawPalette) {
	// Draw the default lines:
	guiGate::draw(color, drawPalette);
	
	// Add the rectangle: Inner Square
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12		Added Black for color=false
	if (color)
		glColor4f((float)renderInfo_outputNum, 0.0, 0.0, 1.0);
	else
		glColor4f(0, 0, 0, 1.0);		

	glRectd(renderInfo_clickBox.begin.x, renderInfo_clickBox.begin.y, renderInfo_clickBox.end.x, renderInfo_clickBox.end.y);

	// Set the color back to the old color:
	glColor4f( 0.0, 0.0, 0.0, 1.0 );
}

void guiGateTOGGLE::setGUIParam( string paramName, string value ) {
	if (paramName == "CLICK_BOX") {
		istringstream iss(value);
		char dump;
		iss >> renderInfo_clickBox.begin.x >> dump >> renderInfo_clickBox.begin.y >>
			dump >> renderInfo_clickBox.end.x >> dump >> renderInfo_clickBox.end.y;
	}
	guiGate::setGUIParam(paramName, value);
}

void guiGateTOGGLE::setLogicParam( string paramName, string value ) {
	if (paramName == "OUTPUT_NUM") {
		istringstream iss(value);
		iss >> renderInfo_outputNum;
	}
	guiGate::setLogicParam(paramName, value);
}

// Toggle the output button on and off:
klsMessage::Message_SET_GATE_PARAM* guiGateTOGGLE::checkClick( GLfloat x, GLfloat y ) {
	klsBBox toggleButton;

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Return if CLICK_BOX does not exist
	if (gparams.find("CLICK_BOX") == gparams.end())
		return NULL;
	// Get the size of the CLICK square from the parameters:
	string clickBox = getGUIParam( "CLICK_BOX" );
	if (clickBox == "")
		return NULL;
	istringstream iss(clickBox);
	GLdouble minx = 0;
	GLdouble miny = 0;
	GLdouble maxx = 0;
	GLdouble maxy = 0;
	char dump;
	iss >> minx >> dump >> miny >> dump >> maxx >> dump >> maxy;

	toggleButton.addPoint( modelToWorld( GLPoint2f( minx, miny ) ) );
	toggleButton.addPoint( modelToWorld( GLPoint2f( minx, maxy ) ) );
	toggleButton.addPoint( modelToWorld( GLPoint2f( maxx, miny ) ) );
	toggleButton.addPoint( modelToWorld( GLPoint2f( maxx, maxy ) ) );

	if (toggleButton.contains( GLPoint2f( x, y ) )) {
		setLogicParam("OUTPUT_NUM", (getLogicParam("OUTPUT_NUM") == "0") ? "1" : "0" );
/*		ostringstream oss;
		oss << "SET GATE ID " << getID() << " PARAMETER OUTPUT_NUM " << getLogicParam("OUTPUT_NUM"); */
		return new klsMessage::Message_SET_GATE_PARAM(getID(), "OUTPUT_NUM", getLogicParam("OUTPUT_NUM"));
	} else return NULL;
}

// ******************** END guiGateTOGGLE **********************

// *********************** guiGateKEYPAD *************************

guiGateKEYPAD::guiGateKEYPAD() {
	guiGate();
	// Default to 0 when creating:
	//NOTE: Does not send this to the core, just updates it
	// on the GUI side.
	setLogicParam( "OUTPUT_NUM", "0" );
	keypadValue = "0";
	
	// All click boxes are in gui params as a list of type KEYPAD_BOX_<val>
	//	param values are of type "minx,miny,maxx,maxy"
}

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Added drawPalette to do not draw wide outlines in palette
void guiGateKEYPAD::draw(bool color, bool drawPalette) {
	// Position the gate at its x and y coordinates:
	glLoadMatrixd(mModel);
	
	if (color) {
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// To draw inner square when gate is rotated
		GLfloat Color[4];
		glGetFloatv(GL_CURRENT_COLOR, Color);
		glColor4f(0.0, 0.4f, 1.0, 0.3f);

		float x1 = renderInfo_valueBox.begin.x;
		float y1 = renderInfo_valueBox.begin.y;
		float x2 = renderInfo_valueBox.end.x;
		float y2 = renderInfo_valueBox.end.y;

		// To rotate digits
		istringstream iss(gparams["angle"]);
		GLfloat angle;
		iss >> angle;
		bool mirror = false;
		if (gparams.find("mirror") != gparams.end())
			mirror = gparams["mirror"] == "true" ? true : false;

		float x1p = (x1 * cos(angle*DEG2RAD) - y1 * sin(angle*DEG2RAD)) * ((angle == 90 || angle == 270) ? -1 : 1);
		float y1p = (x1 * sin(angle*DEG2RAD) + y1 * cos(angle*DEG2RAD)) * ((angle == 90 || angle == 270) ? -1 : 1) * (mirror ? -1 : 1);
		float x2p = (x2 * cos(angle*DEG2RAD) - y2 * sin(angle*DEG2RAD)) * ((angle == 90 || angle == 270) ? -1 : 1);
		float y2p = (x2 * sin(angle*DEG2RAD) + y2 * cos(angle*DEG2RAD)) * ((angle == 90 || angle == 270) ? -1 : 1) * (mirror ? -1 : 1);

		// Add the rectangle - this is a highlight so needs done before main gate draw:
		glRectd(x1p, y1p, x2p, y2p);

		// Set the color back to the old color:
		glColor4f(Color[0], Color[1], Color[2], Color[3]);
	}

	// Draw the default lines:
	guiGate::draw(color, drawPalette);
}

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Parameter ROTATE_KEYPAD_BOX for old style KEYPAD can only be true
// Indicate if KEYPAD_BOX_ areas must rotate
void guiGateKEYPAD::setGUIParam(string paramName, string value) {
	if (paramName == "ROTATE_KEYPAD_BOX") {
		if (value != "true")
			return;
	}
	guiGate::setGUIParam(paramName, value);
}

void guiGateKEYPAD::setLogicParam( string paramName, string value ) {
	if (paramName == "OUTPUT_NUM" || paramName == "OUTPUT_BITS") {
		istringstream iss(paramName == "OUTPUT_NUM" ? value : lparams["OUTPUT_NUM"]);
		int intVal;
		iss >> intVal;
        ostringstream ossVal, ossParamName;
		// Convert to hex
        for (int i=2*sizeof(int) - 1; i>=0; i--) {
            ossVal << "0123456789ABCDEF"[((intVal >> i*4) & 0xF)];
        }
        iss.clear(); iss.str(paramName == "OUTPUT_BITS" ? value : lparams["OUTPUT_BITS"]);
        iss >> intVal;
        string currentValue = ossVal.str().substr(ossVal.str().size()-(intVal/4),(intVal/4));		
		ossParamName << "KEYPAD_BOX_" << currentValue;
		if (gparams.find(ossParamName.str()) == gparams.end()) {
			ossParamName.str("");
			map < string, string >::iterator gparamWalk = gparams.begin();
			while (gparamWalk != gparams.end()) {
				if ((gparamWalk->first).substr(0,11) != "KEYPAD_BOX_") { gparamWalk++; continue; }
				ossParamName << gparamWalk->first;
				break;
			}			
		}
		if (ossParamName.str() != "") {
			string clickBox = getGUIParam( ossParamName.str() );
			istringstream iss(clickBox);
			char dump;
			iss >> renderInfo_valueBox.begin.x >> dump >> renderInfo_valueBox.begin.y >> 
				dump >> renderInfo_valueBox.end.x >> dump >> renderInfo_valueBox.end.y;
		}		
	}

	guiGate::setLogicParam(paramName, value);
}

// Check the click boxes for the keypad and set appropriately:
klsMessage::Message_SET_GATE_PARAM* guiGateKEYPAD::checkClick( GLfloat x, GLfloat y ) {
	map < string, string >::iterator gparamWalk = gparams.begin();
	while (gparamWalk != gparams.end()) {
		// Is this a keypad box param?
		if ((gparamWalk->first).substr(0,11) != "KEYPAD_BOX_") { gparamWalk++; continue; }
		
		klsBBox keyButton;
	
		// Get the size of the CLICK square from the parameters:
		string clickBox = getGUIParam( gparamWalk->first );
		istringstream iss(clickBox);
		GLdouble minx = -0.5;
		GLdouble miny = -0.5;
		GLdouble maxx = 0.5;
		GLdouble maxy = 0.5;
		char dump;
		iss >> minx >> dump >> miny >> dump >> maxx >> dump >> maxy;

		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// Do not rotate KEYPAD_BOX
		keyButton.addPoint(modelToWorld(GLPoint2f(minx, miny), false));
		keyButton.addPoint(modelToWorld(GLPoint2f(minx, maxy), false));
		keyButton.addPoint(modelToWorld(GLPoint2f(maxx, miny), false));
		keyButton.addPoint(modelToWorld(GLPoint2f(maxx, maxy), false));
	
		if (keyButton.contains( GLPoint2f( x, y ) )) {
			// Retrieve the value of the box
			iss.clear();
			keypadValue = (gparamWalk->first).substr(11,(gparamWalk->first).size()-11);
			iss.str( keypadValue );
			int keypadIntVal;
			// Convert to decimal (cheap hack)
			iss >> setbase(16) >> keypadIntVal;
			ostringstream ossValue;
			ossValue << keypadIntVal;
			setLogicParam("OUTPUT_NUM", ossValue.str() );
/*			ostringstream oss;
			oss << "SET GATE ID " << getID() << " PARAMETER OUTPUT_NUM " << getLogicParam("OUTPUT_NUM"); */
			return new klsMessage::Message_SET_GATE_PARAM(getID(), "OUTPUT_NUM", getLogicParam("OUTPUT_NUM"));
		}
		gparamWalk++;
	}
	
	return NULL;
}

// ******************** END guiGateKEYPAD **********************

// *********************** guiGateREGISTER *************************

guiGateREGISTER::guiGateREGISTER() {
	guiGate();
	// Default to 0 when creating:
	// NOTE: Does not send this to the core, just updates it
	// on the GUI side.
	renderInfo_numDigitsToShow = 1;

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Set default VALUE_BOX
	renderInfo_valueBox.begin.x = 0;
	renderInfo_valueBox.begin.y = 0;
	renderInfo_valueBox.end.x = 0;
	renderInfo_valueBox.end.y =0 ;
	renderInfo_diffx = renderInfo_valueBox.end.x - renderInfo_valueBox.begin.x;
	renderInfo_diffy = renderInfo_valueBox.end.y - renderInfo_valueBox.begin.y;

	setLogicParam( "CURRENT_VALUE", "0" );
	setLogicParam( "UNKNOWN_OUTPUTS", "false" );
	display_BCD = false;		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	hide_display = false;	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
}

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Added drawPalette to do not draw wide outlines in palette
void guiGateREGISTER::draw(bool color, bool drawPalette) {
	// Draw the default lines:
	guiGate::draw(color, drawPalette);
	if (hide_display) return;
	float diffx = renderInfo_diffx;
	float diffy = renderInfo_diffy;
	
	diffx /= (double)renderInfo_numDigitsToShow; // set width of each digit
	
	//Inner Square for value
	// Display box
	glBegin(GL_LINE_LOOP);
	glVertex2f(renderInfo_valueBox.begin.x, renderInfo_valueBox.begin.y);
	glVertex2f(renderInfo_valueBox.begin.x, renderInfo_valueBox.end.y);
	glVertex2f(renderInfo_valueBox.end.x, renderInfo_valueBox.end.y);
	glVertex2f(renderInfo_valueBox.end.x, renderInfo_valueBox.begin.y);
	glEnd();

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12		Added Black for color=false
	if (color)
	{
		// Draw the number in red (or blue if inputs are not all sane)
		if (renderInfo_drawBlue) glColor4f(0.3f, 0.3f, 1.0, 1.0);
		else glColor4f(1.0, 0.0, 0.0, 1.0);
	}
	else
		glColor4f(0, 0, 0, 1.0);	// Black

	GLfloat lineWidthOld;
	glGetFloatv(GL_LINE_WIDTH, &lineWidthOld);
	glLineWidth(2.0);

	istringstream iss(gparams["angle"]);
	GLfloat angle;
	iss >> angle;
	bool mirror = false;
	if (gparams.find("mirror") != gparams.end())
		mirror = gparams["mirror"] == "true" ? true : false;

	// THESE ARE ALL SEVEN SEGMENTS WITH DIFFERENTIAL COORDS.  USE THEM FOR EACH DIGIT VALUE FOR EACH DIGIT
	//		AND INCREMENT CURRENTDIGIT.  CURRENTDIGIT=0 IS MSB.
	glBegin(GL_LINES);
	for (unsigned int currentDigit = 0; currentDigit < renderInfo_currentValue.size(); currentDigit++) {
		char c = renderInfo_currentValue[currentDigit];
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12		An eight if color=false
		if (!color)
			c = 8;
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// Rotate display to show always correct

		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// Don´t show from A to F if display_BCD is false
		if (c <= '9' || !display_BCD)
		{
			double x0, y0, x1, x2, y1, y2;
			if (angle == 180 || angle == 270)
			{
				x0 = renderInfo_valueBox.begin.x + diffx * (renderInfo_currentValue.size() - currentDigit);
				y0 = renderInfo_valueBox.begin.y + diffy;
			}
			else
			{
				x0 = renderInfo_valueBox.begin.x + diffx * currentDigit;
				y0 = renderInfo_valueBox.begin.y;
			}
			if (c != '1' && c != '4' && c != 'B' && c != 'D') {											// TOP
				x1 = (diffx*0.1875);
				y1 = (diffy*0.88462);
				x2 = (diffx*0.8125);
				y2 = (diffy*0.88462);
				if (mirror) {
					y1 = diffy - y1;	y2 = diffy - y2;
				}
				if (angle == 180 || angle == 270) {
					x1 *= -1;	y1 *= -1;	x2 *= -1;	y2 *= -1;
				}
				glVertex2f(x0 + x1, y0 + y1);
				glVertex2f(x0 + x2, y0 + y2);
			}
			if (c != '0' && c != '1' && c != '7' && c != 'C') {											// MIDLE
				x1 = (diffx*0.1875);
				y1 = (diffy*0.5);
				x2 = (diffx*0.8125);
				y2 = (diffy*0.5);
				if (mirror) {
					y1 = diffy - y1;	y2 = diffy - y2;
				}
				if (angle == 180 || angle == 270) {
					x1 *= -1;	y1 *= -1;	x2 *= -1;	y2 *= -1;
				}
				glVertex2f(x0 + x1, y0 + y1);
				glVertex2f(x0 + x2, y0 + y2);
			}
			if (c != '1' && c != '4' && c != '7' && c != '9' && c != 'A' && c != 'F') {					// BOTTOM
				x1 = (diffx*0.1875);
				y1 = (diffy*0.11538);
				x2 = (diffx*0.8125);
				y2 = (diffy*0.11538);
				if (mirror) {
					y1 = diffy - y1;	y2 = diffy - y2;
				}
				if (angle == 180 || angle == 270) {
					x1 *= -1;	y1 *= -1;	x2 *= -1;	y2 *= -1;
				}
				glVertex2f(x0 + x1, y0 + y1);
				glVertex2f(x0 + x2, y0 + y2);
			}
			if (c != '1' && c != '2' && c != '3' && c != '7' && c != 'D') {								// TL
				x1 = (diffx*0.1875);
				y1 = (diffy*0.88462);
				x2 = (diffx*0.1875);
				y2 = (diffy*0.5);
				if (mirror) {
					y1 = diffy - y1;	y2 = diffy - y2;
				}
				if (angle == 180 || angle == 270) {
					x1 *= -1;	y1 *= -1;	x2 *= -1;	y2 *= -1;
				}
				glVertex2f(x0 + x1, y0 + y1);
				glVertex2f(x0 + x2, y0 + y2);
			}
			if (c != '5' && c != '6' && c != 'B' && c != 'C' && c != 'E' && c != 'F') {					// TR
				x1 = (diffx*0.8125);
				y1 = (diffy*0.88462);
				x2 = (diffx*0.8125);
				y2 = (diffy*0.5);
				if (mirror) {
					y1 = diffy - y1;	y2 = diffy - y2;
				}
				if (angle == 180 || angle == 270) {
					x1 *= -1;	y1 *= -1;	x2 *= -1;	y2 *= -1;
				}
				glVertex2f(x0 + x1, y0 + y1);
				glVertex2f(x0 + x2, y0 + y2);
			}
			if (c != '1' && c != '3' && c != '4' && c != '5' && c != '7' && c != '9') {					// BL
				x1 = (diffx*0.1875);
				y1 = (diffy*0.11538);
				x2 = (diffx*0.1875);
				y2 = (diffy*0.5);
				if (mirror) {
					y1 = diffy - y1;	y2 = diffy - y2;
				}
				if (angle == 180 || angle == 270) {
					x1 *= -1;	y1 *= -1;	x2 *= -1;	y2 *= -1;
				}
				glVertex2f(x0 + x1, y0 + y1);
				glVertex2f(x0 + x2, y0 + y2);
			}
			if (c != '2' && c != 'C' && c != 'E' && c != 'F') {											// BR
				x1 = (diffx*0.8125);
				y1 = (diffy*0.11538);
				x2 = (diffx*0.8125);
				y2 = (diffy*0.5);
				if (mirror) {
					y1 = diffy - y1;	y2 = diffy - y2; }
				if (angle == 180 || angle == 270) {
					x1 *= -1;	y1 *= -1;	x2 *= -1;	y2 *= -1; }
				glVertex2f(x0 + x1, y0 + y1);
				glVertex2f(x0 + x2, y0 + y2);
			}
		}
	}
	glEnd();
	glLineWidth(lineWidthOld);
	glColor4f(0.0, 0.0, 0.0, 1.0);
}

void guiGateREGISTER::setLogicParam( string paramName, string value ) {
	int intVal;
	if (paramName == "INPUT_BITS") {
		// How many digits should I show? (min of 1)
		istringstream iss( value );
		iss >> renderInfo_numDigitsToShow;
		renderInfo_numDigitsToShow = (int) ceil( ((double) renderInfo_numDigitsToShow) / 4.0 );
		if (renderInfo_numDigitsToShow == 0) renderInfo_numDigitsToShow = 1;
		if (getLogicParam("CURRENT_VALUE") != "") {
			iss.clear(); iss.str( getLogicParam("CURRENT_VALUE") );
			iss >> intVal;		
	        ostringstream ossVal;
	        for (int i=2*sizeof(int) - 1; i>=0; i--) {
	            ossVal << "0123456789ABCDEF"[((intVal >> i*4) & 0xF)];
	        }
	        renderInfo_currentValue = ossVal.str().substr(ossVal.str().size()-renderInfo_numDigitsToShow,renderInfo_numDigitsToShow);
		}
	} else if (paramName == "UNKNOWN_OUTPUTS") {
		renderInfo_drawBlue = (value == "true");
	} else if (paramName == "CURRENT_VALUE") {
		istringstream iss( value );
		iss >> intVal;		
        ostringstream ossVal;
        for (int i=2*sizeof(int) - 1; i>=0; i--) {
            ossVal << "0123456789ABCDEF"[((intVal >> i*4) & 0xF)];
        }
        renderInfo_currentValue = ossVal.str().substr(ossVal.str().size()-renderInfo_numDigitsToShow,renderInfo_numDigitsToShow);
	}
	guiGate::setLogicParam(paramName, value);
}

void guiGateREGISTER::setGUIParam( string paramName, string value ) {
	if (paramName == "VALUE_BOX") {
		istringstream iss(value);
		char dump;
		iss >> renderInfo_valueBox.begin.x >> dump >> renderInfo_valueBox.begin.y >>
			dump >> renderInfo_valueBox.end.x >> dump >> renderInfo_valueBox.end.y;
		renderInfo_diffx = renderInfo_valueBox.end.x - renderInfo_valueBox.begin.x;
		renderInfo_diffy = renderInfo_valueBox.end.y - renderInfo_valueBox.begin.y;
	}
	else if (paramName == "BCD") {	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		display_BCD = (value == "true");
	}
	else if (paramName == "HIDE_DISPLAY") {	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		hide_display = (value == "true");
	}
	guiGate::setGUIParam(paramName, value);
}

// ******************** END guiGateREGISTER **********************

// *********************** guiGatePULSE *************************


guiGatePULSE::guiGatePULSE() {
	guiGate();
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Set the default CLICK box:
	renderInfo_clickBox.begin.x = 0;
	renderInfo_clickBox.begin.y = 0;
	renderInfo_clickBox.end.x = 0;
	renderInfo_clickBox.end.y = 0;
}

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Set CLICK_BOX param
void guiGatePULSE::setGUIParam(string paramName, string value) {
	if (paramName == "CLICK_BOX") {
		istringstream iss(value);
		char dump;
		iss >> renderInfo_clickBox.begin.x >> dump >> renderInfo_clickBox.begin.y >>
			dump >> renderInfo_clickBox.end.x >> dump >> renderInfo_clickBox.end.y;
	}
	guiGate::setGUIParam(paramName, value);
}

// Send a pulse message to the logic core whenever the gate is
// clicked on:
klsMessage::Message_SET_GATE_PARAM* guiGatePULSE::checkClick( GLfloat x, GLfloat y ) {
	klsBBox pulseButton;

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Return if CLICK_BOX does not exis
	if (gparams.find("CLICK_BOX") == gparams.end())
		return NULL;
	// Get the size of the CLICK square from the parameters:
	string clickBox = getGUIParam( "CLICK_BOX" );
	istringstream iss(clickBox);
	GLdouble minx = 0;
	GLdouble miny = 0;
	GLdouble maxx = 0;
	GLdouble maxy = 0;
	char dump;
	iss >> minx >> dump >> miny >> dump >> maxx >> dump >> maxy;

	pulseButton.addPoint( modelToWorld( GLPoint2f( minx, miny ) ) );
	pulseButton.addPoint( modelToWorld( GLPoint2f( minx, maxy ) ) );
	pulseButton.addPoint( modelToWorld( GLPoint2f( maxx, miny ) ) );
	pulseButton.addPoint( modelToWorld( GLPoint2f( maxx, maxy ) ) );

	if (pulseButton.contains( GLPoint2f( x, y ) )) {
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12		PULSE_WITH now is a logic param
		return new klsMessage::Message_SET_GATE_PARAM(getID(), "PULSE", getLogicParam("PULSE_WIDTH"));
	} else return NULL;
}

// ******************** END guiGatePULSE **********************


guiGateLED::guiGateLED() {
	guiGate();
	
	// Set the default LED box:
	// Format is: "minx miny maxx maxy"
	renderInfo_ledBox.begin = { 0,0 };
	renderInfo_ledBox.end  = { 0,0 };
}

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Added drawPalette to do not draw wide outlines in palette
void guiGateLED::draw(bool color, bool drawPalette) {
	StateType outputState = HI_Z;
	
	// Draw the default lines:
	guiGate::draw(color, drawPalette);

	// Get the first connected input in the LED's library description:
	// map i/o name to wire id
	map< string, guiWire* >::iterator theCnk = connections.begin();
	if( theCnk != connections.end() ) {
		outputState = (theCnk->second)->getState()[0];
	}
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12		Added Black for color=false
	// Retouch some colors to get bettter monochrome bitmaps in clipboard
	if (color)
		switch( outputState ) {
		case ZERO:
			glColor4f( 0.0, 0.0, 0.0, 1.0 );				// Black
			break;
		case ONE:
			glColor4f( 1.0, 0.0, 0.0, 1.0 );				// Red
			break;
		case HI_Z:
			glColor4f( 0.0, 1.0, 0.0, 1.0 );				// Green
			break;
		case UNKNOWN:
			glColor4f(0.0, 0.0, 1.0, 1.0);					// Blue
			break;
		case CONFLICT:
			glColor4f( 0.0, 1.0, 1.0, 1.0 );				// Cyan
			break;
		}
	else
		glColor4f(0, 0, 0, 1.0);							// Black

	//Inner Square
	glRectd  ( renderInfo_ledBox.begin.x, renderInfo_ledBox.begin.y, renderInfo_ledBox.end.x, renderInfo_ledBox.end.y ) ;

	// Set the color back to black:
	glColor4f( 0.0, 0.0, 0.0, 1.0 );
}

void guiGateLED::setGUIParam( string paramName, string value ) {
	if (paramName == "LED_BOX") {
		istringstream iss(value);
		char dump;
		iss >> renderInfo_ledBox.begin.x >> dump >> renderInfo_ledBox.begin.y >>
			dump >> renderInfo_ledBox.end.x >> dump >> renderInfo_ledBox.end.y;
	}
	guiGate::setGUIParam(paramName, value);
}

// ********************************** guiWIRE ***********************************

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Added drawPalette to do not draw wide outlines in palette
void guiGateWIRE::draw(bool color, bool drawPalette) {
	StateType outputState = HI_Z;
	
	// Get the first connected input in the WIRE's library description:
	// map i/o name to wire id
	map< string, guiWire* >::iterator theCnk = connections.begin();
	if (theCnk != connections.end()) {
		outputState = (theCnk->second)->getState()[0];
	}

	// Retouch some colors to get bettter monochrome bitmaps in clipboard
	if (!drawPalette) {
		if (color) {
			if (this->getLibraryName() == "Deprecated" && wxGetApp().appSettings.markDeprecated) {
				glColor4f(1.0f, 0.0f, 1.0f, 1.0f);				// Magenta
			}
			else {
				switch (outputState) {
				case ZERO:
					glColor4f(0.0, 0.0, 0.0, 1.0);				// Black
					break;
				case ONE:
					glColor4f(1.0, 0.0, 0.0, 1.0);				// Red
					break;
				case HI_Z:
					glColor4f(0.0, 1.0, 0.0, 1.0);				// Green
					break;
				case UNKNOWN:
					glColor4f(0.0, 0.0, 1.0, 1.0);				// Blue
					break;
				case CONFLICT:
					glColor4f(0.0, 1.0, 1.0, 1.0);				// Cyan
					break;
				}
			}
		}
		else
			glColor4f(0, 0, 0, 1.0);							// Black
		}
	else
		glColor4f(0, 0, 0, 1.0);								// Black

	// Draw the default lines:
	guiGate::draw(color, drawPalette);

	// Set the color back to black:
	glColor4f(0.0, 0.0, 0.0, 1.0);
}

// ********************************** guiLabel ***********************************

guiLabel::guiLabel() {
	guiGate();
	// Set default parameters:
	setGUIParam( "LABEL_TEXT", "BLANK" );
	setGUIParam( "TEXT_HEIGHT", "2.0" );
}

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Added drawPalette to do not draw wide outlines in palette
void guiLabel::draw(bool color, bool drawPalette) {	

	// Position the gate at its x and y coordinates:
	glLoadMatrixd(mModel);
	
	// Pick the color for the text:
	if( selected && color ) {
		GLdouble c = 1.0 - SELECTED_LABEL_INTENSITY;
		theText.setColor( 1.0, c / 4, c / 4, SELECTED_LABEL_INTENSITY );
	} else {
		theText.setColor( 0.0, 0.0, 0.0, 1.0 );
	}
	
	// Pedro Casanova (casanova@ujaen.es) 2021/01-02
	if (this->getGUIParam("angle") == "180" || this->getGUIParam("angle") == "270") {
		glRotatef(180, 0.0, 0.0, 1.0);
	}

	if (this->getGUIParam("mirror") == "true")
		glScalef(1, -1, 1);

	// Draw the text:
	theText.draw();

	guiGate::draw(color, drawPalette);
}

// A custom setParam function is required because
// the object must resize it's bounding box 
// each time the LABEL_TEXT or TEXT_HEIGHT parameter is set.
void guiLabel::setGUIParam( string paramName, string value ) {
	if( (paramName == "LABEL_TEXT") || (paramName == "TEXT_HEIGHT")  ) {

		if( paramName == "TEXT_HEIGHT" ) {
			// Make the text parameter safe:
			istringstream iss(value);
			GLdouble textHeight = 1.0;
			iss >> textHeight;

			if( textHeight < 0 ) textHeight = -textHeight;
			if( textHeight < 0.01 ) textHeight = 0.01;
			
			ostringstream oss;
			oss << textHeight;
			value = oss.str();
		}
	
		guiGate::setGUIParam( paramName, value );

		string labelText = getGUIParam("LABEL_TEXT");
		GLdouble height = getTextHeight();
		theText.setSize( height );
		theText.setText( labelText );

		//Sets bounding box size
		this->calcBBox();
	} else {
		guiGate::setGUIParam( paramName, value );
	}
}

void guiLabel::calcBBox( void ) {
	GLbox textBBox = theText.getBoundingBox();
	// Pedro Casanova (casanova@ujaen.es) 2021/01-02
	// To center vertically 0.75 instead of 0.5 in dy
	float dx = fabs(textBBox.right - textBBox.left) * 0.5f;
	float dy = fabs(textBBox.top - textBBox.bottom) * 0.75f;
	theText.setPosition(-dx, +dy);
	modelBBox.reset();
	modelBBox.addPoint(GLPoint2f(textBBox.left - dx, textBBox.bottom + dy));
	modelBBox.addPoint( GLPoint2f(textBBox.right-dx, textBBox.top+dy) );

	// Recalculate the world-space bbox:
	updateBBoxes();
}


// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Modified to permit variable labes size (LINK).
// ************************ TO/FROM gate *************************
guiTO_FROM::guiTO_FROM() {
	// Note that I don't set the JUNCTION_ID parameter yet, because
	// that would call setParam() and that would call calcBBox()
	// and that wants to know that the gate's type is, which we don't know yet.

	guiGate();

}

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Added drawPalette to do not draw wide outlines in palette
void guiTO_FROM::draw(bool color, bool drawPalette) {
	// Draw the lines for this gate:
	guiGate::draw(color, drawPalette);

	// Position the gate at its x and y coordinates:
	glLoadMatrixd(mModel);
	
	// Pick the color for the text:
	if( selected && color ) {
		GLdouble c = 1.0 - SELECTED_LABEL_INTENSITY;
		theText.setColor( 1.0, c / 4, c / 4, SELECTED_LABEL_INTENSITY );
	} else {
		theText.setColor( 0.0, 0.0, 0.0, 1.0 );
	}
	
	//********************************
	//Edit by Joshua Lansford 04/04/07
	//Upside down text on tos and froms
	//isn't that exciting.
	//This will rotate the text around
	//before it is printed		
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12		Changed 90 to 270 to rotate same as Label
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12		TO, FROM and LINK are diferent

	float translate=0;
	float rotate=0;
	GLbox textBBox = theText.getBoundingBox();
	GLdouble textWidth = textBBox.right - textBBox.left;
	float offx = fabs(textBBox.top - textBBox.bottom) * 0.5 * 0.675;
	if (getGUIType() == "TO") {
		if (this->getGUIParam("angle") == "180" || this->getGUIParam("angle") == "270") {
			translate = textWidth + offx + 0.2;
			rotate = 180;
		}
	} else if (getGUIType() == "FROM") {
		if (this->getGUIParam("angle") == "180" || this->getGUIParam("angle") == "270") {
			translate = -(textWidth + offx);
			rotate = 180;
		} else {
			translate = -0.2f;
		}
	} else if (getGUIType() == "LINK") {
		if (this->getGUIParam("angle") == "180" || this->getGUIParam("angle") == "270") {
			translate = textWidth + offx + 0.2;
			rotate = 180;
		}
	}

	glTranslatef(translate, 0, 0);
	glRotatef(rotate, 0, 0, 1);

	if (this->getGUIParam("mirror") == "true")
		glScalef(1, -1, 1);

	//End of Edit*********************

	// Draw the text:
	theText.draw();
}

// A custom setParam function is required because
// the object must resize it's bounding box 
// each time the JUNCTION_ID parameter is set.
void guiTO_FROM::setLogicParam( string paramName, string value ) {
	if( paramName == "JUNCTION_ID" ) {
		guiGate::setLogicParam( paramName, value );

		string labelText = getLogicParam("JUNCTION_ID");
		theText.setText( labelText );
		GLdouble height = getTextHeight();
		theText.setSize(height);

		//Sets bounding box size
		this->calcBBox();
	} else {
		guiGate::setLogicParam( paramName, value );
	}
}

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// A custom setParam function is required because
// the object must resize it's bounding box 
// each time the TEXT_HEIGHT parameter is set.
void guiTO_FROM::setGUIParam(string paramName, string value) {
	if (paramName == "TEXT_HEIGHT") {

		// Make the text parameter safe:
		istringstream iss(value);
		GLdouble textHeight = 1.0;
		iss >> textHeight;

		if (textHeight < 0) textHeight = -textHeight;
		if (textHeight < 0.01) textHeight = 0.01;

		ostringstream oss;
		oss << textHeight;
		value = oss.str();

		guiGate::setGUIParam(paramName, value);

		string labelText = getLogicParam("JUNCTION_ID");
		GLdouble height = getTextHeight();

		theText.setSize(height);
		theText.setText(labelText);

		//Sets bounding box size
		this->calcBBox();
	}
	else {
		guiGate::setGUIParam(paramName, value);
	}
}

void guiTO_FROM::calcBBox( void ) {
	
	// Set the gate's bounding box based on the lines:
	guiGate::calcBBox();

	// Get the text's bounding box:	
	GLbox textBBox = theText.getBoundingBox();

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// LINK text is now variable

	float dx = fabs(textBBox.right - textBBox.left) / 2.0f;
	float dy = fabs(textBBox.top - textBBox.bottom) / 2.0f;

	float offy = fabs(textBBox.top - textBBox.bottom) * 0.3f / 1.5f;
	float offx = fabs(textBBox.top - textBBox.bottom) * 0.4f / 1.5f;

// Adjust the bounding box based on the text's bbox:
	GLdouble textWidth = textBBox.right - textBBox.left;
	GLdouble textHeight = textBBox.top- textBBox.bottom;
	if (getGUIType() == "TO") {
		GLPoint2f bR = modelBBox.getBottomRight();
		bR.x += textWidth+offx;
		modelBBox.addPoint(bR);
		theText.setPosition(offx, dy + offy);
	}
	else if (getGUIType() == "FROM") {
		GLPoint2f tL = modelBBox.getTopLeft();
		tL.x -= textWidth+offx;
		modelBBox.addPoint(tL);
		theText.setPosition(tL.x + 0.2f, dy + offy);
	}
	else if (getGUIType() == "LINK") {	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		GLPoint2f bR = modelBBox.getBottomRight();
		bR.x += textWidth+offx;
		bR.y -= 1.1f * textHeight / 2.0f;
		modelBBox.addPoint(bR);
		bR.y += 1.1f * textHeight;
		modelBBox.addPoint(bR);
		theText.setPosition(offx, dy + offy);
	}

	// Recalculate the world-space bbox:
	updateBBoxes();
}

// ************************ RAM gate ****************************
// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Modified to limit data size values.
//*************************************************
//Edit by Joshua Lansford 12/25/2006
//I am creating a guiGate for the RAM so that
//the ram can have its own special pop-up window
guiGateRAM::guiGateRAM(){
	guiGate();
	ramPopupDialog = NULL;
}

guiGateRAM::~guiGateRAM(){	
	//Destroy is how you 'delete' wxwidget objects
	if( ramPopupDialog != NULL ){
		ramPopupDialog->Destroy();
		ramPopupDialog = NULL;
	}
}


void guiGateRAM::doParamsDialog(void* gCircuit, wxCommandProcessor* wxcmd){
	if( ramPopupDialog == NULL ){
		ramPopupDialog = new RamPopupDialog( this, (GUICircuit*)gCircuit );
		ramPopupDialog->updateGridDisplay();
	}
	ramPopupDialog->SetFocus();
	ramPopupDialog->Show( true );
}

//Saves the ram contents to the circuit file
//when the circuit saves
void guiGateRAM::saveGateTypeSpecifics( XMLParser* xparse ){

	int dataSize;
	istringstream iss(getLogicParam("DATA_BITS"));
	iss >> dataSize;

	for( map< unsigned long, unsigned long >::iterator I = memory.begin();
	     	I != memory.end();  ++I ){
		 I->second = I->second & ((int)pow((float)2, (int)dataSize) - 1);
	     if( I->second != 0 ){			
			xparse->openTag("lparam");
		    ostringstream memoryValue;
			memoryValue << "Address:" << I->first << " " << I->second; 
			xparse->writeTag("lparam", memoryValue.str());
			xparse->closeTag("lparam");
	     }
	}
}

//Because the ram gui will be passed lots of data
//from the ram logic, we don't want it all going
//into the default hash of changed paramiters.
//Thus we catch it here
void guiGateRAM::setLogicParam( string paramName, string value ){
	
	if( paramName.substr( 0, 8 ) == "lastRead" ){	
		//this makes it so that the pop-up will green things
		//that have been just read
		istringstream addressiss( value );
		unsigned long address = 0;
		addressiss >> address;
		lastRead = address;
		if( ramPopupDialog != NULL )
			ramPopupDialog->updateGridDisplay();
	}else if( paramName.substr( 0, 8 ) == "Address:" ){
		istringstream addressiss( paramName.substr( 8 ) );
		unsigned long address = 0;
		addressiss >> address;
		istringstream dataiss( value );
		unsigned long data = 0;
		dataiss >> data;
		memory[ address ] = data;
		if( ramPopupDialog != NULL )
			ramPopupDialog->updateGridDisplay();
		lastWritten = address;
	}else if( paramName == "MemoryReset" ){
		memory.clear();
		if( ramPopupDialog != NULL )
		    ramPopupDialog->notifyAllChanged();
	}else if( paramName == "ADDRESS_BITS" ) {
		istringstream dataiss( value );
		dataiss >> addressBits;
		guiGate::setLogicParam( paramName, value );
		// Declare the address pins!		
	} else if( paramName == "DATA_BITS" ) {
		istringstream dataiss( value );
		dataiss >> dataBits;
		guiGate::setLogicParam( paramName, value );
	} else {
		guiGate::setLogicParam( paramName, value );
	}
}

//This method is used by the RamPopupDialog to
//learn what values are at different addresses
//in memory.
unsigned long guiGateRAM::getValueAt( unsigned long address ){
	//we want to refrain from creating an entry for an item
	//if it does not already exist.  Therefore we will not
	//use the [] operator
	map<unsigned long, unsigned long>::iterator finder = memory.find( address );
	if( finder != memory.end() )
		return finder->second;
	return 0;
}

//These is used by the pop-up to determine
//what was the last value read and written
long guiGateRAM::getLastWritten(){
	return lastWritten;
}
long guiGateRAM::getLastRead(){
	return lastRead;
}
//End of edit
//*************************************************

// ************************ FSM gate ****************************
// Pedro Casanova (casanova@ujaen.es) 2021/01-02
guiGateFSM::guiGateFSM() {
	guiGate();
	fsmParamDialog = NULL;
}

guiGateFSM::~guiGateFSM() {
	//Destroy is how you 'delete' wxwidget objects
	if (fsmParamDialog != NULL) {
		fsmParamDialog->Destroy();
		fsmParamDialog = NULL;
	}
}

void guiGateFSM::doParamsDialog(void* gCircuit, wxCommandProcessor* wxcmd) {
	if (fsmParamDialog == NULL) {
		fsmParamDialog = new FSMParamDialog(this, (GUICircuit*)gCircuit, wxcmd);
	}
	
	map <string, string>* logicParams = getAllLogicParams();
	map <string, string>::iterator lparamsWalk = logicParams->begin();
	unsigned int nStates=0;
	while (lparamsWalk != logicParams->end()) {
		if ((lparamsWalk->first).substr(0, 6) == "State:")
			if (lparamsWalk->second != "")
				nStates++;
				
		lparamsWalk++;
	}
	
	fsmParamDialog->stateTX->Clear();

	for (unsigned int i = 0; i < nStates; i++)
	{
		ostringstream oss;
		oss << "State:" << i;
		string value = logicParams->find(oss.str())->second;
		if (value!="")
			*fsmParamDialog->stateTX << value << "\n";
	}
	
	fsmParamDialog->SetFocus();
	fsmParamDialog->ShowModal();
}

void guiGateFSM::saveGateTypeSpecifics(XMLParser* xparse) {

	map <string, string>* logicParams = getAllLogicParams();
	map <string, string>::iterator lparamsWalk = logicParams->begin();
	unsigned int nStates = 0;
	while (lparamsWalk != logicParams->end()) {
		if ((lparamsWalk->first).substr(0, 6) == "State:")
			if (lparamsWalk->second != "")
				nStates++;

		lparamsWalk++;
	}

	for (unsigned int i = 0; i < nStates; i++)
	{
		ostringstream oss;
		oss << "State:" << i;
		string value = logicParams->find(oss.str())->second;
		if (value != "") {
			xparse->openTag("lparam");
			oss << " " << value;
			xparse->writeTag("lparam", oss.str());
			xparse->closeTag("lparam");
		}
	}

}

//*************************************************

// ************************ CMB gate ****************************
// Pedro Casanova (casanova@ujaen.es) 2021/01-02
guiGateCMB::guiGateCMB() {
	guiGate();
	cmbParamDialog = NULL;
}

guiGateCMB::~guiGateCMB() {
	//Destroy is how you 'delete' wxwidget objects
	if (cmbParamDialog != NULL) {
		cmbParamDialog->Destroy();
		cmbParamDialog = NULL;
	}
}

void guiGateCMB::doParamsDialog(void* gCircuit, wxCommandProcessor* wxcmd) {
	if (cmbParamDialog == NULL) {
		cmbParamDialog = new CMBParamDialog(this, (GUICircuit*)gCircuit, wxcmd);
	}

	map <string, string>* logicParams = getAllLogicParams();
	map <string, string>::iterator lparamsWalk = logicParams->begin();
	unsigned int nFunctions = 0;
	while (lparamsWalk != logicParams->end()) {
		if ((lparamsWalk->first).substr(0, 9) == "Function:")
			if (lparamsWalk->second != "")
				nFunctions++;
		lparamsWalk++;
	}

	cmbParamDialog->functionTX->Clear();

	for (unsigned int i = 0; i < nFunctions; i++)
	{
		ostringstream oss;
		oss << "Function:" << i;
		string value = logicParams->find(oss.str())->second;
		if (value != "")
			*cmbParamDialog->functionTX << value << "\n";
	}

	cmbParamDialog->SetFocus();
	cmbParamDialog->ShowModal();
}

void guiGateCMB::saveGateTypeSpecifics(XMLParser* xparse) {

	map <string, string>* logicParams = getAllLogicParams();
	map <string, string>::iterator lparamsWalk = logicParams->begin();
	unsigned int nStates = 0;
	while (lparamsWalk != logicParams->end()) {
		if ((lparamsWalk->first).substr(0, 9) == "Function:")
			if (lparamsWalk->second != "")
				nStates++;

		lparamsWalk++;
	}

	for (unsigned int i = 0; i < nStates; i++)
	{
		ostringstream oss;
		oss << "Function:" << i;
		string value = logicParams->find(oss.str())->second;
		if (value != "") {
			xparse->openTag("lparam");
			oss << " " << value;
			xparse->writeTag("lparam", oss.str());
			xparse->closeTag("lparam");
		}
	}

}

//*************************************************
