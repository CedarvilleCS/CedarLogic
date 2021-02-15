/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   PaletteFrame: Organizes PaletteCanvas objects
*****************************************************************************/

#include "PaletteFrame.h"
#include "wx/listbox.h"

using namespace std;

DECLARE_APP(MainApp)


BEGIN_EVENT_TABLE(PaletteFrame, wxPanel)
	EVT_LISTBOX(ID_LISTBOX, PaletteFrame::OnListSelect)
END_EVENT_TABLE()


PaletteFrame::PaletteFrame( wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size ) 
	: wxPanel( parent, id, pos, size, wxNO_BORDER ) {
	paletteSizer = new wxBoxSizer( wxVERTICAL );
	map < string, map < string, LibraryGate > >::iterator libWalk = wxGetApp().libraries.begin();
	while (libWalk != wxGetApp().libraries.end()) {
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// Libraries "Deprecated" do not show
		if (libWalk->first!="Deprecated")
			strings.Add((const wxChar *)((libWalk->first).c_str())); // KAS
		libWalk++;
	}
	//## strings.Add("Hidden");		//Test
	listBox = new wxListBox(this, ID_LISTBOX, wxDefaultPosition, wxSize(0,strings.GetCount()*14), strings, wxLB_SINGLE);
	paletteSizer->Add( listBox, wxSizerFlags(0).Expand().Border(wxALL, 0) );
	paletteSizer->Show( listBox );
	for (unsigned int i = 0; i < strings.GetCount(); i++) {
		PaletteCanvas* paletteCanvas = new PaletteCanvas( this, wxID_ANY, strings[i], wxDefaultPosition, wxDefaultSize );
		paletteSizer->Add( paletteCanvas, wxSizerFlags(1).Expand().Border(wxALL, 0) );
		paletteSizer->Hide( paletteCanvas );
		pcanvases[strings[i]] = paletteCanvas;
	}
	listBox->SetFirstItem(0);
	currentPalette = pcanvases.begin()->second;
	paletteSizer->Show( currentPalette );
	this->SetSizer( paletteSizer );
}

void PaletteFrame::OnListSelect( wxCommandEvent& evt ) {
	for (unsigned int i = 0; i < strings.GetCount(); i++) {
		if (listBox->IsSelected(i)) {
			paletteSizer->Hide( currentPalette );
			currentPalette = pcanvases[strings[i]];
			paletteSizer->Show( currentPalette );
			paletteSizer->Layout();
			currentPalette->Activate();
			break;
		}
	}
}

PaletteFrame::~PaletteFrame() {
	map < wxString, PaletteCanvas* >::iterator canvasWalk = pcanvases.begin();
	while (canvasWalk != pcanvases.end()) {
		delete canvasWalk->second;
		canvasWalk++;
	}
}
