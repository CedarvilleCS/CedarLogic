/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   PaletteCanvas: Renders the gateImage objects in a palette
*****************************************************************************/

#ifndef PALETTECANVAS_H
#define PALETTECANVAS_H

#include "MainApp.h"
#include "gateImage.h"
#include <vector>

using namespace std;

class PaletteCanvas : public wxScrolledWindow {
public:
	PaletteCanvas( wxWindow *parent, wxWindowID, wxString &libName, const wxPoint &pos, const wxSize &size );
	~PaletteCanvas();

    void OnPaint( wxPaintEvent &event );
	void Activate( void );
	
private:
	wxBoxSizer* gateSizer;
	vector< gateImage* > gates;
	string libraryName;
	bool init;
	bool activate;
	
	DECLARE_EVENT_TABLE()
};

#endif /*PALETTECANVAS_H*/
