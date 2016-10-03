#include "wireSegment.h"

#include "MainApp.h"
DECLARE_APP(MainApp)

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

bool wireSegment::isHorizontal(void) {
	return !verticalSeg;
}

bool wireSegment::isVertical(void) {
	return verticalSeg;
}

void wireSegment::printme(string lineBegin) {
	wxGetApp().logfile << lineBegin << "Segment " << id << " ";
	if (isVertical()) wxGetApp().logfile << "vertical" << endl;
	else wxGetApp().logfile << "horizontal" << endl;
	wxGetApp().logfile << lineBegin << "\t" << begin.x << "," << begin.y << " to " << end.x << "," << end.y << endl;
	wxGetApp().logfile << lineBegin << "\t\tDIFFS: " << diffBegin.x << "," << diffBegin.y << " and " << diffEnd.x << "," << diffEnd.y << endl;
	for (unsigned int i = 0; i < connections.size(); i++) {
		GLPoint2f connPoint;
		//		connections[i].cGate->getHotspotCoords(connections[i].connection, connPoint.x, connPoint.y);
		wxGetApp().logfile << lineBegin << "\tconnected to " << connections[i].gid << " @ " << connections[i].connection << endl; //" at point " << connPoint.x << "," << connPoint.y << endl;
	}
	map < GLfloat, vector < long > >::iterator iwalk = intersects.begin();
	while (iwalk != intersects.end()) {
		for (unsigned int i = 0; i < (iwalk->second).size(); i++) {
			wxGetApp().logfile << lineBegin << "\tintersection with " << (iwalk->second)[i] << " @ " << iwalk->first << endl;
		}
		if ((iwalk->second).size() == 0) wxGetApp().logfile << "intersection listed but no segments for " << iwalk->first << endl;
		iwalk++;
	}
}

wireSegment::wireSegment() : klsCollisionObject(COLL_WIRE_SEG) {};

// Give the segment initial values - begin and end points, and orientation
wireSegment::wireSegment(GLPoint2f nB, GLPoint2f nE, bool nisVertical, unsigned long nid) : klsCollisionObject(COLL_WIRE_SEG), verticalSeg(nisVertical), begin(nB), end(nE), id(nid) {
	calcBBox();
};