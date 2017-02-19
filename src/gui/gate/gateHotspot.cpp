
#include "gateHotspot.h"

static const GLfloat GATE_HOTSPOT_THICKNESS = 0.05f / 2.0f;

gateHotspot::gateHotspot() : gateHotspot("") { }

gateHotspot::gateHotspot(const std::string &name) :
		klsCollisionObject(COLL_GATE_HOTSPOT),
		name(name),
		modelLocation(0, 0),
		worldLocation(0, 0),
		busLines(1) {

	calcBBox();
}

// Create the bbox for this hotspot:
void gateHotspot::calcBBox() {

	klsBBox newBBox;
	newBBox.addPoint(worldLocation);

	newBBox.extendTop(GATE_HOTSPOT_THICKNESS);
	newBBox.extendBottom(GATE_HOTSPOT_THICKNESS);
	newBBox.extendLeft(GATE_HOTSPOT_THICKNESS);
	newBBox.extendRight(GATE_HOTSPOT_THICKNESS);

	this->setBBox(newBBox);
}

GLPoint2f gateHotspot::getLocation() {
	return worldLocation;
}

void gateHotspot::setBusLines(int busLines) {
	this->busLines = busLines;
}

int gateHotspot::getBusLines() {
	return busLines;
}