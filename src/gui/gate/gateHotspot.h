


#pragma once
#include "../collision/klsCollisionChecker.h"
#include <string>
#include "../graphics/gl_defs.h"

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
	int busLines;
	Point modelLocation, worldLocation;
};