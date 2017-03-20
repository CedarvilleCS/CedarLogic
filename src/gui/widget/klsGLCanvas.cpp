/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
					 Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.

   klsGLCanvas: Generic implementation of OpenGL canvas
*****************************************************************************/

#include "klsGLCanvas.h"
#include "../MainApp.h"
#include "../dialog/paramDialog.h"
#include "../GLFont/glfont2.h"
#include "../graphics/gl_text.h"
#include "klsMiniMap.h"

// Included to use the min() and max() templates:
#include <algorithm>
using namespace std;

// Activate this #define statement if you want to
// show the mouse handling arrows:
//#define CANVAS_DEBUG_TESTS_ON


DECLARE_APP(MainApp)

BEGIN_EVENT_TABLE(klsGLCanvas, wxGLCanvas)
EVT_PAINT(klsGLCanvas::wxOnPaint)
EVT_SIZE(klsGLCanvas::wxOnSize)
EVT_ERASE_BACKGROUND(klsGLCanvas::wxOnEraseBackground)

EVT_MOUSEWHEEL(klsGLCanvas::wxOnMouseWheel)
EVT_MOUSE_EVENTS(klsGLCanvas::wxOnMouseEvent)

EVT_KEY_DOWN(klsGLCanvas::wxKeyDown)
EVT_KEY_UP(klsGLCanvas::wxKeyUp)

EVT_TIMER(SCROLL_TIMER_ID, klsGLCanvas::OnScrollTimer)
END_EVENT_TABLE()


klsGLCanvas::klsGLCanvas(wxWindow *parent, const wxString& name, wxWindowID id,
	const wxPoint& pos, const wxSize& size, long style) :
	wxGLCanvas(parent, id, nullptr, pos, size, style | wxFULL_REPAINT_ON_RESIZE | wxWANTS_CHARS, name) {

	glcontext = new wxGLContext(this);

	// Zoom and OpenGL coordinate of upper-left corner of this canvas:
	viewZoom = DEFAULT_ZOOM;
	panX = panY = 0.0;

	autoScrollEnable();

	// The mouse is in the window:
	mouseOutOfWindow = false;

	// Mouse wheel rotation tracking variable:
	wheelRotation = 0.0;

	// Set the mouse coords memory:
	setMouseCoords(GLPoint2f(0.0, 0.0));
	setMouseScreenCoords(wxPoint(0, 0));

	setIsDragging(false, BUTTON_LEFT);
	setDragStartCoords(GLPoint2f(0.0, 0.0), BUTTON_LEFT);
	setDragEndCoords(GLPoint2f(0.0, 0.0), BUTTON_LEFT);

	setIsDragging(false, BUTTON_MIDDLE);
	setDragStartCoords(GLPoint2f(0.0, 0.0), BUTTON_MIDDLE);
	setDragEndCoords(GLPoint2f(0.0, 0.0), BUTTON_MIDDLE);

	setIsDragging(false, BUTTON_RIGHT);
	setDragStartCoords(GLPoint2f(0.0, 0.0), BUTTON_RIGHT);
	setDragEndCoords(GLPoint2f(0.0, 0.0), BUTTON_RIGHT);

	// Set up scrolling timer:
	scrollTimer = new wxTimer(this, SCROLL_TIMER_ID);
	scrollTimer->Stop();

	setHorizGrid(1);
	disableHorizGrid();

	setVertGrid(1);
	disableVertGrid();

	glInitialized = false;

	minimap = NULL;

	canvasLocked = false;
}


klsGLCanvas::~klsGLCanvas() {
	scrollTimer->Stop();
	delete scrollTimer;
	return;
}

void klsGLCanvas::updateMiniMap() {
	GLPoint2f p1, p2;
	getViewport(p1, p2);
	if (minimap != NULL) minimap->update(p1, p2);
}

