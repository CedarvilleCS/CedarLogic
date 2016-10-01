#ifndef GL_DEFS_H_
#define GL_DEFS_H_

#include "gl_wrapper.h"

struct GLPoint2f {
	GLPoint2f( GLfloat newX = 0.0, GLfloat newY = 0.0 ) : x(newX), y(newY) {};
	GLfloat x, y;


	bool operator==(const GLPoint2f& other);
};

struct GLLine2f {
	GLPoint2f begin;
	GLPoint2f end;
};

#define POINTS_PER_VERTEX 12
#define WIRE_BBOX_THICKNESS 0.25
#define DEG2RAD 0.0174533
#define EQUALRANGE 0.00125

#define CEDAR_GLLIST_CONNECTPOINT 1024

void defineGLLists();

#endif /*GL_DEFS_H_*/
