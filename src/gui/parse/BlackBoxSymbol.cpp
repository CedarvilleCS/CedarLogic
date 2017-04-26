
#include "BlackBoxSymbol.h"

Point generateShapeRectangle(InVector &left, InVector &top, InVector &bottom, InVector &right) {

	int widthPins = (top.size() > bottom.size()) ? top.size() : bottom.size();
	int heightPins = (left.size() > right.size()) ? left.size() : right.size();

	unsigned int leftLongest = 0;
	unsigned int rightLongest = 0;
	unsigned int topLongest = 0;
	unsigned int bottomLongest = 0;

	for (InputData i : left) {
		if (i.name.length() > leftLongest) {
			leftLongest = i.name.length();
		}
	}
	for (InputData i : right) {
		if (i.name.length() > rightLongest) {
			rightLongest = i.name.length();
		}
	}
	for (InputData i : top) {
		if (i.name.length() > topLongest) {
			topLongest = i.name.length();
		}
	}
	for (InputData i : bottom) {
		if (i.name.length() > bottomLongest) {
			bottomLongest = i.name.length();
		}
	}
	
	int widthText = leftLongest + rightLongest;
	int heightText = topLongest + bottomLongest;

	float width = (widthText > widthPins) ? widthText : widthPins;
	float height = (heightText > heightPins) ? heightText : heightPins;

	return{width + 2.0f,height + 2.0f};
}

void generateShapePins(Point size, InVector &left, InVector &top, InVector &bottom, InVector &right) {
	float width = size.x;
	float height = size.y;

	// Do left and right pins
	float middleY = 0;
	// Left
	float startLeft = middleY - (left.size() / 2);
	for (int i = 0; i < (int)left.size(); i++) {
		left[i].hotspotTail = Point(-width / 2, startLeft + i);
		left[i].hotspot = Point((-width / 2) - 1, startLeft + i);
	}
	// Right
	float startRight = middleY - (right.size() / 2);
	for (int i = 0; i < (int)right.size(); i++) {
		right[i].hotspotTail = Point(width / 2, startRight + i);
		right[i].hotspot = Point((width / 2) + 1, startRight + i);
	}

	// Do top and bottom pins
	float middleX = 0;
	// Top
	float startTop = middleX - (top.size() / 2);
	for (int i = 0; i < (int)top.size(); i++) {
		top[i].hotspotTail = Point(startTop + i, height / 2);
		top[i].hotspot = Point(startTop + i, (height / 2) + 1);
	}
	// Bottom
	float startBottom = middleX - (bottom.size() / 2);
	for (int i = 0; i < (int)bottom.size(); i++) {
		bottom[i].hotspotTail = Point(startBottom + i, -height / 2);
		bottom[i].hotspot = Point(startBottom + i, (-height / 2) - 1);
	}
}

void generateShapeTextPosition(InVector &left, InVector &top, InVector &bottom, InVector &right) {
	for (InputData &i : left) {
		i.textPosition.x = i.hotspot.x + 2.0;
		i.textPosition.y = i.hotspot.y + 0.5;
	}
	for (InputData &i : right) {
		i.textPosition.x = i.hotspot.x - i.name.length() - 1.0;
		i.textPosition.y = i.hotspot.y + 0.5;
	}
	for (InputData &i : top) {
		i.textPosition.x = i.hotspot.x - 0.5;
		i.textPosition.y = i.hotspot.y - i.name.length() - 1.0;
	}
	for (InputData &i : bottom) {
		i.textPosition.x = i.hotspot.x - 0.5;
		i.textPosition.y = i.hotspot.y + 2.0;
	}
}