// Print the canvas contents to a bitmap:
wxImage klsGLCanvas::renderToImage(unsigned long width, unsigned long height, unsigned long colorDepth, bool noColor) {
	//WARNING!!! Heavily platform-dependent code ahead! This only works in MS Windows because of the
	// DIB Section OpenGL rendering.

		// Create a DIB section.
		// (The Windows wxBitmap implementation will create a DIB section for a bitmap if you set
		// a color depth of 24 or greater.)
	wxBitmap theBM(width, height, colorDepth);

	// Get a memory hardware device context for writing to the bitmap DIB Section:
	wxMemoryDC myDC;
	myDC.SelectObject(theBM);
	WXHDC theHDC = myDC.GetHDC();

	// The basics of setting up OpenGL to render to the bitmap are found at:
	// http://www.nullterminator.net/opengl32.html
	// http://www.codeguru.com/cpp/g-m/opengl/article.php/c5587/

	PIXELFORMATDESCRIPTOR pfd;
	int iFormat;

	// set the pixel format for the DC
	::ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_SUPPORT_GDI;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = colorDepth;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	iFormat = ::ChoosePixelFormat((HDC)theHDC, &pfd);
	::SetPixelFormat((HDC)theHDC, iFormat, &pfd);

	// create and enable the render context (RC)
	HGLRC hRC = ::wglCreateContext((HDC)theHDC);
	::wglMakeCurrent((HDC)theHDC, hRC);

	// Setup the viewport for rendering:
	reclaimViewport();
	// Reset the glViewport to the size of the bitmap:
	glViewport(0, 0, (GLint)width, (GLint)height);


	ColorPalette::setClearColor(ColorPalette::SchematicBackground);
	ColorPalette::setColor(ColorPalette::GateShape);

	// Load the font texture
	gl_text::loadFont(wxGetApp().appSettings.textFontFile);


	//TODO: Check if alpha is hardware supported, and
	// don't enable it if not!
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	//*********************************
	//Edit by Joshua Lansford 4/05/07
	//I placed this in here to hopefully
	//anti-alis the the text font.
	//however, it doesn't but it does
	//anti-alies the gates which looks nice.
	glEnable(GL_LINE_SMOOTH);
	//End of edit


	// Do the rendering here.
	klsGLCanvasRender(noColor);

	// Flush the OpenGL buffer to make sure the rendering has happened:	
	glFlush();

	// Destroy the OpenGL rendering context, release the memDC, and
	// convert the DIB Section into a wxImage to return to the caller:
	::wglMakeCurrent(NULL, NULL);
	::wglDeleteContext(hRC);
	myDC.SelectObject(wxNullBitmap);

	// Set the OpenGL context back to the klsGLCanvas' context, rather
	// than NULL. (Gates depend on having an OpenGL context live in order
	// to do their translation matrix setup.):
	SetCurrent(*glcontext);

	return theBM.ConvertToImage();
}



