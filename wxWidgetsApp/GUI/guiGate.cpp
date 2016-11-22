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
	gparams["angle"] = "0.0";
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
	glRotatef( angle, 0.0, 0.0, 1.0);
	
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

// Convert model->world coordinates:
GLPoint2f guiGate::modelToWorld( GLPoint2f c ) {

	// Perform a matrix-vector multiply to get the point coordinates in world-space:
	GLfloat x = c.x * mModel[0] + c.y * mModel[4] + 1.0*mModel[12];
	GLfloat y = c.x * mModel[1] + c.y * mModel[5] + 1.0*mModel[13];

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

void guiGate::draw(bool color) {

	GLint oldStipple = 0; // The old line stipple pattern, if needed.
	GLint oldRepeat = 0;  // The old line stipple repeat pattern, if needed.
	GLboolean lineStipple = false; // The old line stipple enable flag, if needed.

	// Position the gate at its x and y coordinates:
	glLoadMatrixd(mModel);


	if( selected && color ) {
		// Store the old line stipple pattern:
		lineStipple = glIsEnabled( GL_LINE_STIPPLE );
		glGetIntegerv( GL_LINE_STIPPLE_PATTERN, &oldStipple );
		glGetIntegerv( GL_LINE_STIPPLE_REPEAT, &oldRepeat );
	
		// Draw the gate with dotted lines:
		glEnable( GL_LINE_STIPPLE );
		glLineStipple( 1, 0x9999 );
	}

	// Draw the gate:
	glBegin(GL_LINES);
	for( unsigned int i = 0; i < vertices.size(); i++ ) {
		glVertex2f( vertices[i].x, vertices[i].y );
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
void guiGate::insertLine( float x1, float y1, float x2, float y2 ) {
	vertices.push_back( GLPoint2f( x1, y1 ) );
	vertices.push_back( GLPoint2f( x2, y2 ) );
}


// Recalculate the bounding box, based on the lines that are included already:
void guiGate::calcBBox( void ) {
	modelBBox.reset();
	
	for( unsigned int i = 0; i < vertices.size(); i++ ) {
		modelBBox.addPoint( vertices[i] );
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
	float x, y;
	getHotspotCoords( hsName, x, y );
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

void guiGate::doParamsDialog( void* gc, wxCommandProcessor* wxcmd ) {
	if (wxGetApp().libraries[libName][libGateName].dlgParams.size() == 0) return;
	paramDialog myDialog(wxT("Parameters"), gc, this, wxcmd);
	myDialog.SetFocus();
	myDialog.ShowModal();
}

// *********************** guiGateTOGGLE *************************

guiGateTOGGLE::guiGateTOGGLE() {
	guiGate();
	// Default to "off" state when creating a toggle gate:
	//NOTE: Does not send this to the core, just updates it
	// on the GUI side.
	setLogicParam( "OUTPUT_NUM", "0" );
	
	// Set the default CLICK box:
	// Format is: "minx miny maxx maxy"
	setGUIParam( "CLICK_BOX", "-0.76,-0.76,0.76,0.76" );
}

void guiGateTOGGLE::draw( bool color ) {
	// Draw the default lines:
	guiGate::draw(color);
	
	// Add the rectangle:
	glColor4f( (float)renderInfo_outputNum, 0.0, 0.0, 1.0 );

	//Inner Square
	if (color) glRectd  ( renderInfo_clickBox.begin.x, renderInfo_clickBox.begin.y, 
			renderInfo_clickBox.end.x, renderInfo_clickBox.end.y ) ;
	
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

	// Get the size of the CLICK square from the parameters:
	string clickBox = getGUIParam( "CLICK_BOX" );
	istringstream iss(clickBox);
	GLdouble minx = -0.5;
	GLdouble miny = -0.5;
	GLdouble maxx = 0.5;
	GLdouble maxy = 0.5;
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

void guiGateKEYPAD::draw( bool color ) {
	// Position the gate at its x and y coordinates:
	glLoadMatrixd(mModel);
	
	// Add the rectangle - this is a highlight so needs done before main gate draw:
	glColor4f( 0.0, 0.4f, 1.0, 0.3f );

	//Inner Square
	if (color) glRectd  ( renderInfo_valueBox.begin.x, renderInfo_valueBox.begin.y, 
			renderInfo_valueBox.end.x, renderInfo_valueBox.end.y ) ;
	
	// Set the color back to the old color:
	glColor4f( 0.0, 0.0, 0.0, 1.0 );

	// Draw the default lines:
	guiGate::draw(color);
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
		
		keyButton.addPoint( modelToWorld( GLPoint2f( minx, miny ) ) );
		keyButton.addPoint( modelToWorld( GLPoint2f( minx, maxy ) ) );
		keyButton.addPoint( modelToWorld( GLPoint2f( maxx, miny ) ) );
		keyButton.addPoint( modelToWorld( GLPoint2f( maxx, maxy ) ) );
	
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
	//NOTE: Does not send this to the core, just updates it
	// on the GUI side.
	renderInfo_numDigitsToShow = 1;
	setLogicParam( "CURRENT_VALUE", "0" );
	setLogicParam( "UNKNOWN_OUTPUTS", "false" );
}

void guiGateREGISTER::draw( bool color ) {
	// Draw the default lines:
	guiGate::draw(color);

	float diffx = renderInfo_diffx;
	float diffy = renderInfo_diffy;
	
	diffx /= (double)renderInfo_numDigitsToShow; // set width of each digit
	
	//Inner Square for value
	if (color) {
		// Display box
		glBegin( GL_LINE_LOOP );
			glVertex2f(renderInfo_valueBox.begin.x,renderInfo_valueBox.begin.y);
			glVertex2f(renderInfo_valueBox.begin.x,renderInfo_valueBox.end.y);
			glVertex2f(renderInfo_valueBox.end.x,renderInfo_valueBox.end.y);
			glVertex2f(renderInfo_valueBox.end.x,renderInfo_valueBox.begin.y);
		glEnd();
		
		// Draw the number in red (or blue if inputs are not all sane)
		if (renderInfo_drawBlue) glColor4f( 0.3f, 0.3f, 1.0, 1.0 );
		else glColor4f( 1.0, 0.0, 0.0, 1.0 );

		GLfloat lineWidthOld;
		glGetFloatv(GL_LINE_WIDTH, &lineWidthOld);
		glLineWidth(2.0);
        
		// THESE ARE ALL SEVEN SEGMENTS WITH DIFFERENTIAL COORDS.  USE THEM FOR EACH DIGIT VALUE FOR EACH DIGIT
		//		AND INCREMENT CURRENTDIGIT.  CURRENTDIGIT=0 IS MSB.
		glBegin( GL_LINES );
		for (unsigned int currentDigit = 0; currentDigit < renderInfo_currentValue.size(); currentDigit++) {
			char c = renderInfo_currentValue[currentDigit];
			if ( c != '1' && c != '4' && c != 'B' && c != 'D' ) {
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.1875),renderInfo_valueBox.begin.y+(diffy*0.88462)); // TOP
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.8125),renderInfo_valueBox.begin.y+(diffy*0.88462)); }
			if ( c != '0' && c != '1' && c != '7' && c != 'C' ) {
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.1875),renderInfo_valueBox.begin.y+(diffy*0.5)); // MID
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.8125),renderInfo_valueBox.begin.y+(diffy*0.5)); }
			if ( c != '1' && c != '4' && c != '7' && c != '9' && c != 'A' && c != 'F' ) {
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.1875),renderInfo_valueBox.begin.y+(diffy*0.11538)); // BOTTOM
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.8125),renderInfo_valueBox.begin.y+(diffy*0.11538)); }
			if ( c != '1' && c != '2' && c != '3' && c != '7' && c != 'D' ) {
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.1875),renderInfo_valueBox.begin.y+(diffy*0.88462)); // TL
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.1875),renderInfo_valueBox.begin.y+(diffy*0.5)); }
			if ( c != '5' && c != '6' && c != 'B' && c != 'C' && c != 'E' && c != 'F' ) {
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.8125),renderInfo_valueBox.begin.y+(diffy*0.88462)); // TR
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.8125),renderInfo_valueBox.begin.y+(diffy*0.5)); }
			if ( c != '1' && c != '3' && c != '4' && c != '5' && c != '7' && c != '9' ) {
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.1875),renderInfo_valueBox.begin.y+(diffy*0.11538)); // BL
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.1875),renderInfo_valueBox.begin.y+(diffy*0.5)); }
			if ( c != '2' && c != 'C' && c != 'E' && c != 'F' ) {
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.8125),renderInfo_valueBox.begin.y+(diffy*0.11538)); // BR
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.8125),renderInfo_valueBox.begin.y+(diffy*0.5)); }
		}
		glEnd();
		glLineWidth(lineWidthOld);
		glColor4f( 0.0, 0.0, 0.0, 1.0 );
	}
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
	guiGate::setGUIParam(paramName, value);
}

