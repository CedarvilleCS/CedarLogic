
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



// Create an opengl context to be used with wxGLCanvases.
// This should only be called one time.
void createGLContext();

// Get the context created by createGLContext.
// To start drawing to a glCanvas, use context->setCurrent(canvas).
wxGLContext * getGLContext();

// This creates a framebuffer and sets it as active.
// glBegin() still needs to be called.
void startRenderToWxBitmap();

// This creates a wxBitmap and copies data from the framebuffer into it.
// It then sets the framebuffer back to the default.
// glEnd() still needs to be called before this.
wxBitmap finishRenderToWxBitmap();
