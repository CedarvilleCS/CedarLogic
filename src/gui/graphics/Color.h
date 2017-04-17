
#pragma once
#include <iosfwd>
#include <wx/colour.h>

struct Color {

	// Create the color white.
	Color();

	// Create a color from rgba values.
	Color(float r, float g, float b, float a = 1.0f);

	// Blend this color with another.
	// 'ratio' is the amount of the other color.
	Color interpolate(const Color &other, float ratio) const;

	operator wxColour () const;

	friend std::istream & operator >> (std::istream &in, Color &c);

	friend std::ostream & operator << (std::ostream &out, const Color &c);

	float r;

	float g;

	float b;

	float a;
};
