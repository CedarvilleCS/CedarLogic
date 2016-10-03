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
#include "../logic/logic_defaults.h" // StateType
#include "klsCollisionChecker.h"

class guiGate;
class XMLParser;

struct glWireRenderInfo {
	vector< GLPoint2f > vertexPoints;
	vector< GLPoint2f > intersectPoints;
	vector< GLLine2f > lineSegments;
};

float distanceToLine( GLPoint2f p, GLPoint2f l1, GLPoint2f l2 );

struct wireConnection {
	guiGate* cGate; // hold a pointer since this object shouldn't know about the gateList
	unsigned long gid; // hold an id that will be useful in copy/paste remapping of ids
	string connection; // know what hotspot i am connected to in the gate
};

// Class wireSegment:
//	All segments are defined by a begin and end point, plus orientation.  Begin always is
//		less than end along the oriented line (bottom to top, left to right)
// 	Vertical/horizontal is a flag in the segment rather than determinate on endpoints
//		i.e. the wire, once declared vertical will always be vertical
class wireSegment : public klsCollisionObject {
public:
	// Create the bbox for this wire segment:
	void calcBBox();

	bool isHorizontal();

	bool isVertical();

	// Hold the orientation of the wire segment.  Once it is initialized,
	//	the orientation does not change.
	bool verticalSeg;

	//Whenever "begin" or "end" are changed, calcBBox() must be called
	// to re-build the bounding box.
	GLPoint2f begin, end;

	// All segments must know their relative position, in this case to the
	//	first segment's (initial vertical bar) begin point
	GLPoint2f diffBegin, diffEnd;
	
	// Keep a list of the connections that are on this segment
	vector < wireConnection > connections;
	
	// Keeps a sorted list of intersections with other segments
	//		For horizontal segments the key is the x value (since all y's are the same)
	//		vice versa for vertical segments, holds the id of the intersected seg
	map < GLfloat, vector < long > > intersects;
	
	// ID for this seg in its parent map
	long id;
	
	// pretty print
	void printme(string lineBegin = "");

	wireSegment();
	
	// Give the segment initial values - begin and end points, and orientation
	wireSegment(GLPoint2f nB, GLPoint2f nE, bool nisVertical, unsigned long nid);
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

	long getHoverSegmentID();

	GLPoint2f getCenter();

	// Moving functions
	void move(GLPoint2f origin, GLPoint2f delta);

	// Create the bbox for this wire, based on
	// the bboxes of the wire segments. Also,
	// add the wire segments into the subObjs list:
	void calcBBox();

	// Selection functions
	bool isSelected(void);

	void select(void);

	void unselect(void);

	// ID functions
	void setID(long nid);

	unsigned long getID(void);

	// State functions
	void setState(StateType ns);

	StateType getState(void);

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
	void calcShape();

	// Manual wire routing functionality
	//	Takes a mouse pointer and finds the segment in question.
	//	The segment is isolated between its nearest intersections
	//	and set as the dragging segment.
	bool startSegDrag(klsCollisionObject* mouse);

	//	The current dragging segment is moved to a new position
	//	while the associated segments are added/modified to keep
	//	our drag segment connected in the tree
	void updateSegDrag(klsCollisionObject* mouse);

	//	The current dragging segment is dropped, clean up
	void endSegDrag(void);

	// pretty print
	void printme(string lineBegin = "");

	void debugStatement(int lineBegin, string descriptor);

private:
	// Take existing segment connections and update their map keys
	bool refreshIntersections(bool removeBadSegs = false);

	// Self-explanatory, see comments in source
	void removeZeroLengthSegments();

	// Take existing segments and merge concurrent segments
	void mergeSegments();
	void generateRenderInfo();

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
	unsigned long id;
	StateType state;
	klsBBox mouseCoords;
	long hoverSegmentID;

	// Handle a pointer for the segment being moved, -1 if not set
	long currentDragSegment;

	glWireRenderInfo renderInfo;
};

#endif /*GUIWIRE_H_*/
