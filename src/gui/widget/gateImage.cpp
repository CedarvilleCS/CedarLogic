
#include "gateImage.h"
#include "wx/image.h"
#include "wx/wx.h"
#include "klsGLCanvas.h"
#include "../graphics/gl_text.h"
#include "gui/graphics/gl_defs.h"
#include <fstream>
#include "gui/gate/guiGate.h"
#include "gui/GUICircuit.h"

wxBEGIN_EVENT_TABLE(gateImage, wxGLCanvas)
EVT_PAINT(gateImage::OnPaint)
EVT_ENTER_WINDOW(gateImage::OnEnterWindow)
EVT_LEAVE_WINDOW(gateImage::OnLeaveWindow)
EVT_MOUSE_EVENTS(gateImage::OnMouseEvent)
EVT_ERASE_BACKGROUND(gateImage::OnEraseBackground)
wxEND_EVENT_TABLE()

DECLARE_APP(MainApp)

using namespace std;

gateImage::gateImage(const string &gateName, wxWindow *parent) :
	wxGLCanvas(parent, wxID_ANY, nullptr, wxDefaultPosition, wxSize(IMAGESIZE, IMAGESIZE), wxFULL_REPAINT_ON_RESIZE, "") {

	if (count == 0) {
		
		glContext = new wxGLContext(this);
		glContext->SetCurrent(*this);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		// Load the font texture
		gl_text::loadFont(wxGetApp().appSettings.textFontFile);
	}


	count++;

	mouseHover = false;

	this->gateName = gateName;

	this->SetToolTip(wxGetApp().libraries[wxGetApp().gateNameToLibrary[gateName]][gateName].caption);
}

gateImage::~gateImage() {

	count--;
	if (count == 0) {
		delete glContext;
	}
}

void gateImage::OnPaint(wxPaintEvent &event) {

	glContext->SetCurrent(*this);

	// Clear the background.
	if (mouseHover) {
		ColorPalette::setClearColor(ColorPalette::GateHotspot);
	}
	else {
		ColorPalette::setClearColor(ColorPalette::SchematicBackground);
	}
	glClear(GL_COLOR_BUFFER_BIT);

	// Create and destroy an entire circuit just to make a gate.
	guiGate *m_gate = GUICircuit().createGate(gateName, 0, true);

	// Set drawing bounds based on gate.
	m_gate->setGLcoords(0, 0);
	m_gate->calcBBox();
	setViewport(m_gate);
	
	// Draw the gate and destory it.
	ColorPalette::setColor(ColorPalette::GateShape);
	m_gate->draw();
	delete m_gate;

	SwapBuffers();
}

void gateImage::OnMouseEvent(wxMouseEvent& event) {

	if (event.LeftDown()) {
		wxGetApp().newGateToDrag = gateName;
	}
	else if (event.LeftUp()) {
		wxGetApp().newGateToDrag = "";
	}
}

void gateImage::OnEnterWindow(wxMouseEvent& event) {

	if (!(event.LeftIsDown()))
		mouseHover = true;

	Refresh();
}

void gateImage::OnLeaveWindow(wxMouseEvent& event) {

	mouseHover = false;
	Refresh();
}

void gateImage::OnEraseBackground(wxEraseEvent& event) {
	// Do nothing, so that the palette doesn't flicker!
}



void gateImage::setViewport(guiGate *m_gate) {

	// Set the projection matrix:	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	wxSize sz = GetClientSize();
	klsBBox m_gatebox = m_gate->getModelBBox();
	Point minCorner = Point(m_gatebox.getLeft() - 0.5, m_gatebox.getTop() + 0.5);
	Point maxCorner = Point(m_gatebox.getRight() + 0.5, m_gatebox.getBottom() - 0.5);

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

	// Reset the glViewport to the size of the bitmap:
	glViewport(0, 0, GATEIMAGESIZE, GATEIMAGESIZE);
}

wxGLContext *gateImage::glContext = nullptr;
int gateImage::count = 0;