#include "wireSegment.h"

#include "../MainApp.h"
DECLARE_APP(MainApp)

#define WIRE_BBOX_THICKNESS 0.25

void wireSegment::calcBBox() {
	klsBBox newBBox;
	newBBox.addPoint(begin);
	newBBox.addPoint(end);

	if (isHorizontal()) {
		newBBox.extendTop(WIRE_BBOX_THICKNESS / 2.0);
		newBBox.extendBottom(WIRE_BBOX_THICKNESS / 2.0);
	}
	else {
		newBBox.extendLeft(WIRE_BBOX_THICKNESS / 2.0);
		newBBox.extendRight(WIRE_BBOX_THICKNESS / 2.0);
	}

	this->setBBox(newBBox);
}

bool wireSegment::isHorizontal() const {
	return !verticalSeg;
}

bool wireSegment::isVertical() const {
	return verticalSeg;
}

wireSegment::wireSegment() : klsCollisionObject(COLL_WIRE_SEG) {};

// Give the segment initial values - begin and end points, and orientation
wireSegment::wireSegment(Point nB, Point nE, bool nisVertical, unsigned long nid) :
	klsCollisionObject(COLL_WIRE_SEG), verticalSeg(nisVertical), begin(nB), end(nE), id(nid) {
	calcBBox();
};