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
	
	// Wire end/junction points
	glNewList( CEDAR_GLLIST_CONNECTPOINT, GL_COMPILE );
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(0, 0);
 		for (int z=0; z <= 360; z += 360/POINTS_PER_VERTEX)
			glVertex2f(cos(z*DEG2RAD)*wxGetApp().appSettings.wireConnRadius, sin(z*DEG2RAD)*wxGetApp().appSettings.wireConnRadius);
		glEnd();
	glEndList();

	// Pedro Casanova (casanova@ujaen.es) 2020/04-11
	// Cross junction
	glNewList(CEDAR_GLLIST_CROSSPOINT, GL_COMPILE);
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex2f(wxGetApp().appSettings.wireConnRadius, wxGetApp().appSettings.wireConnRadius);
		glVertex2f(-wxGetApp().appSettings.wireConnRadius, -wxGetApp().appSettings.wireConnRadius);
		glVertex2f(-wxGetApp().appSettings.wireConnRadius, wxGetApp().appSettings.wireConnRadius);
		glVertex2f(wxGetApp().appSettings.wireConnRadius, -wxGetApp().appSettings.wireConnRadius);
		glEnd();
		glLineWidth(1);
	glEndList();
	
}
