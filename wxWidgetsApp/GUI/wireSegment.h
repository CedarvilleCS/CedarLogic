
// This header was pulled out of guiWire.h to keep modifications
// of guiWire from recompiling the whole project.
// Tyler J Drake 9-3-2016

#ifndef WIRE_SEGMENT_H
#define WIRE_SEGMENT_H

#include <cfloat>
#include <string>
#include <vector>
#include "../logic/logic_values.h" // StateType
#include "klsCollisionChecker.h"

class guiGate;

struct wireConnection {
	guiGate* cGate; // hold a pointer since this object shouldn't know about the gateList
	unsigned long gid; // hold an id that will be useful in copy/paste remapping of ids
	string connection; // know what hotspot i am connected to in the gate
};

// guiWire's are made of wireSegments.
// Segments are vertical or horizontal.
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

	// Endpoints of the segment.
	// Begin is always less than end.
	GLPoint2f begin;
	GLPoint2f end;

	// All segments must know their relative position, in this case to the
	//	first segment's (initial vertical bar) begin point
	GLPoint2f diffBegin;
	GLPoint2f diffEnd;

	// Keep a list of the connections that are on this segment
	vector < wireConnection > connections;

	// Keeps a sorted list of intersections with other segments
	//		For horizontal segments the key is the x value (since all y's are the same)
	//		vice versa for vertical segments, holds the id of the intersected seg
	map < GLfloat, vector < long > > intersects;

	// ID for this seg in its parent map
	long id;

	wireSegment();

	// Give the segment initial values - begin and end points, and orientation
	wireSegment(GLPoint2f nB, GLPoint2f nE, bool nisVertical, unsigned long nid);
};

#endif