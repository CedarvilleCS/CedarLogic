/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   paramDialog: Generates and handles parameters dialog for a gate
*****************************************************************************/

#ifndef PARAMDIALOG_H_
#define PARAMDIALOG_H_

#include "MainApp.h"
#include "GUICircuit.h"
#include "guiGate.h"
#include <string>
#include <map>

using namespace std;

class paramDialog : public wxDialog {
public:
    // ctor(s)
    
    paramDialog(const wxString& title, void* gCircuit, guiGate* gGate, wxCommandProcessor* wxcmd);
	~paramDialog();
	
	void OnOK( wxCommandEvent &evt );
	void OnLoad( wxCommandEvent &evt );
	void OnSave( wxCommandEvent &evt );
	void OnTextEntry( wxCommandEvent &evt) { validateData(); };
	
private:
	GUICircuit* gCircuit;
	guiGate* gGate;
	wxCommandProcessor* wxcmd;
	wxGridSizer* dlgSizer;
	vector< wxStaticText* > paramNames;
	vector< wxControl* > paramVals;
	wxButton* ok;
	wxButton* cancel;

	bool validateData();

    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

#endif /*PARAMDIALOG_H_*/
