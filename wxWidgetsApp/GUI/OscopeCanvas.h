/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   OscopeCanvas: renders the waveform for the oscope
*****************************************************************************/

#ifndef OSCOPECANVAS_H_
#define OSCOPECANVAS_H_

class GUICircuit;
class OscopeCanvas;

#include "MainApp.h"
#include "wx/glcanvas.h"
#include "GUICircuit.h"
#include "../logic/logic_defaults.h"

#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <deque>
using namespace std;

#define OSCOPE_HORIZONTAL 200.0

class OscopeFrame;

class OscopeCanvas: public wxGLCanvas
{
public:
    OscopeCanvas( wxWindow *parent, GUICircuit* gCircuit, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = _T("OscopeCanvas") );

    ~OscopeCanvas();

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    
    void UpdateMenu(void);
	void UpdateData(void);
		
	// Render this page
    void OnRender();
    wxImage generateImage();
    
    void clearData( void ) {
    	stateValues.clear();
    };

	// Pointer to the main application graphic circuit
	GUICircuit* gCircuit;

private:
	// Stored values of wire states:
	map< string, deque< StateType > > stateValues;
	
	bool m_init;
	
	OscopeFrame* parentFrame;
	
	DECLARE_EVENT_TABLE()
};

#endif /*OSCOPECANVAS_H_*/
