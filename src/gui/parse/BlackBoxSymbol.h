#pragma once

#include <vector>
#include <string>
#include "gui/graphics/gl_defs.h"
using namespace std;

struct InputData {

	// Connection point.
	Point hotspot;

	// Second part of hotspot line (connected to rest of gate).
	Point hotspotTail;

	// Name of input.
	std::string name;

	// Rotation of text.
	float rotation;

	// Top-left corner of text (before rotation).
	Point textPosition;

	// Position of junction in schematic.
	Point originalPosition;

	bool operator<(const InputData &other) const {

		if (rotation == 0.0) {
			return originalPosition.y < other.originalPosition.y;
		}
		else {
			return originalPosition.x < other.originalPosition.x;
		}
	}
};

using InVector = std::vector<InputData>;

// Get rect size that fits all pins.
Point generateShapeRectangle(InVector &left, InVector &top, InVector &bottom, InVector &right);

// Set hotspot and hotspotTail for every input.
void generateShapePins(Point size, InVector &left, InVector &top, InVector &bottom, InVector &right);

// Set textPosition for every input.
void generateShapeTextPosition(InVector &left, InVector &top, InVector &bottom, InVector &right);