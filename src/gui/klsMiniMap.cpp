/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   klsMiniMap: Renders as a bitmap the whole circuit
*****************************************************************************/

#include "klsMiniMap.h"
#include "guiText.h"

#include "guiGate.h"
#include "guiWire.h"

// Enable access to objects in the main application
DECLARE_APP(MainApp)

BEGIN_EVENT_TABLE(klsMiniMap, wxPanel)
	EVT_PAINT(klsMiniMap::OnPaint)
	
	//Josh Edit 4/9/07
    EVT_ERASE_BACKGROUND(klsMiniMap::OnEraseBackground)
	EVT_MOUSE_EVENTS(klsMiniMap::OnMouseEvent)
END_EVENT_TABLE()

klsMiniMap::klsMiniMap(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        long style, const wxString& name) :
	wxPanel(parent, id, pos, size, style|wxSUNKEN_BORDER, name ) {
	m_init = false;
	currentCanvas = NULL;
}

void klsMiniMap::setViewport() {
	// Set the projection matrix:	
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	wxSize sz = GetClientSize();
	float minX = FLT_MAX, minY = FLT_MAX, maxX = -FLT_MAX, maxY = -FLT_MAX;
	unordered_map < unsigned long, guiGate* >::iterator gateWalk = gateList->begin();
	while (gateWalk != gateList->end()) {
		float x, y;
		(gateWalk->second)->getGLcoords(x, y);
		if (x < minX) minX = x;
		if (y < minY) minY = y;
		if (x > maxX) maxX = x;
		if (y > maxY) maxY = y;
		gateWalk++;
	}
	if (origin.x < minX) minX = origin.x;
	if (origin.y > maxY) maxY = origin.y;
	if (endpoint.x > maxX) maxX = endpoint.x;
	if (endpoint.y < minY) minY = endpoint.y;

	minCorner = GLPoint2f(minX-5,maxY+5);
	maxCorner = GLPoint2f(maxX+5,minY-5);

	double screenAspect = (double) sz.GetHeight() / (double) sz.GetWidth();
	double mapWidth = maxCorner.x - minCorner.x;
	double mapHeight = minCorner.y - maxCorner.y; // max and min corner's defs are weird...
	
	GLPoint2f orthoBoxTL, orthoBoxBR;
	
	// If the map's width is the limiting factor:
	if( screenAspect * mapWidth >= mapHeight ) {
		// Fit to width:
		double imageHeight = screenAspect * mapWidth;

		// Set the ortho box width equal to the map width, and center the
		// height in the box:
		orthoBoxTL = GLPoint2f( minCorner.x, minCorner.y + 0.5*(imageHeight - mapHeight) );
		orthoBoxBR = GLPoint2f( maxCorner.x, maxCorner.y - 0.5*(imageHeight - mapHeight) );
	} else {
		// Fit to height:
		double imageWidth = mapHeight / screenAspect;

		// Set the ortho box height equal to the map height, and center the
		// width in the box:
		orthoBoxTL = GLPoint2f( minCorner.x - 0.5*(imageWidth - mapWidth), minCorner.y );
		orthoBoxBR = GLPoint2f( maxCorner.x + 0.5*(imageWidth - mapWidth), maxCorner.y );
	}

	// gluOrtho2D(left, right, bottom, top); (In world-space coords.)
	gluOrtho2D(orthoBoxTL.x, orthoBoxBR.x, orthoBoxBR.y, orthoBoxTL.y);
	glViewport(0, 0, (GLint) sz.GetWidth(), (GLint) sz.GetHeight());

	// Store minCorner and maxCorner for use in mouse handler:
	minCorner = orthoBoxTL;
	maxCorner = orthoBoxBR;

	// Set the model matrix:
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}