// ******************** END guiGateREGISTER **********************

// *********************** guiGatePULSE *************************


// Send a pulse message to the logic core whenever the gate is
// clicked on:
klsMessage::Message_SET_GATE_PARAM* guiGatePULSE::checkClick( GLfloat x, GLfloat y ) {
	klsBBox toggleButton;

	// Get the size of the CLICK square from the parameters:
	string clickBox = getGUIParam( "CLICK_BOX" );
	istringstream iss(clickBox);
	GLdouble minx = -0.5;
	GLdouble miny = -0.5;
	GLdouble maxx = 0.5;
	GLdouble maxy = 0.5;
	char dump;
	iss >> minx >> dump >> miny >> dump >> maxx >> dump >> maxy;

	toggleButton.addPoint( modelToWorld( GLPoint2f( minx, miny ) ) );
	toggleButton.addPoint( modelToWorld( GLPoint2f( minx, maxy ) ) );
	toggleButton.addPoint( modelToWorld( GLPoint2f( maxx, miny ) ) );
	toggleButton.addPoint( modelToWorld( GLPoint2f( maxx, maxy ) ) );

	if (toggleButton.contains( GLPoint2f( x, y ) )) {
/*		ostringstream oss;
		oss << "SET GATE ID " << getID() << " PARAMETER PULSE " << getGUIParam("PULSE_WIDTH"); */
		return new klsMessage::Message_SET_GATE_PARAM(getID(), "PULSE", getGUIParam("PULSE_WIDTH"));
	} else return NULL;
}