// Setup the GL matrices for this canvas:
// (This needs to be called everytime that the matrices will be used.)
void klsGLCanvas::reclaimViewport(void) {

	// Set the projection matrix:	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	wxSize sz = GetClientSize();
	// gluOrtho2D(left, right, bottom, top); (In world-space coords.)
	gluOrtho2D(panX, panX + (sz.GetWidth() * viewZoom), panY - (sz.GetHeight() * viewZoom), panY);
	glViewport(0, 0, (GLint)sz.GetWidth(), (GLint)sz.GetHeight());

	// Set the model matrix:
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


// Set the viewport (Set the left/top and right/bottom coordinates).
// NOTE: It will enforce a 1:1 aspect ratio, but it will make the best
// attempt to fit the zoom box as close as possible. Basically, it will
// fit the longest side to the window, and center the rest.
void klsGLCanvas::setViewport(GLPoint2f topLeft, GLPoint2f bottomRight) {
	wxSize sz = GetClientSize();
	double sAspect = (double)sz.GetHeight() / (double)sz.GetWidth();

	double newWidth = bottomRight.x - topLeft.x;
	double newHeight = topLeft.y - bottomRight.y;
	double aspect = newHeight / newWidth;

	bool useWidth = aspect < sAspect; // Use the width as the limiting factor.

	double newZoom = 1.0;
	GLPoint2f newPan;

	if (useWidth) {
		// The box width determines the new zoom factor:
		newZoom = newWidth / sz.GetWidth();

		// The x coordinate is the edge of the box:
		newPan.x = topLeft.x;

		// The y coordinate must center the box:
		newPan.y = topLeft.y + 0.5 * (sz.GetHeight() * newZoom - newHeight); // y + (1/2 of the leftover margins)
	}
	else {
		// The box height determines the new zoom factor:
		newZoom = newHeight / sz.GetHeight();

		// The y coordinate is the edge of the box:
		newPan.y = topLeft.y;

		// The x coordinate must center the box:
		newPan.x = topLeft.x - 0.5 * (sz.GetWidth() * newZoom - newWidth); // x - (1/2 of the leftover margins)
	}

	// Set the new viewport:
	setZoom(newZoom);
	setPan(newPan.x, newPan.y);
}


void klsGLCanvas::getViewport(GLPoint2f& p1, GLPoint2f& p2) {
	wxSize sz = GetClientSize();
	p1.x = panX;
	p1.y = panY;
	p2.x = panX + (sz.GetWidth()*viewZoom);
	p2.y = panY - (sz.GetHeight()*viewZoom);
}

void klsGLCanvas::klsGLCanvasRender(bool noColor) {
	int w, h;
	GetClientSize(&w, &h);

	ColorPalette::setClearColor(ColorPalette::SchematicBackground);

	//clear window
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Render the background grid:
	if ((horizOn || vertOn) && wxGetApp().appSettings.gridlineVisible) {
		// Note: since adding a very few line primitives is a small price to pay,
		//	we not only draw the grid for the visible area, but also (PAN_STEP*viewZoom)
		//	around the visible area.  This is so when the user pans, there will be no
		//	flicker at the edge of the grid.

		Color oldColor = ColorPalette::getColor();

		GLdouble vW = panX;
		GLdouble vE = panX + (w * viewZoom);
		GLdouble vS = panY - (h * viewZoom);
		GLdouble vN = panY;

		if (horizOn) {
			long gridSpacing = max((long)(horizSpacing + 0.5), (long)1); // Limit the grid spacing options to integer values!
			long glSpacing = max((long)gridSpacing, (long)(MIN_GRID_SCREEN_SPACING * viewZoom));

			long firstX = (long)(glSpacing * floor((vW - (PAN_STEP*viewZoom)) / (float)glSpacing + 0.5));
			
			ColorPalette::setColor(ColorPalette::SchematicGrid);
			glBegin(GL_LINES);
			for (long x = firstX; x < vE + (PAN_STEP*viewZoom); x += glSpacing) {
				glVertex2f(x, vS - (PAN_STEP*viewZoom));
				glVertex2f(x, vN + (PAN_STEP*viewZoom));
			}
			glEnd();
		}

		if (vertOn) {
			long gridSpacing = max((long)(vertSpacing + 0.5), (long)1); // Limit the grid spacing options to integer values!
			long glSpacing = max((long)gridSpacing, (long)(MIN_GRID_SCREEN_SPACING * viewZoom));

			long firstY = (long)(glSpacing * floor((vS - (PAN_STEP*viewZoom)) / (float)glSpacing + 0.5));
			
			ColorPalette::setColor(ColorPalette::SchematicGrid);
			glBegin(GL_LINES);
			for (long y = firstY; y < vN + (PAN_STEP*viewZoom); y += glSpacing) {
				glVertex2f(vW - (PAN_STEP*viewZoom), y);
				glVertex2f(vE + (PAN_STEP*viewZoom), y);
			}
			glEnd();
		}

		// Set the color back to the old color:
		ColorPalette::setColor(oldColor);
	}

	// Call subclassed Render():
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	OnRender(noColor);
}


void klsGLCanvas::wxOnPaint(wxPaintEvent& event) {
	wxPaintDC dc(this);

	SetCurrent(*glcontext);
	// Init OpenGL once, but after SetCurrent
	if (!glInitialized)
	{

		ColorPalette::setClearColor(ColorPalette::SchematicBackground);
		ColorPalette::setColor(ColorPalette::GateShape);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		//TODO: Check if alpha is hardware supported, and
		// don't enable it if not!
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		//*********************************
		//Edit by Joshua Lansford 4/05/07
		//I placed this in here to hopefully
		//anti-alis the the text font.
		//however, it doesn't but it does
		//anti-alies the gates which looks nice.
		//glEnable( GL_LINE_SMOOTH );
		//End of edit

		// Load the font texture
		gl_text::loadFont(wxGetApp().appSettings.textFontFile);

		// Connection point list
		defineGLLists();

		glInitialized = true;
	}

	reclaimViewport();
	klsGLCanvasRender();

	// Show the new buffer:
	glFlush();
	SwapBuffers();
}


void klsGLCanvas::wxOnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
	// We're using double-buffering, so do nothing, to avoid flashing.
}


void klsGLCanvas::wxOnSize(wxSizeEvent& event)
{
	SetCurrent(*glcontext);
	Refresh();
}


void klsGLCanvas::getPan(GLdouble &x, GLdouble &y) {
	x = this->panX;
	y = this->panY;
}


