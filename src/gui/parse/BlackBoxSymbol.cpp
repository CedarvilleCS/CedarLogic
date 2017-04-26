
#include "BlackBoxSymbol.h"

Point generateShapeRectangle(InVector &left, InVector &top, InVector &bottom, InVector &right) {
	return{};
}

void generateShapePins(Point size, InVector &left, InVector &top, InVector &bottom, InVector &right) {
	float width = size.x;
	float height = size.y;

	// Do left and right pins
	float middleY = height / 2;
	// Left
	float startLeft = middleY - (left.size() / 2);
	for (int i = 0; i < left.size(); i++) {
		left[i].hotspotTail = Point(-width / 2, startLeft + i);
		left[i].hotspot = Point((-width / 2) - 1, startLeft + i);
	}
	// Right
	float startRight = middleY - (right.size() / 2);
	for (int i = 0; i < right.size(); i++) {
		right[i].hotspotTail = Point(width / 2, startRight + i);
		right[i].hotspot = Point((width / 2) + 1, startRight + i);
	}

	// Do top and bottom pins
	float middleX = width / 2;
	// Top
	float startTop = middleX - (top.size() / 2);
	for (int i = 0; i < top.size(); i++) {
		top[i].hotspotTail = Point(startTop + i, height / 2);
		top[i].hotspot = Point(startTop + i, (height / 2) + 1);
	}
	// Bottom
	float startBottom = middleX - (bottom.size() / 2);
	for (int i = 0; i < right.size(); i++) {
		bottom[i].hotspotTail = Point(startBottom + i, -height / 2);
		bottom[i].hotspot = Point(startBottom + i, (-height / 2) - 1);
	}
}

void generateShapeTextPosition(InVector &left, InVector &top, InVector &bottom, InVector &right) {

}