// ******************** END guiGatePULSE **********************


guiGateLED::guiGateLED() {
	guiGate();
	
	// Set the default LED box:
	// Format is: "minx miny maxx maxy"
	setGUIParam( "LED_BOX", "-0.76,-0.76,0.76,0.76" );
}

void guiGateLED::draw( bool color ) {
	StateType outputState = HI_Z;
	
	// Draw the default lines:
	guiGate::draw(color);
	
	// Get the first connected input in the LED's library description:
	// map i/o name to wire id
	map< string, guiWire* >::iterator theCnk = connections.begin();
	if( theCnk != connections.end() ) {
		outputState = (theCnk->second)->getState()[0];
	}

	switch( outputState ) {
	case ZERO:
		glColor4f( 0.0, 0.0, 0.0, 1.0 );
		break;
	case ONE:
		glColor4f( 1.0, 0.0, 0.0, 1.0 );
		break;
	case HI_Z:
		glColor4f( 0.0, 0.78f, 0.0, 1.0 );
		break;
	case UNKNOWN:
		glColor4f( 0.3f, 0.3f, 1.0, 1.0 );
		break;
	case CONFLICT:
		glColor4f( 0.0, 1.0, 1.0, 1.0 );
		break;
	}

	//Inner Square
	if (color) glRectd  ( renderInfo_ledBox.begin.x, renderInfo_ledBox.begin.y, 
			renderInfo_ledBox.end.x, renderInfo_ledBox.end.y ) ;

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

// ********************************** guiLabel ***********************************


guiLabel::guiLabel() {
	guiGate();
	// Set default parameters:
	setGUIParam( "LABEL_TEXT", "BLANK" );
	setGUIParam( "TEXT_HEIGHT", "2.0" );
}

void guiLabel::draw( bool color ) {
	// Position the gate at its x and y coordinates:
	glLoadMatrixd(mModel);
	
	// Pick the color for the text:
	if( selected && color ) {
		GLdouble c = 1.0 - SELECTED_LABEL_INTENSITY;
		theText.setColor( 1.0, c / 4, c / 4, SELECTED_LABEL_INTENSITY );
	} else {
		theText.setColor( 0.0, 0.0, 0.0, 1.0 );
	}
	
	// Draw the text:
	theText.draw();
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
	float dx = fabs(textBBox.right-textBBox.left)/2.;
	float dy = fabs(textBBox.top-textBBox.bottom)/2.;
	double currentX, currentY; theText.getPosition(currentX, currentY);
	theText.setPosition(-dx, +dy);
	modelBBox.reset();
	modelBBox.addPoint( GLPoint2f(textBBox.left-dx, textBBox.bottom+dy) );
	modelBBox.addPoint( GLPoint2f(textBBox.right-dx, textBBox.top+dy) );

	// Recalculate the world-space bbox:
	updateBBoxes();
}



// ************************ TO/FROM gate *************************

guiTO_FROM::guiTO_FROM() {
	// Note that I don't set the JUNCTION_ID parameter yet, because
	// that would call setParam() and that would call calcBBox()
	// and that wants to know that the gate's type is, which we don't know yet.
	
	guiGate();
	
	// Initialize the text object:
	theText.setSize( TO_FROM_TEXT_HEIGHT );
}

void guiTO_FROM::draw( bool color ) {
	// Draw the lines for this gate:
	guiGate::draw();

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
	if( this->getGUIParam( "angle" ) == "180" ||
	    this->getGUIParam( "angle" ) ==  "90" ){
		
		//scoot the label over
		GLbox textBBox = theText.getBoundingBox();
		GLdouble textWidth = textBBox.right - textBBox.left;
		int direction = 0;
		if( getGUIType() == "TO" ) {
			direction = +1;
		} else if (getGUIType() == "FROM") {
			direction = -1;
		}
		glTranslatef( direction * (textWidth + FLIPPED_OFFSET), 0, 0 );
		
		//and spin it around
		glRotatef( 180, 0.0, 0.0, 1.0);
	}
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
		theText.setSize( TO_FROM_TEXT_HEIGHT );

		//Sets bounding box size
		this->calcBBox();
	} else {
		guiGate::setLogicParam( paramName, value );
	}
}