void klsGLCanvas::setPan(GLdouble newX, GLdouble newY) {
	// Clamp the panning ranges:
	newX = max(newX, MIN_PAN);
	newX = min(newX, MAX_PAN);

	newY = max(newY, MIN_PAN);
	newY = min(newY, MAX_PAN);

	// Set the new pan values:
	panX = newX;
	panY = newY;

	// Reset the mouse coordinates to the new pan settings, and
	// call OnMouseMove() because the mouse's gl coords have changed:
	setMouseCoords();
	GLPoint2f m = getMouseCoords();
	OnMouseMove(m.x, m.y, isShiftDown, isControlDown);
	updateMiniMap();

	Refresh(); // Obviously it needs refreshed after a pan.
}


//Julian: Added to assist in zoom to mouse
void klsGLCanvas::setCenter(GLdouble newX, GLdouble newY)
{
	GLPoint2f topLeft;
	GLPoint2f bottomRight;
	GLPoint2f center;

	getViewport(topLeft, bottomRight);
	center = getCenter();

	setPan(newX - (center.x - topLeft.x), newY - (center.y - topLeft.y));
}

void klsGLCanvas::translatePan(GLdouble relX, GLdouble relY) {
	GLdouble x, y;
	getPan(x, y);
	setPan(x + relX, y + relY);
}


void klsGLCanvas::OnScrollTimer(wxTimerEvent& event) {
	wxSize sz = GetClientSize();
	bool mouseOnBorder = false;
	bool isDuringDrag = isDragging(BUTTON_LEFT);

	//TODO: Make this work so that you can auto-scroll even on the edge of a
	// maximized window!
	wxPoint mPos = getMouseScreenCoords();
	if ((mPos.x == 0) || (mPos.x == sz.GetWidth())) {
		mouseOnBorder = true;
	}

	if ((mPos.y == 0) || (mPos.y == sz.GetHeight())) {
		mouseOnBorder = true;
	}

	//TODO: Find a way to disable auto-scroll when a new gate is being dragged around.
	if (isDuringDrag && (mouseOnBorder || mouseOutOfWindow)) {
		GLdouble transX = 0.0;
		GLdouble transY = 0.0;
		if (mPos.x <= 0) {
			transX = -SCROLL_STEP * getZoom();
		}
		else if (mPos.x >= sz.GetWidth()) {
			transX = +SCROLL_STEP * getZoom();
		}

		if (mPos.y <= 0) {
			transY = +SCROLL_STEP * getZoom();
		}
		else if (mPos.y >= sz.GetHeight()) {
			transY = -SCROLL_STEP * getZoom();
		}

		// Call this once, to avoid the mouse callback being run twice:
		translatePan(transX, transY);
	}
}


void klsGLCanvas::setZoom(GLdouble newZoom) {
	// Clamp the newZoom factor within the allowed zoom
	// sizes:
	newZoom = max(newZoom, MIN_ZOOM);
	newZoom = min(newZoom, MAX_ZOOM);

	GLPoint2f center = getCenter();
	GLPoint2f topLeft;
	GLPoint2f bottomRight;
	getViewport(topLeft, bottomRight);

	GLPoint2f oldDist = center - topLeft;
	GLPoint2f newDist = oldDist;

	oldDist.x *= newZoom / viewZoom;
	oldDist.y *= newZoom / viewZoom;

	viewZoom = newZoom;

	translatePan(newDist.x - oldDist.x, newDist.y - oldDist.y);
}


