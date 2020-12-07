#include <cmath>
#include "gl_defs.h"

#include "MainApp.h"
DECLARE_APP(MainApp)


GLPoint2f GLPoint2f::operator+(const GLPoint2f &other) const {
	return GLPoint2f(x + other.x, y + other.y);
}

GLPoint2f GLPoint2f::operator-(const GLPoint2f &other) const {
	return GLPoint2f(x - other.x, y - other.y);
}

void GLPoint2f::operator+=(const GLPoint2f &other) {
	x += other.x;
	y += other.y;
}

void GLPoint2f::operator-=(const GLPoint2f &other) {
	x -= other.x;
	y -= other.y;
}

bool GLPoint2f::operator==(const GLPoint2f& other) const {
	return (x >= other.x - EQUALRANGE &&
		x <= other.x + EQUALRANGE &&
		y <= other.y + EQUALRANGE &&
		y >= other.y - EQUALRANGE);
}

bool GLPoint2f::operator!=(const GLPoint2f &other) const {
	return !(*this == other);
}

void defineGLLists() {
	float degInRad = 0;
	
	// Wire end/junction points
	glNewList( CEDAR_GLLIST_CONNECTPOINT, GL_COMPILE );
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(0, 0);
 		for (int z=0; z <= 360; z += 360/POINTS_PER_VERTEX)
		{
			degInRad = z*DEG2RAD;
			glVertex2f(cos(degInRad)*wxGetApp().appSettings.wireConnRadius, sin(degInRad)*wxGetApp().appSettings.wireConnRadius);
		}
		glEnd();
	glEndList();
	
}
