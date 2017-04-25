/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
					 Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.

   klsMiniMap: Renders as a bitmap the whole circuit
*****************************************************************************/

#include "klsMiniMap.h"
#include "../graphics/gl_text.h"
#include "../gate/guiGate.h"
#include "../wire/guiWire.h"
#include "klsGLCanvas.h"

// Enable access to objects in the main application
#include "../MainApp.h"
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
	wxPanel(parent, id, pos, size, style | wxSUNKEN_BORDER, name) {
	m_init = false;
	currentCanvas = NULL;
}

void klsMiniMap::setViewport() {
	// Set the projection matrix:	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	wxSize sz = GetClientSize();
	float minX = FLT_MAX, minY = FLT_MAX, maxX = -FLT_MAX, maxY = -FLT_MAX;
	auto gateWalk = gateList->begin();
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

	minCorner = Point(minX - 5, maxY + 5);
	maxCorner = Point(maxX + 5, minY - 5);

	double screenAspect = (double)sz.GetHeight() / (double)sz.GetWidth();
	double mapWidth = maxCorner.x - minCorner.x;
	double mapHeight = minCorner.y - maxCorner.y; // max and min corner's defs are weird...

	Point orthoBoxTL, orthoBoxBR;

	// If the map's width is the limiting factor:
	if (screenAspect * mapWidth >= mapHeight) {
		// Fit to width:
		double imageHeight = screenAspect * mapWidth;

		// Set the ortho box width equal to the map width, and center the
		// height in the box:
		orthoBoxTL = Point(minCorner.x, minCorner.y + 0.5*(imageHeight - mapHeight));
		orthoBoxBR = Point(maxCorner.x, maxCorner.y - 0.5*(imageHeight - mapHeight));
	}
	else {
		// Fit to height:
		double imageWidth = mapHeight / screenAspect;

		// Set the ortho box height equal to the map height, and center the
		// width in the box:
		orthoBoxTL = Point(minCorner.x - 0.5*(imageWidth - mapWidth), minCorner.y);
		orthoBoxBR = Point(maxCorner.x + 0.5*(imageWidth - mapWidth), maxCorner.y);
	}

	// gluOrtho2D(left, right, bottom, top); (In world-space coords.)
	gluOrtho2D(orthoBoxTL.x, orthoBoxBR.x, orthoBoxBR.y, orthoBoxTL.y);
	glViewport(0, 0, (GLint)sz.GetWidth(), (GLint)sz.GetHeight());

	// Store minCorner and maxCorner for use in mouse handler:
	minCorner = orthoBoxTL;
	maxCorner = orthoBoxBR;

	// Set the model matrix:
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void klsMiniMap::generateImage() {
	
	wxSize sz = GetClientSize();
    
    if (isGLContextGood()) {
        // Start drawing off-screen.
        startRenderToWxImage(sz.x, sz.y);

        setViewport();

        glViewport(0, 0, sz.GetWidth(), sz.GetHeight());

        ColorPalette::setClearColor(ColorPalette::SchematicBackground);
        ColorPalette::setColor(ColorPalette::GateShape);

        // Do the rendering here.
        renderMap();

        // Finish drawing offscreen.
        mapImage = finishRenderToWxImage();
    }
    else {
        mapImage = wxImage(sz.x, sz.y);
    }
}

void klsMiniMap::renderMap() {
	int w, h;
	GetClientSize(&w, &h);

	//clear window
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	ColorPalette::setColor(ColorPalette::GateShape);

	// Draw the wires:
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	auto thisWire = wireList->begin();
	while (thisWire != wireList->end()) {
		if (thisWire->second != nullptr) {
			(thisWire->second)->draw(true);
		}
		thisWire++;
	}

	// Draw the gates:
	auto thisGate = gateList->begin();
	while (thisGate != gateList->end()) {
		(thisGate->second)->draw(true);
		thisGate++;
	}

	if (gateList->size() == 0) return;
	glLoadIdentity();

	ColorPalette::setColor(ColorPalette::GateHotspot);
	float lineWidthOld;
	glGetFloatv(GL_LINE_WIDTH, &lineWidthOld);
	glLineWidth(2.0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(origin.x, origin.y);
	glVertex2f(origin.x, endpoint.y);
	glVertex2f(endpoint.x, endpoint.y);
	glVertex2f(endpoint.x, origin.y);
	glEnd();
	glLineWidth(lineWidthOld);
}

void klsMiniMap::update(Point origin, Point endpoint) {
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
	Refresh(false);
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
		Point p1, p2;
		wxSize sz = GetClientSize();
		currentCanvas->getViewport(p1, p2);
		Point halfViewport((p2.x - p1.x) / 2, (p2.y - p1.y) / 2);
		int diffX = evt.GetPosition().x, diffY = evt.GetPosition().y;
		float fdiffX = diffX * (maxCorner.x - minCorner.x) / sz.GetWidth();
		float fdiffY = diffY * (maxCorner.y - minCorner.y) / sz.GetHeight();
		currentCanvas->setPan(minCorner.x + fdiffX - halfViewport.x, minCorner.y + fdiffY - halfViewport.y);
	}
}