void klsGLCanvas::wxOnMouseEvent(wxMouseEvent& event) {
	reclaimViewport();

	isShiftDown = event.ShiftDown();
	isControlDown = event.ControlDown();

	// Always set the mouse coords to the current event:
	setMouseScreenCoords(event.GetPosition());
	setMouseCoords();

	// Check all of the button events:
	if (event.LeftDown()) {
		mouseOutOfWindow = false; // Assume that we clicked inside the window!
		beginDrag(BUTTON_LEFT);
		OnMouseDown(event); // Call the event handler.
	}
	else if (event.LeftUp() || event.LeftDClick()) {
		endDrag(BUTTON_LEFT);
		OnMouseUp(event);
	}
	else if (event.RightDown() || event.RightDClick()) {
		beginDrag(BUTTON_RIGHT);
		OnMouseDown(event); // Call the event handler.
	}
	else if (event.RightUp()) {
		endDrag(BUTTON_RIGHT);
		OnMouseUp(event);
	}
	else if (event.MiddleDown() || event.MiddleDClick()) {
		beginDrag(BUTTON_MIDDLE);
		OnMouseDown(event); // Call the event handler.
		if (event.MiddleDClick()) {
			// Debugging screen shot code.
			// I left it in because it was nifty to have around, especially
			// for writing documentation.	
			// Render the canvas to a bitmap and save it:
			wxSize sz = GetClientSize();
			wxImage screenShot = renderToImage(sz.GetWidth(), sz.GetHeight());
			wxBitmap myBMP(screenShot);
			myBMP.SaveFile("screen_shot.bmp", wxBITMAP_TYPE_BMP);
		}

	}
	else if (event.MiddleUp()) {
		endDrag(BUTTON_MIDDLE);
		OnMouseUp(event);
	}
	else {
		// It's not a button event, so check the others:
		if (event.Entering()) {
			mouseOutOfWindow = false;
			scrollTimer->Stop();
			OnMouseEnter(event);
		}
		else if (event.Leaving() && !isDragging(BUTTON_MIDDLE)) { // Don't allow auto-scroll during pan-scrolling.
		 // Flag the scroll event by telling it that the
		 // mouse has left the window:
			mouseOutOfWindow = true;

			// Start the scroll timer:
			if (isAutoScrollOn() && isDragging(BUTTON_LEFT)) {
				scrollTimer->Start(SCROLL_TIMER_RATE);
			}

			// Call the event handler:
			OnMouseLeave(event);
		}
		else {
			// Handle the drag-pan event here if needed:
			if (isDragging(BUTTON_MIDDLE)) {
				GLPoint2f mouseDelta(getMouseCoords().x - getDragStartCoords(BUTTON_MIDDLE).x,
					getMouseCoords().y - getDragStartCoords(BUTTON_MIDDLE).y);

				translatePan(-mouseDelta.x, -mouseDelta.y);
			}

			// It's nothing else, so it must be a mouse motion event:
			GLPoint2f m = getMouseCoords();
			OnMouseMove(m.x, m.y, event.ShiftDown(), event.ControlDown());
		}

	}

	// Refresh the canvas to show the mouse drag highlights if needed:
#ifdef CANVAS_DEBUG_TESTS_ON
	Refresh();
#endif

}


void klsGLCanvas::wxOnMouseWheel(wxMouseEvent& event) {
	reclaimViewport();

	// Accumulate mouse wheel events until they amount
	// to one "line", and then take them line at a time:
	wheelRotation += event.GetWheelRotation();
	int rotationLines = (int)wheelRotation / event.GetWheelDelta();
	wheelRotation -= rotationLines * event.GetWheelDelta();


	if (rotationLines != 0) {
		OnMouseWheel(rotationLines / abs(rotationLines));
	}

	// Update the drag-pan event here if needed:
	if (isDragging(BUTTON_MIDDLE)) {
		GLPoint2f mouseDelta(getMouseCoords().x - getDragStartCoords(BUTTON_MIDDLE).x,
			getMouseCoords().y - getDragStartCoords(BUTTON_MIDDLE).y);

		translatePan(-mouseDelta.x, -mouseDelta.y);
	}

	updateMiniMap();
	event.Skip(); // Send the event on to wxOnMouseEvent, so that the gl coordinates get updated.
}


// Start a drag event right away, using the current mouse coordinates.
// This captures the mouse using CaptureMouse() and sets the "Drag Start Coords"
// to the current mouse coordinates.
// (This is usually called by this class right before OnMouseDown(), but
// can be called by the subclasses. For example, right after an OnMouseEnter()
// in which a gate is being dragged. Or, maybe also for a Paste from clipboard event.)
void klsGLCanvas::beginDrag(mouseButton whichButton) {
	// If we are already in a drag event for this button, ignore any additional
	// ones that come along. This allows a beginDrag() called from
	// an event handler to not CaptureMouse() too many times.
	if (isDragging(whichButton)) return;

	// Set the keyboard focus to this window. This allows the user to re-set
	// the keyboard focus to this window by clicking on it.
	SetFocus();

	// Bind all mouse events to this window:
	CaptureMouse();

	// Set the dragging start coordinates:
	setDragStartCoords(getMouseCoords(), whichButton);

	// Set the flag to tell us that the button is dragging:
	setIsDragging(true, whichButton);
}


