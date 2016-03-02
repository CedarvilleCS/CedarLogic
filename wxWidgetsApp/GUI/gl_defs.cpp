#include <cmath>
#include "gl_defs.h"

#include "MainApp.h"
DECLARE_APP(MainApp)

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
