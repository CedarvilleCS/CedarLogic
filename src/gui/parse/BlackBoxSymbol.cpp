
#include "BlackBoxSymbol.h"

Point generateShapeRectangle(InVector &left, InVector &top, InVector &bottom, InVector &right) {

	int widthPins = (top.size() > bottom.size()) ? top.size() : bottom.size();
	int heightPins = (left.size() > right.size()) ? left.size() : right.size();

	int leftLongest = 0;
	int rightLongest = 0;
	int topLongest = 0;
	int bottomLongest = 0;

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

	return{width + 2.0,height + 2.0};
}

void generateShapePins(Point size, InVector &left, InVector &top, InVector &bottom, InVector &right) {

}

void generateShapeTextPosition(InVector &left, InVector &top, InVector &bottom, InVector &right) {
	for (InputData i : left) {
		i.textPosition.x = i.hotspot.x + i.name.length() + 1.0;
		i.textPosition.y = i.hotspot.y + 0.5;
	}
	for (InputData i : right) {
		i.textPosition.x = i.hotspot.x - i.name.length() - 1.0;
		i.textPosition.y = i.hotspot.y + 0.5;
	}
	for (InputData i : top) {
		i.textPosition.x = i.hotspot.x - 0.5;
		i.textPosition.y = i.hotspot.y - i.name.length() - 1.0;
	}
	for (InputData i : bottom) {
		i.textPosition.x = i.hotspot.x - 0.5;
		i.textPosition.y = i.hotspot.y + i.name.length() + 1.0;
	}
}