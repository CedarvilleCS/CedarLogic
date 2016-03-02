/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   PaletteFrame: Organizes PaletteCanvas objects
*****************************************************************************/

#ifndef PALETTEFRAME_H_
#define PALETTEFRAME_H_

#include "MainApp.h"
#include "PaletteCanvas.h"
#include "gateImage.h"
#include <vector>

using namespace std;

#define ID_LISTBOX 6666

class PaletteFrame : public wxPanel {
public:
	PaletteFrame( wxWindow *parent, wxWindowID, const wxPoint &pos, const wxSize &size );
	~PaletteFrame();
	
	void OnListSelect( wxCommandEvent& evt );
	void OnPaint( wxPaintEvent& evt );
	
private:
	wxBoxSizer* paletteSizer;
	wxListBox* listBox;
	wxArrayString strings;
	PaletteCanvas* currentPalette;
	map < wxString, PaletteCanvas* > pcanvases;
	DECLARE_EVENT_TABLE()
};

#endif /*PALETTEFRAME_H_*/
