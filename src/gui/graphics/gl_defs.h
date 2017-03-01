#ifndef GL_DEFS_H_
#define GL_DEFS_H_

#include "gl_wrapper.h"
#include "Point.h"

struct GLLine2f {
	Point begin;
	Point end;
};

#define POINTS_PER_VERTEX 12
#define WIRE_BBOX_THICKNESS 0.25
#define DEG2RAD 0.0174533

#define CEDAR_GLLIST_CONNECTPOINT 1024

void defineGLLists();

#endif /*GL_DEFS_H_*/
