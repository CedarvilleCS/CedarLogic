/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   klsGLCanvas: Generic implementation of OpenGL canvas
*****************************************************************************/

#ifndef KLS_GL_CANVAS_H_
#define KLS_GL_CANVAS_H_

#include <cmath>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <deque>
#include "wx/glcanvas.h"
#include "../graphics/gl_defs.h"

class klsMiniMap;

using namespace std;

#define MIN_ZOOM 1.0/120.0
#define MAX_ZOOM 1.0*1.0
#define DEFAULT_ZOOM 1.0/10.0

// The amount of zooming done per step (in %).
#define ZOOM_STEP 0.75


#define MIN_PAN -1.0e10
#define MAX_PAN 1.0e10

// The amount of panning done per step for keypress (in pixels).
#define PAN_STEP 30

// The amount of panning done per step for autoscroll (in pixels).
#define SCROLL_STEP 30
#define SCROLL_TIMER_RATE 30
#define SCROLL_TIMER_ID 1

#define GRID_INTENSITY 0.08
#define MIN_GRID_SCREEN_SPACING 13

enum mouseButton {
	BUTTON_LEFT = 0,
	BUTTON_MIDDLE,
	BUTTON_RIGHT,
	NUM_BUTTONS
};

class klsGLCanvas: public wxGLCanvas
{

public:
    klsGLCanvas( wxWindow *parent,
		const wxString& name = "klsGLCanvas",
		wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0 );

    virtual ~klsGLCanvas();


	// Print the canvas contents to a bitmap:
	wxImage renderToImage( unsigned long width, unsigned long height, unsigned long colorDepth = 32, bool noColor = false );

	//TODO: Add some scrollbars and some methods for setting the usable canvas size.

	// Handle events from wxWidgets:
    void wxOnPaint(wxPaintEvent& event);
    void wxOnSize(wxSizeEvent& event);
    void wxOnEraseBackground(wxEraseEvent& event);
    void klsGLCanvasRender( bool noColor = false );

    void wxOnMouseEvent(wxMouseEvent& event);
    void wxOnMouseWheel(wxMouseEvent& event);

    void wxKeyDown(wxKeyEvent& event);
	void wxKeyUp(wxKeyEvent& event);


	// Send events to subclassed canvas:
	// NOTE: The coordinates of the wxMouseEvent are screen
	// coordinates and should be ignored in these methods.
	virtual void OnMouseDown( wxMouseEvent& event ) {};
	virtual void OnMouseUp( wxMouseEvent& event ) {};
    virtual void OnMouseMove( GLdouble x, GLdouble y, bool ShiftDown, bool CtrlDown ) {};
    virtual void OnMouseEnter( wxMouseEvent& event ) {};
    virtual void OnMouseLeave( wxMouseEvent& event ) {};
    
    // Default OnMouseWheel handler simply zooms using the mouse wheel:
    virtual void OnMouseWheel( long numOfLines );

    virtual void OnKeyDown( wxKeyEvent& event ) {};
    virtual void OnKeyUp( wxKeyEvent& event ) {};

    virtual void OnRender( bool noColor = false ) {};
	virtual void OnSize(void) {};

	// Event query methods:
	// (Need member vars to back these up, too.)
	GLPoint2f getDragStartCoords( mouseButton whichButton = BUTTON_LEFT ) { return dragStartCoords[whichButton]; };
	GLPoint2f getMouseCoords(void) { return mouseCoords; };
	bool isMouseInWindow(void) { return !mouseOutOfWindow; }
	GLPoint2f getDragEndCoords( mouseButton whichButton = BUTTON_LEFT ) { return dragEndCoords[whichButton]; };
	bool isDragging(  mouseButton whichButton = BUTTON_LEFT ) { return isDraggingFlag[whichButton]; };
	
	// Return the point snapped to the nearest grid point:
	GLPoint2f getSnappedPoint( GLPoint2f c );

	void updateMiniMap(void);
	
	// Editing control functions
	void lockCanvas(void) { canvasLocked = true; }
	void unlockCanvas(void) { canvasLocked = false; }
	bool isLocked(void) { return canvasLocked; }

	// Event creation methods:
	// Start a drag event right away, using the current mouse coordinates.
	// This captures the mouse using CaptureMouse() and sets the "Drag Start Coords"
	// to the current mouse coordinates.
	// (This is usually called by this class right before OnMouseDown(), but
	// can be called by the subclasses. For example, right after an OnMouseEnter()
	// in which a gate is being dragged. Or, maybe also for a Paste from clipboard event.)
	void beginDrag(mouseButton whichButton = BUTTON_LEFT);

