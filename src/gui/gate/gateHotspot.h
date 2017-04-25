


#pragma once
#include "gui/collision/klsCollisionChecker.h"
#include <string>
#include "gui/graphics/Point.h"

class guiGate;

class gateHotspot : public klsCollisionObject {

	friend guiGate;

public:
	gateHotspot();

	gateHotspot(const std::string &name);

	// Create the bbox for this hotspot:
	void calcBBox();

	Point getLocation();

	void setBusLines(int busLines);

	int getBusLines();

	std::string name;

protected:
	Point modelLocation;
	Point worldLocation;
	int busLines;
};
