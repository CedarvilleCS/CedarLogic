


#pragma once
#include "../../klsCollisionChecker.h"
#include <string>
#include "../../gl_defs.h"

class guiGate;

class gateHotspot : public klsCollisionObject {

	friend guiGate;

public:
	gateHotspot();

	gateHotspot(std::string hsName);

	// Create the bbox for this hotspot:
	void calcBBox();

	GLPoint2f getLocation();

	void setBusLines(int _busLines);

	int getBusLines();

	std::string name;

protected:
	int busLines;
	GLPoint2f modelLocation, worldLocation;
};