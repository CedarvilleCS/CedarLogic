
#include "guiGate.h"

#include "gui/MainApp.h"
#include "gui/dialog/paramDialog.h"
#include "gui/wire/guiWire.h"
#include "gui/graphics/gl_defs.h"
#include "gateHotspot.h"
DECLARE_APP(MainApp)

guiGate::guiGate() : klsCollisionObject(COLL_GATE) {
	myX = 1.0;
	myY = 1.0;
	selected = false;
	gparams["angle"] = "0.0";
}

guiGate::~guiGate() {
	
	for (auto &p : hotspots) {
		//delete p.second;
	}
}

void guiGate::setID(long nid) {
	gateID = nid;
};

unsigned long guiGate::getID() {
	return gateID;
};

void guiGate::setLibraryName(string nLibName, string nLibGateName) {
	libName = nLibName;
	libGateName = nLibGateName;
};

string guiGate::getLibraryName() {
	return libName;
};

string guiGate::getLibraryGateName() {
	return libGateName;
};

string guiGate::getLogicType() {
	return wxGetApp().libParser.getGateLogicType(libGateName);
};

string guiGate::getGUIType() {
	return wxGetApp().libParser.getGateGUIType(libGateName);
};



void guiGate::setGLcoords(float x, float y, bool noUpdateWires) {
	this->myX = x;
	this->myY = y;

	// Update the matrices and bounding box:
	updateBBoxes(noUpdateWires);
}

void guiGate::getGLcoords(float &x, float &y) {
	x = this->myX;
	y = this->myY;
}

void guiGate::translateGLcoords(float x, float y) {
	setGLcoords(this->myX + x, this->myY + y);
}

void guiGate::finalizeWirePlacements() {
	updateConnectionMerges();
}

map < string, string >* guiGate::getAllGUIParams() {
	return &gparams;
}

map < string, string >* guiGate::getAllLogicParams() {
	return &lparams;
}

void guiGate::declareInput(string name) {
	isInput[name] = true;
}

void guiGate::declareOutput(string name) {
	isInput[name] = false;
}

// Draw this gate as unselected:
void guiGate::unselect(void) {
	selected = false;
}

void guiGate::select() {
	selected = true;
}

bool guiGate::clickSelect(float x, float y) {
	if (this->getBBox().contains(Point(x, y))) {
		selected = true;
		return true;
	}
	else {
		return false;
	}
}

// Insert a line in the line list.
void guiGate::insertLine(float x1, float y1, float x2, float y2) {
	vertices.push_back(Point(x1, y1));
	vertices.push_back(Point(x2, y2));
}

// Insert a hotspot in the hotspot list.
void guiGate::insertHotspot(float x1, float y1, string connection, int busLines) {
	if (hotspots.find(connection) != hotspots.end()) return; // error: hotspot already exists

	gateHotspot* newHS = new gateHotspot(connection);
	newHS->modelLocation = Point(x1, y1);
	newHS->setBusLines(busLines);

	// Add the hs to the gate's struct:
	hotspots[connection] = newHS;

	// Add the hs to the gate's sub-object list:
	this->insertSubObject(newHS);

	// Update the hotspot's world-space bbox:
	updateBBoxes();
}

// Check if any of the hotspots of this gate are within the delta
// of the world coordinates sX and sY. delta is in gl coords.
string guiGate::checkHotspots(float x, float y, float delta) {
	// Set up the mouse as a collision object:
	klsCollisionObject mouse(COLL_MOUSEBOX);
	klsBBox mBox;
	mBox.addPoint(Point(x, y));
	mBox.extendTop(delta);
	mBox.extendBottom(delta);
	mBox.extendLeft(delta);
	mBox.extendRight(delta);
	mouse.setBBox(mBox);

	// Check if any hotspots hit the mouse:
	CollisionGroup results = this->checkSubsToObj(&mouse);
	auto rs = results.begin();
	while (rs != results.end()) {
		// If there were any hotspots that hit, then return
		// the first one to the caller:
		if ((*rs)->getType() == COLL_GATE_HOTSPOT) {
			return ((gateHotspot*)*rs)->name;
		}

		rs++;
	}

	return "";
}

void guiGate::getHotspotCoords(string hsName, float &x, float &y) {

	if (hotspots.find(hsName) == hotspots.end()) {
		//TODO: Couldn't find hotspot, so give a useful warning.
		return;
	}

	Point hs = hotspots[hsName]->getLocation();
	x = hs.x;
	y = hs.y;
	return;
}

std::string guiGate::getHotspotPal(const std::string &hotspot) {

	Point coords;
	getHotspotCoords(hotspot, coords.x, coords.y);

	//looping looking for another hotspot with the same location
	map<string, Point> hotspotList = getHotspotList();

	for (const auto &possiblePal : hotspotList) {
		if (possiblePal.first != hotspot && possiblePal.second == coords) {
			return possiblePal.first;
		}
	}
	return "";
}

bool guiGate::isVerticalHotspot(string hsName) {
	float x, y;
	getHotspotCoords(hsName, x, y);
	return (min(getBBox().getTop() - y, y - getBBox().getBottom()) < min(getBBox().getRight() - x, x - getBBox().getLeft()));
}

// Run through my connections and update the merges
void guiGate::updateConnectionMerges() {
	auto connWalk = connections.begin();
	while (connWalk != connections.end()) {
		(connWalk->second)->endSegDrag();
		connWalk++;
	}
}

klsBBox guiGate::getModelBBox() {
	return modelBBox;
};