// Force the drag event to end, by unclaiming the mouse (If all other buttons haven't
// claimed a drag event too) and setting the "Drag End Coords".
void klsGLCanvas::endDrag(mouseButton whichButton) {
	// Set the dragging start coordinates:
	setDragEndCoords(getMouseCoords(), whichButton);

	// Set the flag to tell us that the button is finished dragging:
	setIsDragging(false, whichButton);

	// Release the mouse capture.
	// wxWidgets has an assertion for when the mouse is released too many times.
	// This function is called for double clicking and ESC presses (among other reasons).
	// In both of those situations ReleaseMouse is called without CaptureMouse.
	if (HasCapture()) {
		ReleaseMouse();
	}
}


void klsGLCanvas::wxKeyDown(wxKeyEvent& event) {
	reclaimViewport();

	// Give the subclassed handler first dibs on the event:
	OnKeyDown(event);

	// If the subclassed handler took the event, then don't handle it:
	if (event.GetSkipped()) return;

	switch (event.GetKeyCode()) {
	case WXK_LEFT:
	case WXK_NUMPAD_LEFT:
		translatePan(-PAN_STEP * getZoom(), 0.0);
		break;
	case WXK_RIGHT:
	case WXK_NUMPAD_RIGHT:
		translatePan(+PAN_STEP * getZoom(), 0.0);
		break;
	case WXK_UP:
	case WXK_NUMPAD_UP:
		translatePan(0.0, PAN_STEP * getZoom());
		break;
	case WXK_DOWN:
	case WXK_NUMPAD_DOWN:
		translatePan(0.0, -PAN_STEP * getZoom());
		break;
	case 43: // + key on top row (Works for both '+' and '=')
		//if (!shiftKeyOn) break;
	case WXK_NUMPAD_ADD:
		setZoom(getZoom() * ZOOM_STEP);
		break;
	case 45: // - key on top row (Works for both '-' and '_')
	case WXK_NUMPAD_SUBTRACT:
		setZoom(getZoom() / ZOOM_STEP);
		break;
	default:
		event.Skip();
		break;
	}

	updateMiniMap();
}

void klsGLCanvas::wxKeyUp(wxKeyEvent& event) {
	reclaimViewport();

	OnKeyUp(event);
}

//Julian: Moved implementation from header
void klsGLCanvas::OnMouseWheel(long numOfLines) {
	zoomToMouse(numOfLines);
}

GLPoint2f klsGLCanvas::getSnappedPoint(GLPoint2f c) {
	GLfloat x = horizSpacing * floor(c.x / horizSpacing + 0.5);
	GLfloat y = vertSpacing * floor(c.y / vertSpacing + 0.5);
	return GLPoint2f(x, y);
}

void klsGLCanvas::setHorizGrid(GLfloat hSpacing) {
	horizOn = true;
	if (hSpacing != 0.0) horizSpacing = hSpacing;
}

void klsGLCanvas::disableHorizGrid() {
	horizOn = false;
}

void klsGLCanvas::setVertGrid(GLfloat vSpacing) {
	vertOn = true;
	if (vSpacing != 0.0) vertSpacing = vSpacing;
}

void klsGLCanvas::disableVertGrid() {
	vertOn = false;
}

void klsGLCanvas::setMouseCoords() {
	int w, h;
	GetClientSize(&w, &h);
	wxPoint m = getMouseScreenCoords();
	float glX = panX + (m.x * viewZoom);
	float glY = panY - (m.y * viewZoom);

	setMouseCoords(GLPoint2f(glX, glY));
}

//Julian: Added to allow for zoom to mouse
void klsGLCanvas::zoomToMouse(long numLines)
{
	GLPoint2f center = getCenter();
	GLPoint2f mouse = getMouseCoords();

	GLPoint2f centerToMouse = mouse - center;
	centerToMouse.x /= getZoom();
	centerToMouse.y /= getZoom();

	if (numLines > 0) {
		setZoom(getZoom() * (pow(ZOOM_STEP, numLines)));
	}
	else {
		setZoom(getZoom() / (pow(ZOOM_STEP, -numLines)));
	}

	centerToMouse.x *= getZoom();
	centerToMouse.y *= getZoom();

	setCenter(mouse.x - centerToMouse.x, mouse.y - centerToMouse.y);
}

GLPoint2f klsGLCanvas::getCenter() {
	GLPoint2f topLeft, bottomRight;
	getViewport(topLeft, bottomRight);

	GLPoint2f center = bottomRight + topLeft;
	center.x /= 2;
	center.y /= 2;

	return center;
}