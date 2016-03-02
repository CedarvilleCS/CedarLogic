/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   gateImage: Generates a bitmap for a gate in the library, used in palette
*****************************************************************************/

#ifndef GATEIMAGE_H_
#define GATEIMAGE_H_

class gateImage;

#include "MainApp.h"
#include "wx/glcanvas.h"
#include "guiGate.h"
#include "../logic/logic_defaults.h"
#include "wx/generic/dragimgg.h"
#include "GUICircuit.h"
#define wxDragImage wxGenericDragImage
#include <string>

#define GATEIMAGESIZE 32
#define IMAGESIZE 34

using namespace std;

class gateImage : public wxWindow {
public:
    gateImage( string gateName, wxWindow *parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = _T("") );

    ~gateImage();

    void OnPaint(wxPaintEvent& event);
    void mouseCallback(wxMouseEvent& event);
    void OnEnterWindow(wxMouseEvent& event) { if (!(event.LeftIsDown())) inImage = true; Refresh(); };
    void OnLeaveWindow(wxMouseEvent& event) { inImage = false; Refresh(); };

	void OnEraseBackground( wxEraseEvent& event );

	string getGateName() { return gateName; };

private:
	void setViewport();
	void generateImage();
	void renderMap();
	void update();
	
	guiGate* m_gate;

	string gateName;
	bool inImage;
	wxImage gImage;
	
	wxDragImage* m_dragImage;
	bool m_init;
	GUICircuit* tempcir;
	
	DECLARE_EVENT_TABLE()
};

#endif /*GATEIMAGE_H_*/
