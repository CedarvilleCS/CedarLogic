
#include "Color.h"
#include <iostream>
#include <iomanip>

Color::Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) { }

Color::Color(float r, float g, float b, float a) :
	r(r), g(g), b(b), a(a) { }

Color Color::interpolate(const Color &other, float ratio) const {

	return Color(
		r + ratio * (other.r - r),
		g + ratio * (other.g - g),
		b + ratio * (other.b - b),
		a + ratio * (other.a - a));
}

Color::operator wxColour () const {
	return wxColour(r * 255, g * 255, b * 255, a * 255);
}

std::istream & operator >> (std::istream &in, Color &c) {
	in >> c.r >> c.g >> c.b >> c.a;
	return in;
}

std::ostream & operator << (std::ostream &out, const Color &c) {
	out << std::setprecision(2) << std::fixed <<
		c.r << ' ' << c.g << ' ' << c.b << ' ' << c.a;
	return out;
}