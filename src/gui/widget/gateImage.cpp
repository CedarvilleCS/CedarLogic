
#include "gateImage.h"
#include "wx/image.h"
#include "wx/wx.h"
#include "klsGLCanvas.h"
#include "../graphics/gl_text.h"
#include <fstream>
#include "gui/gate/guiGate.h"
#include "gui/GUICircuit.h"

BEGIN_EVENT_TABLE(gateImage, wxWindow)
EVT_PAINT(gateImage::OnPaint)
EVT_ENTER_WINDOW(gateImage::OnEnterWindow)
EVT_LEAVE_WINDOW(gateImage::OnLeaveWindow)
EVT_MOUSE_EVENTS(gateImage::OnMouseEvent)
EVT_ERASE_BACKGROUND(gateImage::OnEraseBackground)
END_EVENT_TABLE()

DECLARE_APP(MainApp)


using namespace std;

gateImage::gateImage(const string &gateName, wxWindow *parent) :
	wxWindow(parent, wxID_ANY, wxDefaultPosition, wxSize(IMAGESIZE, IMAGESIZE), wxFULL_REPAINT_ON_RESIZE, "") {

	inImage = false;

	m_gate = GUICircuit().createGate(gateName, 0, true);
	if (m_gate == NULL) return;
	m_gate->setGLcoords(0, 0);
	m_gate->calcBBox();
	this->gateName = gateName;
	update();

	delete m_gate;
	this->SetToolTip(wxGetApp().libraries[wxGetApp().gateNameToLibrary[gateName]][gateName].caption);
}

void gateImage::OnPaint(wxPaintEvent &event) {
	wxPaintDC dc(this);
	wxBitmap gatebitmap(gImage);

	dc.DrawBitmap(gatebitmap, 0, 0, true);
	if (inImage) {
		dc.SetPen(wxPen(*wxBLUE, 2));
	}
	else {
		dc.SetPen(wxPen(*wxWHITE, 2));
	}
	dc.SetBrush(wxBrush(*wxTRANSPARENT_BRUSH));
	dc.DrawRectangle(0, 0, IMAGESIZE, IMAGESIZE);
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
		inImage = true;

	Refresh();
}

void gateImage::OnLeaveWindow(wxMouseEvent& event) {

	inImage = false;
	Refresh();
}

void gateImage::OnEraseBackground(wxEraseEvent& event) {
	// Do nothing, so that the palette doesn't flicker!
}

void gateImage::setViewport() {
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
}

// Print the canvas contents to a bitmap:
void gateImage::generateImage() {
	//WARNING!!! Heavily platform-dependent code ahead! This only works in MS Windows because of the
	// DIB Section OpenGL rendering.

	wxSize sz = GetClientSize();

	// Create a DIB section.
	// (The Windows wxBitmap implementation will create a DIB section for a bitmap if you set
	// a color depth of 24 or greater.)
	wxBitmap theBM(GATEIMAGESIZE, GATEIMAGESIZE, 32);

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
	pfd.cColorBits = 32;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	iFormat = ::ChoosePixelFormat((HDC)theHDC, &pfd);
	::SetPixelFormat((HDC)theHDC, iFormat, &pfd);

	// create and enable the render context (RC)
	HGLRC hRC = ::wglCreateContext((HDC)theHDC);
	HGLRC oldhRC = ::wglGetCurrentContext();
	HDC oldDC = ::wglGetCurrentDC();
	::wglMakeCurrent((HDC)theHDC, hRC);

	// Setup the viewport for rendering:
	setViewport();
	// Reset the glViewport to the size of the bitmap:
	glViewport(0, 0, GATEIMAGESIZE, GATEIMAGESIZE);

	ColorPalette::setClearColor(ColorPalette::SchematicBackground);
	ColorPalette::setColor(ColorPalette::GateShape);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//TODO: Check if alpha is hardware supported, and
	// don't enable it if not!
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	//*********************************
	//Edit by Joshua Lansford 4/09/07
	//anti-alis ing is nice
	//glEnable( GL_LINE_SMOOTH );
	//End of edit

	// Load the font texture
	gl_text::loadFont(wxGetApp().appSettings.textFontFile);

	// Do the rendering here.
	renderMap();

	// Flush the OpenGL buffer to make sure the rendering has happened:	
	glFlush();

	// Destroy the OpenGL rendering context, release the memDC, and
	// convert the DIB Section into a wxImage to return to the caller:
	::wglMakeCurrent(oldDC, oldhRC);
	//::wglMakeCurrent( NULL, NULL );
	::wglDeleteContext(hRC);
	myDC.SelectObject(wxNullBitmap);
	gImage = theBM.ConvertToImage();
}

void gateImage::renderMap() {
	//clear window
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (m_gate != NULL) m_gate->draw();
}

void gateImage::update() {
	setViewport();
	generateImage();
}
