/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   klsMiniMap: Renders as a bitmap the whole circuit
*****************************************************************************/

#ifndef KLSMINIMAP_H_
#define KLSMINIMAP_H_

class klsMiniMap;

#include "wx/glcanvas.h"
#include "wx/image.h"
#include "wx/bitmap.h"
#include "wx/dcmemory.h"
#include "klsGLCanvas.h"
#include <hash_map>
using namespace std;

class guiWire;
class guiGate;

class klsMiniMap : public wxPanel {
public:
	klsMiniMap(wxWindow *parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = _T("klsMiniMap"));
	virtual ~klsMiniMap() { return; };
	
	void setLists( hash_map< unsigned long, guiGate* >* gateList, hash_map< unsigned long, guiWire* >* wireList ) {
		this->gateList = gateList;
		this->wireList = wireList;
	};
	
	void update(GLPoint2f origin = GLPoint2f(0,0), GLPoint2f endpoint = GLPoint2f(0,0));
	void setCanvas( klsGLCanvas* kcanvas ) { currentCanvas = kcanvas; };
	void OnPaint(wxPaintEvent& evt);
	void OnMouseEvent(wxMouseEvent& evt);
	
	//Josh Edit 4/9/07
	void OnEraseBackground(wxEraseEvent& WXUNUSED(event));
	
private:
	void setViewport();
	void generateImage();
	void renderMap();
	
	// viewport rect
	GLPoint2f origin, endpoint;
	
	hash_map< unsigned long, guiGate* >* gateList;
	hash_map< unsigned long, guiWire* >* wireList;
	
	wxImage mapImage;
	bool m_init;

	klsGLCanvas* currentCanvas;	
	
	GLPoint2f minCorner, maxCorner;
DECLARE_EVENT_TABLE()
};

#endif /*KLSMINIMAP_H_*/