// Print the canvas contents to a bitmap:
void klsMiniMap::generateImage() {
//WARNING!!! Heavily platform-dependent code ahead! This only works in MS Windows because of the
// DIB Section OpenGL rendering.

	wxSize sz = GetClientSize();

	// Create a DIB section.
	// (The Windows wxBitmap implementation will create a DIB section for a bitmap if you set
	// a color depth of 24 or greater.)
	wxBitmap theBM( sz.GetWidth(), sz.GetHeight(), 32 );
	
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
    ::ZeroMemory( &pfd, sizeof( pfd ) );
    pfd.nSize = sizeof( pfd );
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_SUPPORT_GDI;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ::ChoosePixelFormat( (HDC) theHDC, &pfd );
    ::SetPixelFormat( (HDC) theHDC, iFormat, &pfd );

    // create and enable the render context (RC)
    HGLRC hRC = ::wglCreateContext( (HDC) theHDC );
    HGLRC oldhRC = ::wglGetCurrentContext();
    HDC oldDC = ::wglGetCurrentDC();
    ::wglMakeCurrent( (HDC) theHDC, hRC );

	// Setup the viewport for rendering:
	setViewport();
	// Reset the glViewport to the size of the bitmap:
	glViewport(0, 0, (GLint) sz.GetWidth(), (GLint) sz.GetHeight());
	
	// Set the bitmap clear color:
	glClearColor (1.0, 1.0, 1.0, 0.0);
	glColor3b(0, 0, 0);
		
	//TODO: Check if alpha is hardware supported, and
	// don't enable it if not!
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	
	//*********************************
	//Edit by Joshua Lansford 4/08/07
	//The minimap could use some anti
	//aleasing
	glEnable( GL_LINE_SMOOTH );
	//End of edit
		
	// Load the font texture
	guiText::loadFont(wxGetApp().appSettings.textFontFile);

	// Do the rendering here.
	renderMap();

	// Flush the OpenGL buffer to make sure the rendering has happened:	
	glFlush();
	
	// Destroy the OpenGL rendering context, release the memDC, and
	// convert the DIB Section into a wxImage to return to the caller:
    ::wglMakeCurrent( oldDC, oldhRC );
    //::wglMakeCurrent( NULL, NULL );
    ::wglDeleteContext( hRC );
	myDC.SelectObject(wxNullBitmap);
	mapImage = theBM.ConvertToImage();
}

void klsMiniMap::renderMap() {
	int w, h;
	GetClientSize(&w, &h);

	//clear window
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glColor4f( 0, 0, 0, 1 );
	
	// Draw the wires:
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	unordered_map< unsigned long, guiWire* >::iterator thisWire = wireList->begin();
	while( thisWire != wireList->end() ) {
		if (thisWire->second != nullptr) {
			(thisWire->second)->draw(false);
		}
		thisWire++;
	}

	// Draw the gates:
	unordered_map< unsigned long, guiGate* >::iterator thisGate = gateList->begin();
	while( thisGate != gateList->end() ) {
		(thisGate->second)->draw(false);
		thisGate++;
	}
	
	if (gateList->size() == 0) return;
	glLoadIdentity();
	glColor4f( 1, 0, 0, 1 );
	GLfloat lineWidthOld;
	glGetFloatv(GL_LINE_WIDTH, &lineWidthOld);
	glLineWidth(2.0);
	glBegin(GL_LINE_LOOP);
		glVertex2f( origin.x, origin.y );
		glVertex2f( origin.x, endpoint.y );
		glVertex2f( endpoint.x, endpoint.y );
		glVertex2f( endpoint.x, origin.y );
	glEnd();
	glLineWidth(lineWidthOld);
}

void klsMiniMap::update(GLPoint2f origin, GLPoint2f endpoint) {
	//wxClientDC dc(this);
	//dc.FloodFill(0, 0, *wxWHITE);
	this->origin = origin;
	this->endpoint = endpoint;
	
	generateImage();
	
	//*****************************
	//Edit by Joshua Lansford 4/9/07
	//In search of determineing the
	//cause of the miniMap spazz bug
	//I refactored update and
	//OnPaint.
	//Instead of update doing both
	//jobs, I now have OnPaint
	//handling the painting
	//and update handling the update-
	//ing.  It didn't seem right
	//for the update to be grabbing
	//the hardware and rendering
	//to it when we were not servicing
	//a onPaint request.
	//***************************
	
	
	//wxBitmap mapBitmap(mapImage);
	//dc.DrawBitmap(mapBitmap, 0, 0, true);
	Refresh( false );
	//Update makes it so that we don't lag
	//when the user is moveing the cavase around
	wxWindow::Update();
}

void klsMiniMap::OnPaint(wxPaintEvent& evt) {
	//Josh Edit 4/9/07 see coment put in update
	
	//update();
	
	wxPaintDC dc(this);
	//dc.FloodFill(0, 0, *wxWHITE);
	wxBitmap mapBitmap(mapImage);
	dc.DrawBitmap(mapBitmap, 0, 0, true);
	
	evt.Skip();
}

//Josh Edit 4/9/07 We were flickering, so I put this in here.
void klsMiniMap::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
  // Do nothing, to avoid flashing.
}

void klsMiniMap::OnMouseEvent(wxMouseEvent& evt) {
	if (evt.LeftIsDown() && currentCanvas != NULL) {
		GLPoint2f p1, p2;
		wxSize sz = GetClientSize();
		currentCanvas->getViewport( p1, p2 );
		GLPoint2f halfViewport((p2.x-p1.x)/2, (p2.y-p1.y)/2);
		int diffX = evt.GetPosition().x, diffY = evt.GetPosition().y;
		float fdiffX = diffX * (maxCorner.x-minCorner.x)/sz.GetWidth();
		float fdiffY = diffY * (maxCorner.y-minCorner.y)/sz.GetHeight();
		currentCanvas->setPan( minCorner.x+fdiffX-halfViewport.x, minCorner.y+fdiffY-halfViewport.y );
	}
}
