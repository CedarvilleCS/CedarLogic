
#pragma once

struct Color {

	// Create the color white.
	Color();

	// Create a color from rgba values.
	Color(float r, float g, float b, float a = 1.0f);

	// Blend this color with another.
	// 'ratio' is the amount of the other color.
	Color interpolate(const Color &other, float ratio) const;

	float r;

	float g;

	float b;

	float a;
};