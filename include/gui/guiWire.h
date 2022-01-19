/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   guiWire: GUI representation of wire objects
*****************************************************************************/

#ifndef GUIWIRE_H_
#define GUIWIRE_H_

#include <cfloat>
#include <string>
#include <vector>
#include "logic_values.h" // StateType
#include "klsCollisionChecker.h"
#include "wireSegment.h"

class guiGate;
class XMLParser;

float distanceToLine(GLPoint2f p, GLPoint2f l1, GLPoint2f l2);

struct glWireRenderInfo {
	vector< GLPoint2f > vertexPoints;
	vector< GLPoint2f > intersectPoints;
	vector< GLLine2f > lineSegments;
};

class guiWire : public klsCollisionObject {
public:
	guiWire();

	virtual ~guiWire();

	// Connection functions
	//		addConnection: if openMode is true, then no shape is calculated; waiting for setSegmentMap call.
	void addConnection(guiGate* iGate, string connection, bool openMode = false);

	void removeConnection(guiGate* iGate, string connection);

	long numConnections();

	vector < wireConnection > getConnections();

	// Update the placement of a connection by extending/moving its
	//	segment.  Will set up a mouse coord from the current position
	//	and another one from the new position to pass to updateSegDrag
	void updateConnectionPos(unsigned long gid, string connection);

	void draw(bool color = true);

	bool hover(float cx, float cy, float delta);

	GLPoint2f getCenter();

	// Moving functions
	void move(GLPoint2f origin, GLPoint2f delta);

	// Create the bbox for this wire, based on
	// the bboxes of the wire segments. Also,
	// add the wire segments into the subObjs list:
	void calcBBox();

	// Selection functions
	bool isSelected();
	void select();
	void unselect();

	// Set the wire's first (and most likely only) Id.
	void setID(IDType wireId);

	// Get the wire's first (and most likely only) Id.
	IDType getID() const;

	// Set the all of the wire's Ids (For buses, really).
	void setIDs(const std::vector<IDType> &ids);

	// Set all of the wire's Ids (For buses, really).
	const std::vector<IDType> & getIDs() const;
	
	// Set the state for each wire id. (again multiple because buses)
	void setState(std::vector<StateType> state);
	
	// Set the state of a busline in this wire from its id.
	void setSubState(IDType buslineId, StateType state);

	// Get the state for each wire id.
	const std::vector<StateType> & getState() const;

	// Give directions for XML tag definition of wire
	void saveWire(XMLParser* xparse);

	// Get the list of pointers to segments that constitute the wire shape
	// Get the mapping of ID to segment that is the wire shape
	//	NOTE: To do anything with the returned map via intersections of segs
	//			you must set the segments' parent map pointer
	map < long, wireSegment > getSegmentMap();

	// Set the map/tree from which the wire will generate its shape.
	//	Tree must contain valid wireConnection information.
	//	IMPORTANT: Also must set nextSegID to be a valid ID
	void setSegmentMap(map < long, wireSegment > newSegMap);

	// We need to hold an initial seg map for undo/redo functionality.
	map < long, wireSegment > getOldSegmentMap();

	// Generates segList, all segments being generated
	//	from the coordinates of the connection point vertices.
	//	As segments are created, they are merged with segments
	//	following the same path, and duplicates are removed.
	void calcShape();  // TODO

	// Manual wire routing functionality
	//	Takes a mouse pointer and finds the segment in question.
	//	The segment is isolated between its nearest intersections
	//	and set as the dragging segment.
	bool startSegDrag(klsCollisionObject* mouse);  // TODO

	//	The current dragging segment is moved to a new position
	//	while the associated segments are added/modified to keep
	//	our drag segment connected in the tree
	void updateSegDrag(klsCollisionObject* mouse);

	//	The current dragging segment is dropped, clean up
	void endSegDrag();

private:
	// Take existing segment connections and update their map keys
	bool refreshIntersections(bool removeBadSegs = false);

	// Self-explanatory, see comments in source
	void removeZeroLengthSegments();  // TODO

	// Take existing segments and merge concurrent segments
	void mergeSegments();
	void generateRenderInfo();  // TODO




	// Store the tree in a non-pointered way for easy copy
	map < long, wireSegment > segMap;
	map < long, wireSegment > oldSegMap;
	long nextSegID;

	// What is the whole wire connected to?
	vector< wireConnection > connectPoints;

	// Instance vars
	bool selected;
	bool setVerticalBar;
	long headSegment; // reference segment
	
	// The wire ids for each wire in the bus.
	// These same ids are used in the gui and logic systems.
	// There are maps of id to guiWire in GUICircuit and GUICanvas.
	// In these maps, the first bus id is used to look up the wire.
	// The other ids all point to null.
	std::vector<IDType> ids;

	// The state for each wire in the bus.
	// Most wires will have a vector of size 1.
	std::vector<StateType> state;
	
	klsBBox mouseCoords;

	// Handle a pointer for the segment being moved, -1 if not set
	long currentDragSegment;

	glWireRenderInfo renderInfo;
};

#endif /*GUIWIRE_H_*/