// Get a hotspot from its name.
gateHotspot * guiGate::getHotspot(const std::string &hotspotName) {
	return hotspots[hotspotName];
}

void guiGate::addConnection(string c, guiWire* obj) {
	connections[c] = obj;
}

guiWire* guiGate::getConnection(string hotspot) {
	return connections[hotspot];
}

void guiGate::removeConnection(string c, int &obj) {
	if (connections.find(c) == connections.end()) return;
	obj = connections[c]->getID();
	connections.erase(c);
}

bool guiGate::isConnected(string c) {
	return (connections.find(c) != connections.end());
}

bool guiGate::isSelected() {
	return selected;
}

bool guiGate::isConnectionInput(string idx) {
	return isInput[idx];
}

void guiGate::saveGate(XMLParser* xparse) {
	float x, y;
	this->getGLcoords(x, y);

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
	auto pC = connections.begin();
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
	auto pParams = gparams.begin();
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
	this->saveGateTypeSpecifics(xparse);
	//End of edit***********************

	xparse->closeTag("gate");
}

// Return the map of hotspot names to their coordinates:
map<string, Point> guiGate::getHotspotList() {

	map< string, Point > remappedHS;
	auto hs = hotspots.begin();
	while (hs != hotspots.end()) {
		remappedHS[hs->first] = (hs->second)->getLocation();
		hs++;
	}
	return remappedHS;
}






void guiGate::draw(bool color) {

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

	// Draw the gate:
	glBegin(GL_LINES);
	for (unsigned int i = 0; i < vertices.size(); i++) {
		glVertex2f(vertices[i].x, vertices[i].y);
	}
	glEnd();

	// Reset the stipple parameters:
	if (selected && color) {
		// Reset the line pattern:
		if (!lineStipple) {
			glDisable(GL_LINE_STIPPLE);
		}
		glLineStipple(oldRepeat, oldStipple);
	}
}

void guiGate::doParamsDialog(GUICircuit* gc, wxCommandProcessor* wxcmd) {
	if (wxGetApp().libraries[libName][libGateName].dlgParams.size() == 0) return;
	paramDialog myDialog("Parameters", gc, this, wxcmd);
	myDialog.SetFocus();
	myDialog.ShowModal();
}

void guiGate::setGUIParam(const std::string &paramName, const std::string & value) {

	gparams[paramName] = value;
	if (paramName == "angle") {
		// Update the matrices and bounding box:
		updateConnectionMerges();
		updateBBoxes();
	}
}

string guiGate::getGUIParam(const std::string & paramName) {
	return gparams[paramName];
}

void guiGate::setLogicParam(const std::string & paramName, const std::string & value) {
	lparams[paramName] = value;
}

string guiGate::getLogicParam(const std::string & paramName) {
	return lparams[paramName];
}

Message_SET_GATE_PARAM* guiGate::checkClick(float x, float y) {
	return nullptr;
}

void guiGate::calcBBox(void) {
	modelBBox.reset();

	for (unsigned int i = 0; i < vertices.size(); i++) {
		modelBBox.addPoint(vertices[i]);
	}

	// Recalculate the world-space bbox:
	updateBBoxes();
}

void guiGate::saveGateTypeSpecifics(XMLParser* xparse) { }







// Convert model->world coordinates:
Point guiGate::modelToWorld(Point c) {

	// Perform a matrix-vector multiply to get the point coordinates in world-space:
	float x = c.x * mModel[0] + c.y * mModel[4] + 1.0*mModel[12];
	float y = c.x * mModel[1] + c.y * mModel[5] + 1.0*mModel[13];

	return Point(x, y);
}

// Update the position matrices, and
// update the world-space bounding box and hotspots.
// This is called once whenever the gate's position
// or angle changes.
void guiGate::updateBBoxes(bool noUpdateWires) {
	// Get the translation vars:
	float x, y;
	this->getGLcoords(x, y);

	// Get the angle vars:
	istringstream iss(gparams["angle"]);
	float angle;
	iss >> angle;

	glMatrixMode(GL_MODELVIEW);

	// Set up the forward matrix:
	glLoadIdentity();
	glTranslatef(x, y, 0);
	glRotatef(angle, 0.0, 0.0, 1.0);

	// Modified by Colin 1/16/17 to allow for mirroring of bus ends
	if ((angle == 180 || angle == 270) && this->getGUIType() == "BUSEND") {
		glScalef(1, -1, 1);
	}

	// Read the forward matrix into the member variable:
	glGetDoublev(GL_MODELVIEW_MATRIX, mModel);
	glLoadIdentity();

	// Update all of the hotspots' world coordinates:
	auto hs = hotspots.begin();
	while (hs != hotspots.end()) {
		(hs->second)->worldLocation = modelToWorld((hs->second)->modelLocation);
		(hs->second)->calcBBox();
		hs++;
	}

	// Convert bbox to world-space:
	klsBBox worldBBox;
	worldBBox.addPoint(modelToWorld(modelBBox.getTopLeft()));
	worldBBox.addPoint(modelToWorld(modelBBox.getTopRight()));
	worldBBox.addPoint(modelToWorld(modelBBox.getBottomLeft()));
	worldBBox.addPoint(modelToWorld(modelBBox.getBottomRight()));
	this->setBBox(worldBBox);

	// Update the connected wires' shapes to accomidate the new gate position:
	auto connWalk = connections.begin();
	while (!noUpdateWires && connWalk != connections.end()) {
		(connWalk->second)->updateConnectionPos(this->getID(), connWalk->first);
		connWalk++;
	}
}