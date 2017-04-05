
#pragma once
#include "../collision/klsCollisionChecker.h"
#include <string>
#include <map>
#include <vector>
#include "gui/graphics/Point.h"
#include "gui/graphics/gl_text.h"

class wxCommandProcessor;
class gateHotspot;
class guiWire;
class Message_SET_GATE_PARAM;
class XMLParser;
class GUICircuit;

class guiGate : public klsCollisionObject {
public:
	guiGate();

	virtual ~guiGate();

	void setID(long nid);

	unsigned long getID();

	void setLibraryName(std::string nLibName, std::string nLibGateName);

	std::string getLibraryName();

	std::string getLibraryGateName();

	std::string getLogicType();

	std::string getGUIType();

	void setGLcoords(float x, float y, bool noUpdateWires = false);

	void getGLcoords(float &x, float &y);

	// Shift the gate by x and y, relative to its current location:
	void translateGLcoords(float x, float y);

	// Make all connections end their drag so segments are merged
	void finalizeWirePlacements();

	std::map < std::string, std::string >* getAllGUIParams();

	std::map < std::string, std::string >* getAllLogicParams();

	void declareInput(std::string name);

	void declareOutput(std::string name);

	// Draw this gate as unselected:
	void unselect();

	void select();

	// Draw this gate as selected from now until unselect() is
	// called, if the coordinate passed to it is within
	// this gate's bounding box in GL coordinates.
	// Return true if this gate is selected.
	bool clickSelect(float x, float y);

	// Insert a line in the line list.
	void insertLine(float x1, float y1, float x2, float y2);

	// Add a label to the gate.
	void insertLabel(const gl_text &label);

	// Insert a hotspot in the hotspot list.
	void insertHotspot(float x1, float y1, std::string connection, int busLines);

	// Check if any of the hotspots of this gate are within the delta
	// of the world coordinates sX and sY. delta is in gl coords.
	std::string checkHotspots(float x, float y, float delta);

	// Return the coordinates of the hotspot in GL world-space coords.
	void getHotspotCoords(std::string hsName, float &x, float &y);

	// Get the name of the hotspot that overlaps the one given.
	// Return "" for no overlap or bad hotspot name.
	std::string getHotspotPal(const std::string &hotspot);

	// Is a particular hotspot aligned to the vertical or horizontal edge?
	bool isVerticalHotspot(std::string hsName);

	// Update all wires connected to me
	// Update the connected wires' shapes to accomidate the new gate position:
	void updateConnectionMerges();

	klsBBox getModelBBox();

	// Get a hotspot from its name.
	gateHotspot * getHotspot(const std::string &hotspotName);

	void addConnection(std::string, guiWire*);

	guiWire* getConnection(std::string hotspot);

	void removeConnection(std::string, int&);

	bool isConnected(std::string);

	bool isSelected();

	bool isConnectionInput(std::string idx);

	void saveGate(XMLParser*);

	// Return the map of hotspot names to their coordinates:
	std::map<std::string, Point> getHotspotList();

	virtual void draw(bool color = true);

	virtual void doParamsDialog(GUICircuit* gc, wxCommandProcessor* wxcmd);

	virtual void setGUIParam(const std::string &paramName, const std::string &value);

	virtual std::string getGUIParam(const std::string &paramName);

	virtual void setLogicParam(const std::string &paramName, const std::string &value);

	virtual std::string getLogicParam(const std::string &paramName);

	virtual Message_SET_GATE_PARAM* checkClick(float x, float y);

	virtual void calcBBox();

	virtual void saveGateTypeSpecifics(XMLParser* xparse);

protected:

	// Convert model->world coordinates:
	Point modelToWorld(Point c);

	// Get a world-space bounding box:
	klsBBox getWorldBBox() { return this->getBBox(); };

	void updateBBoxes(bool noUpdateWires = false);

	std::string libName;

	std::string libGateName;

	double mModel[16];

	long gateID;

	std::string gateType;

	std::string guiGateType;

	float myX, myY;

	bool selected; // Is this gate selected or not?

				   // Model space bounding box:
	klsBBox modelBBox;

	std::vector<Point> vertices;

	std::vector<gl_text> labels;

	// map i/o name to hotspot coord
	std::map< std::string, gateHotspot* > hotspots;

	// map i/o name to wire id
	std::map< std::string, guiWire* > connections;

	// map i/o name to status as input (true = input, false = output)
	std::map< std::string, bool > isInput;

	// map param name to value
	std::map< std::string, std::string > gparams;	// GUI params

	std::map< std::string, std::string > lparams;  // Logic params
};