	// Force the drag event to end, by unclaiming the mouse (If all other buttons haven't
	// claimed a drag event too) and setting the "Drag End Coords".
	void endDrag(mouseButton whichButton = BUTTON_LEFT);

	// Zoom and Pan methods:
	void getPan(GLdouble &x, GLdouble &y);
	void setPan(GLdouble newX, GLdouble newY);
	void setCenter(GLdouble newX, GLdouble newY);
	void translatePan(GLdouble relX, GLdouble relY);
	void OnScrollTimer(wxTimerEvent& event);

	GLdouble getZoom() { return viewZoom; };
	void setZoom(GLdouble newZoom);
	void zoomToMouse(long); //Julian
	GLPoint2f getCenter(); //Julian

	// Grid background:
	// (Can turn grid back on without changing the past
	// setting by calling it with no params.)
	void setHorizGrid(GLfloat hSpacing = 0.0);
	void setHorizGridColor(GLfloat a, GLfloat b, GLfloat c, GLfloat d);
	void disableHorizGrid(void);

	void setVertGrid(GLfloat vSpacing = 0.0);
	void setVertGridColor(GLfloat a, GLfloat b, GLfloat c, GLfloat d);
	void disableVertGrid(void);

	bool horizOn = true; // Horizontal grid lines on/off.
	GLfloat horizSpacing; // Horizontal grid spacing.
	GLfloat hColor[4]; // Horizontal grid color.

	bool vertOn = true;  // Vertical grid lines on/off.
	GLfloat vertSpacing; // Vertical grid spacing.
	GLfloat vColor[4]; // Vertical grid color.

	// OpenGL handling routines:
	// Setup the GL matrices for this canvas:
	// (This needs to be called everytime that the matrices will be used.)
	void reclaimViewport(void);

	// Set the viewport (Set the left/top and right/bottom coordinates).
	// NOTE: It will enforce a 1:1 aspect ratio, but it will make the best
	// attempt to fit the zoom box as close as possible. Basically, it will
	// fit the longest side to the window, and center the rest.
	void setViewport(GLPoint2f topLeft, GLPoint2f bottomRight);

	// Retrieves the current viewport (left/top and right/bottom)
	void getViewport(GLPoint2f&, GLPoint2f&);

	void autoScrollEnable(void) { autoScrollActive = true; };
	void autoScrollDisable(void) { autoScrollActive = false; };
	bool isAutoScrollOn(void) { return autoScrollActive; };

protected:
	// The minimap associated with this canvas
	klsMiniMap* minimap;

	wxGLContext* glcontext;

private:
	wxPoint mouseScreenCoords;
	wxPoint getMouseScreenCoords(void) { return mouseScreenCoords;  }

	void setMouseScreenCoords( wxPoint newCoords ) { mouseScreenCoords = newCoords; }


	GLPoint2f dragStartCoords[NUM_BUTTONS];
	void setDragStartCoords( GLPoint2f newCoords, mouseButton whichButton = BUTTON_LEFT ) { dragStartCoords[whichButton] = newCoords; };

	GLPoint2f mouseCoords;
	void setMouseCoords( GLPoint2f newCoords ) { mouseCoords = newCoords; };

	// Set the mouse coords by converting last known screen coords:
	void setMouseCoords();

	GLPoint2f dragEndCoords[NUM_BUTTONS];
	void setDragEndCoords( GLPoint2f newCoords, mouseButton whichButton = BUTTON_LEFT ) { dragEndCoords[whichButton] = newCoords; };

	bool isDraggingFlag[NUM_BUTTONS];
	void setIsDragging( bool isDragging, mouseButton whichButton = BUTTON_LEFT ) { isDraggingFlag[whichButton] = isDragging; };

	bool glInitialized; // Is OpenGL initialized on this canvas

	// Zoom and OpenGL coordinate of upper-left corner of this canvas:
	GLdouble viewZoom;
	GLdouble panX, panY;
	
	// Scrolling timer used to auto-scroll the canvas when dragged outside of the
	// window:
	wxTimer* scrollTimer;
	bool autoScrollActive;

	// A variable to describe whether or not the mouse cursor is outside of the window:
	bool mouseOutOfWindow;
	
	// The accumulated mouse wheel rotation:
	double wheelRotation;
	
	// Key Control Flags
	bool isShiftDown;
	bool isControlDown;
	
	// Flag for edit control
	bool canvasLocked;

	DECLARE_EVENT_TABLE()

};

#endif /*KLS_GL_CANVAS_H_*/
