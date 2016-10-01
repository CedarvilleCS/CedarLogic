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

#include <float.h>
#include <string>
#include <vector>
#include "../logic/logic_defaults.h"
#include "klsCollisionChecker.h"
#include "gl_defs.h"
#include "guiGate.h"
#include "XMLParser.h"

struct glWireRenderInfo {
	vector< GLPoint2f > vertexPoints;
	vector< GLPoint2f > intersectPoints;
	vector< GLLine2f > lineSegments;
};

float distanceToLine( GLPoint2f p, GLPoint2f l1, GLPoint2f l2 );
bool operator==(const GLPoint2f& p1, const GLPoint2f& p2);
class guiGate;

struct pointCompare : binary_function<GLPoint2f&, GLPoint2f&, bool>
{
public:
	bool operator()(const GLPoint2f& m1, const GLPoint2f& m2) const
	{ return !(m1.x == m2.x && m1.y == m2.y); }
};

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
	void calcBBox( void ) {
		klsBBox newBBox;
		newBBox.addPoint( begin );
		newBBox.addPoint( end );

		if( isHorizontal() ) {
			newBBox.extendTop( WIRE_BBOX_THICKNESS / 2.0 );
			newBBox.extendBottom( WIRE_BBOX_THICKNESS / 2.0 );
		} else {
			newBBox.extendLeft( WIRE_BBOX_THICKNESS / 2.0 );
			newBBox.extendRight( WIRE_BBOX_THICKNESS / 2.0 );
		}

		this->setBBox( newBBox );
	};

	bool isHorizontal( void ) { return !verticalSeg; };
	bool isVertical( void ) { return verticalSeg; };

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
	wireSegment( ) : klsCollisionObject(COLL_WIRE_SEG) {};
	
	// Give the segment initial values - begin and end points, and orientation
	wireSegment( GLPoint2f nB, GLPoint2f nE, bool nisVertical, unsigned long nid ) : klsCollisionObject(COLL_WIRE_SEG), verticalSeg(nisVertical), begin(nB), end(nE), id(nid) {
		calcBBox();
	};
};

class guiWire : public klsCollisionObject {
public:
	guiWire();

	// TJD. 9/26/2016
	// Added destructor to fix memory bug after transition from mingw to windows.
	// The bug showed itself by segfaulting when copying a gate with a wire selected.
	// The problem was that wireSegment-s that are owned by guiWire and destroyed
	// implicitly by its default destructor were being referenced in klsCollisionObject's destructor.
	// There is a call to insertSubObject() that passes pointers to guiWire's wireSegments into the base class.
	// This problem did not show up in mingw because gcc is too lenient about deleted data.
	// gcc leaves recently deleted stuff alone, windows overwrites it immediately with arbitrary data.
	virtual ~guiWire() {
		deleteSubObjects();
		deleteCollisionObject();
	}

	// Connection functions
	//		addConnection: if openMode is true, then no shape is calculated; waiting for setSegmentMap call.
	void addConnection( guiGate* iGate, string connection, bool openMode = false );
	void removeConnection( guiGate* iGate, string connection);
	long numConnections( void ) { return connectPoints.size(); };
	vector < wireConnection > getConnections( void ) { return connectPoints; };	
	// Update the placement of a connection by extending/moving its
	//	segment.  Will set up a mouse coord from the current position
	//	and another one from the new position to pass to updateSegDrag
	void updateConnectionPos( unsigned long gid, string connection );
	
	void draw( bool color = true );
	bool hover( float cx, float cy, float delta );
	long getHoverSegmentID( void ) { return hoverSegmentID; };
	
	bool isWithinBox( float x1, float y1, float x2, float y2 );
	GLPoint2f getCenter( void );
	
	// Moving functions
	void move( GLPoint2f origin, GLPoint2f delta );
	
	// Create the bbox for this wire, based on
	// the bboxes of the wire segments. Also,
	// add the wire segments into the subObjs list:
	void calcBBox( void );

	// Selection functions
	bool isSelected( void ) { return selected; };
	void select( void ) { selected = true; };
	void unselect( void ) { selected = false; };

	// ID functions
	void setID(long nid) { id = nid; };
	unsigned long getID( void ) { return id; };

	// State functions
	void setState(StateType ns) { state = ns; };
	StateType getState( void ) { return state; };
	
	// Give directions for XML tag definition of wire
	void saveWire ( XMLParser* xparse );
	
	// Get the list of pointers to segments that constitute the wire shape
	// Get the mapping of ID to segment that is the wire shape
	//	NOTE: To do anything with the returned map via intersections of segs
	//			you must set the segments' parent map pointer
	map < long, wireSegment > getSegmentMap( void ) { return segMap; };
	// Set the map/tree from which the wire will generate its shape.
	//	Tree must contain valid wireConnection information.
	//	IMPORTANT: Also must set nextSegID to be a valid ID
	void setSegmentMap( map < long, wireSegment > newSegMap) {
		this->deleteSubObjects(); // prevent coll checker pointers from invalidating
		segMap = newSegMap; 
		calcBBox(); 
		headSegment = ((segMap.begin())->first);
		nextSegID = ((segMap.rbegin())->first)+1;
		endSegDrag();
	};
	
	// We need to hold an initial seg map for undo/redo functionality.
	map < long, wireSegment > getOldSegmentMap( void ) { return oldSegMap; };

	// Generates segList, all segments being generated
	//	from the coordinates of the connection point vertices.
	//	As segments are created, they are merged with segments
	//	following the same path, and duplicates are removed.
	void calcShape( void );

	// Manual wire routing functionality
	//	Takes a mouse pointer and finds the segment in question.
	//	The segment is isolated between its nearest intersections
	//	and set as the dragging segment.
	bool startSegDrag( klsCollisionObject* mouse );
	//	The current dragging segment is moved to a new position
	//	while the associated segments are added/modified to keep
	//	our drag segment connected in the tree
	void updateSegDrag( klsCollisionObject* mouse );
	//	The current dragging segment is dropped, clean up
	void endSegDrag( void );
	
	// pretty print
	void printme(string lineBegin = "");
	void debugStatement( int lineBegin, string descriptor );
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
