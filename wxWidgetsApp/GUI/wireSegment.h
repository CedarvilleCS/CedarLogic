
// This header was pulled out of guiWire.h to keep modifications
// of guiWire from recompiling the whole project.
// Tyler J Drake 9-3-2016

#ifndef WIRESEGMENT_H_
#define WIRESEGMENT_H_

#include <cfloat>
#include <string>
#include <vector>
#include "../logic/logic_values.h" // StateType
#include "klsCollisionChecker.h"

class guiGate;

// This doesn't belong here, but I'm not sure where to put it yet.
// Beware! This implementation is actually in guiWire.cpp!!!
float distanceToLine(GLPoint2f p, GLPoint2f l1, GLPoint2f l2);

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

#endif