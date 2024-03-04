/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   gateImage: Generates a bitmap for a gate in the library, used in palette
*****************************************************************************/

#include "gateImage.h"
#include "wx/image.h"
#include "wx/wx.h"
#include "klsGLCanvas.h"
#include "guiText.h"
#include <fstream>
#include <wx/dnd.h>
#include "glToImage.h"
#include "MainFrame.h"

BEGIN_EVENT_TABLE(gateImage, wxWindow)
    EVT_PAINT(gateImage::OnPaint)
    EVT_ENTER_WINDOW( gateImage::OnEnterWindow )
    EVT_LEAVE_WINDOW( gateImage::OnLeaveWindow )
    EVT_MOUSE_EVENTS( gateImage::mouseCallback )
	EVT_ERASE_BACKGROUND(gateImage::OnEraseBackground)
END_EVENT_TABLE()

DECLARE_APP(MainApp)

gateImage::gateImage( string gateName, wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        long style, const wxString& name ) : 
        wxWindow(parent, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name ) {
	m_init = false;
	inImage = false;

	m_gate = GUICircuit().createGate(gateName, 0, true);
	if (m_gate == NULL) return;
	m_gate->setGLcoords(0,0);
	m_gate->calcBBox();
	this->gateName = gateName;
	update();

	delete m_gate;
	SetToolTip(wxGetApp().libraries[wxGetApp().gateNameToLibrary[gateName]][gateName].caption);
}

gateImage::~gateImage() {
}

void gateImage::OnPaint(wxPaintEvent &event) {
	wxPaintDC dc(this);
	wxBitmap gatebitmap(gImage);
	dc.DrawBitmap(gatebitmap, 0, 0, true);	
	if (inImage) {
		dc.SetPen(wxPen(*wxBLUE, 2, wxPENSTYLE_SOLID));
	} else {
		dc.SetPen(wxPen(*wxWHITE, 2, wxPENSTYLE_SOLID));
	}
		dc.SetBrush(wxBrush(*wxTRANSPARENT_BRUSH));
		dc.DrawRectangle(0,0,IMAGESIZE,IMAGESIZE);
	//event.Skip();
}

void gateImage::mouseCallback( wxMouseEvent& event) {
	if (event.LeftDown()) {
#ifndef USE_WX_DRAGDROP
		wxGetApp().newGateToDrag = gateName;

#ifdef __WXGTK__
		// In gtk the canvas doesn't get mouse events when dragging over
		// it, so we have to tell it to capture the mouse.
		wxGetApp().mainframe->PreGateDrag();

		// Stop drawing the border
		inImage = false;
		Refresh();
#endif

#else
		// If USE_WX_DRAGDROP is defined, we use the wxwidgets native
		// drag and drop. This seems more likely to work cross platform,
		// but it doesn't provide a preview of the gate being dragged.
		wxTextDataObject textData(gateName);
		wxDropSource source(textData, this);

		int flags = 0;
		source.DoDragDrop(flags);
#endif
	} else if (event.LeftUp()) {
		wxGetApp().newGateToDrag = "";
	} else {
		// This is required to pass scroll events through
		event.Skip();
	}
}

void gateImage::OnEraseBackground( wxEraseEvent& event ) {
	// Do nothing, so that the palette doesn't flicker!
}

void gateImage::setViewport() {
	// Set the projection matrix:	
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	wxSize sz = GetClientSize();
	klsBBox m_gatebox = m_gate->getModelBBox();
	GLPoint2f minCorner = GLPoint2f(m_gatebox.getLeft()-0.5,m_gatebox.getTop()+0.5);
	GLPoint2f maxCorner = GLPoint2f(m_gatebox.getRight()+0.5,m_gatebox.getBottom()-0.5);

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
void gateImage::generateImage() {
	glImageCtx glCtx(GATEIMAGESIZE, GATEIMAGESIZE, this);

	// Setup the viewport for rendering:
	setViewport();
	// Reset the glViewport to the size of the bitmap:
	glViewport(0, 0, GATEIMAGESIZE, GATEIMAGESIZE);
	
	// Set the bitmap clear color:
	glClearColor (1.0, 1.0, 1.0, 0.0);
	glColor3b(0, 0, 0);

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		
	//TODO: Check if alpha is hardware supported, and
	// don't enable it if not!
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	
	//*********************************
	//Edit by Joshua Lansford 4/09/07
	//anti-alis ing is nice
	glEnable( GL_LINE_SMOOTH );
	//End of edit

	// Load the font texture
	guiText::loadFont(wxGetApp().appSettings.textFontFile);
	
	// Do the rendering here.
	renderMap();

	// Flush the OpenGL buffer to make sure the rendering has happened:	
	glFlush();

	gImage = glCtx.getImage();
}

void gateImage::renderMap() {
	//clear window
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glColor4f( 0, 0, 0, 1 );

	if (m_gate != NULL) m_gate->draw();
}

void gateImage::update() {
	setViewport();
	generateImage();
}
