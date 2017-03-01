
#include "Point.h"

Point::Point() : x(0.0f), y(0.0f) { }

Point::Point(float x, float y) : x(x), y(y) { }

Point Point::operator+(const Point &other) const {
	return Point(x + other.x, y + other.y);
}

Point Point::operator-(const Point &other) const {
	return Point(x - other.x, y - other.y);
}

void Point::operator+=(const Point &other) {
	x += other.x;
	y += other.y;
}

void Point::operator-=(const Point &other) {
	x -= other.x;
	y -= other.y;
}

bool Point::operator==(const Point& other) const {
	return (x >= other.x - EqualRange &&
		x <= other.x + EqualRange &&
		y <= other.y + EqualRange &&
		y >= other.y - EqualRange);
}

bool Point::operator!=(const Point &other) const {
	return !(*this == other);
}