void guiTO_FROM::calcBBox( void ) {
	
	// Set the gate's bounding box based on the lines:
	guiGate::calcBBox();

	// Get the text's bounding box:	
	GLbox textBBox = theText.getBoundingBox();

	// Adjust the bounding box based on the text's bbox:
	GLdouble textWidth = textBBox.right - textBBox.left;
	if( getGUIType() == "TO" ) {
		GLPoint2f bR = modelBBox.getBottomRight();
		bR.x += textWidth;
		modelBBox.addPoint( bR );
		theText.setPosition( TO_BUFFER, TO_FROM_TEXT_HEIGHT/2+0.30 );
	} else if (getGUIType() == "FROM") {
		GLPoint2f tL = modelBBox.getTopLeft();
		tL.x -= (textWidth + FROM_BUFFER);
		modelBBox.addPoint( tL );
		theText.setPosition( tL.x + FROM_FIX_SHIFT, TO_FROM_TEXT_HEIGHT/2+0.30 );
	}

	// Recalculate the world-space bbox:
	updateBBoxes();
}

// ************************ RAM gate ****************************

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


void guiGateRAM::doParamsDialog( void* gc, wxCommandProcessor* wxcmd ){
	if( ramPopupDialog == NULL ){
		ramPopupDialog = new RamPopupDialog( this, addressBits, (GUICircuit*)gc );
		ramPopupDialog->updateGridDisplay();
	}
	ramPopupDialog->Show( true );
}

//Saves the ram contents to the circuit file
//when the circuit saves
void guiGateRAM::saveGateTypeSpecifics( XMLParser* xparse ){
	for( map< unsigned long, unsigned long >::iterator I = memory.begin();
	     	I != memory.end();  ++I ){
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
	}else{ 
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

// ************************ z80 gate ****************************

//*************************************************
//Edit by us 1/26/2006
//I am creating a guiGate for the z80 so that
//the Z80 can have its own special pop-up window
guiGateZ80::guiGateZ80(){
	guiGate();
	z80PopupDialog = NULL;
}

guiGateZ80::~guiGateZ80(){	
	//Destroy is how you 'delete' wxwidget objects
	if( z80PopupDialog != NULL ){
		z80PopupDialog->Destroy();
		z80PopupDialog = NULL;
	}
}


void guiGateZ80::doParamsDialog( void* gc, wxCommandProcessor* wxcmd ){
	if( z80PopupDialog == NULL ){
		z80PopupDialog = new Z80PopupDialog( this, (GUICircuit*)gc );
	}
	z80PopupDialog->Show( true );
}

void guiGateZ80::setLogicParam( string paramName, string value ){
	

	guiGate::setLogicParam( paramName, value );
	
	if (z80PopupDialog != NULL) {
		z80PopupDialog->NotifyOfRegChange();
	}
	
}
//End of edit
//*************************************************

// ************************ ADC gate ****************************

//*************************************************
//Edit by Joshua Lansford 05/10/2007
//I am creating a guiGate for the ADC so that
//the ADC can have its own special pop-up window
guiGateADC::guiGateADC(){
	guiGate();
	aDCPopupDialog = NULL;
}

guiGateADC::~guiGateADC(){	
	//Destroy is how you 'delete' wxwidget objects
	if( aDCPopupDialog != NULL ){
		aDCPopupDialog->Destroy();
		aDCPopupDialog = NULL;
	}
}


void guiGateADC::doParamsDialog( void* gc, wxCommandProcessor* wxcmd ){
	if( aDCPopupDialog == NULL ){
		aDCPopupDialog = new ADCPopupDialog( this, (GUICircuit*)gc );
		aDCPopupDialog->notifyValueChanged();
	}
	aDCPopupDialog->Show( true );
}

void guiGateADC::setLogicParam( string paramName, string value ){
	guiGate::setLogicParam( paramName, value );
	
	if( paramName == "VALUE" && aDCPopupDialog != NULL )
		aDCPopupDialog->notifyValueChanged();
}

//End of edit
//*************************************************
