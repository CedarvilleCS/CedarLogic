
#pragma once
#include "OpenGL.h"
#include "Color.h"
#include "ColorPalette.h"
#include "Point.h"
#include "LineSegment.h"
#include <wx/image.h>
#include <wx/glcanvas.h>

#define POINTS_PER_VERTEX 12
#define DEG2RAD 0.0174533

#define CEDAR_GLLIST_CONNECTPOINT 1024

void defineGLLists();



bool isGLContextGood();

// Create an opengl context to be used with wxGLCanvases.
// This can be called repeatedly.
void createGLContext(wxGLCanvas &canvas);

void makeGLCanvasCurrent(wxGLCanvas &canvas);

// This creates a framebuffer and sets it as active.
// glBegin() still needs to be called.
void startRenderToWxImage(int width, int height);

// This creates a wxImage and copies data from the framebuffer into it.
// It then sets the framebuffer back to the default.
// glEnd() still needs to be called before this.
wxImage finishRenderToWxImage();
