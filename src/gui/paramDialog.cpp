/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   paramDialog: Generates and handles parameters dialog for a gate
*****************************************************************************/

#include "MainApp.h"
#include "paramDialog.h"
#include "wx/filedlg.h"
#include "wx/spinctrl.h"
#include "wx/checkbox.h"
#include "wx/textctrl.h"
#include "wx/valgen.h"
#include "CircuitParse.h"
#include "commands.h"
#include <sstream>

DECLARE_APP(MainApp)

#define ID_TEXT 8888
#define ID_LOAD 8889
#define ID_SAVE 8890

BEGIN_EVENT_TABLE(paramDialog, wxDialog)
	EVT_BUTTON(wxID_OK, paramDialog::OnOK)
	EVT_BUTTON(ID_LOAD, paramDialog::OnLoad)
	EVT_BUTTON(ID_SAVE, paramDialog::OnSave)
	EVT_TEXT(ID_TEXT, paramDialog::OnTextEntry)
END_EVENT_TABLE()

paramDialog::paramDialog(const wxString& title, void* gCircuit, guiGate* gGate, wxCommandProcessor* wxcmd)
       : wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(250,400), wxCAPTION|wxFRAME_TOOL_WINDOW|wxSTAY_ON_TOP)
{
	// Copy the circuit and gate pointers to this frame:
	this->gCircuit = (GUICircuit*)gCircuit;
	this->gGate = gGate;
	this->wxcmd = wxcmd;
	
	LibraryGate* gateDef = &(wxGetApp().libraries[gGate->getLibraryName()][gGate->getLibraryGateName()]);
	unsigned int numParams = gateDef->dlgParams.size();
	dlgSizer = new wxGridSizer(numParams+1, 2, 2, 2);
	SetSizer(dlgSizer);
	
	for (unsigned int i = 0; i < numParams; i++) {
		paramNames.push_back(new wxStaticText(this, wxID_ANY, gateDef->dlgParams[i].textLabel));
		dlgSizer->Add( paramNames[paramNames.size()-1], 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 3 );
		string initialString;
		// Generate control from type. Type can be: STRING, INT, BOOL, FLOAT, FILE_IN, FILE_OUT.
		if ( gateDef->dlgParams[i].type == "INT" ) {
			// Retrieve a default integer value
			if ( gateDef->dlgParams[i].isGui ) initialString = gGate->getGUIParam(gateDef->dlgParams[i].name);
			else initialString = gGate->getLogicParam(gateDef->dlgParams[i].name);
			istringstream initVal(initialString);
			int initValInt;
			initVal >> initValInt;
			paramVals.push_back(new wxSpinCtrl(this, ID_TEXT, initialString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, (int)(gateDef->dlgParams[i].Rmin), (int)(gateDef->dlgParams[i].Rmax), initValInt));
//			wxGenericValidator* gv = new wxGenericValidator(wxFILTER_INCLUDE_CHAR_LIST, ((wxSpinCtrl*)(paramVals[paramVals.size()-1]))->GetValue())
//			paramVals[paramVals.size()-1]->SetValidator(gv);
		} else if ( gateDef->dlgParams[i].type == "STRING" ) {
			if ( gateDef->dlgParams[i].isGui ) initialString = gGate->getGUIParam(gateDef->dlgParams[i].name);
			else initialString = gGate->getLogicParam(gateDef->dlgParams[i].name);
			paramVals.push_back(new wxTextCtrl(this, wxID_ANY, initialString));
		} else if ( gateDef->dlgParams[i].type == "BOOL" ) {
			paramVals.push_back( new wxCheckBox( this, wxID_ANY, "" ) );
			// Retrieve the current param setting
			if ( gateDef->dlgParams[i].isGui ) initialString = gGate->getGUIParam(gateDef->dlgParams[i].name);
			else initialString = gGate->getLogicParam(gateDef->dlgParams[i].name);
			if ( initialString == "true" ) ((wxCheckBox*)(paramVals[paramVals.size()-1]))->SetValue(true);
		} else if ( gateDef->dlgParams[i].type == "FLOAT" ) {
			if ( gateDef->dlgParams[i].isGui ) initialString = gGate->getGUIParam(gateDef->dlgParams[i].name);
			else initialString = gGate->getLogicParam(gateDef->dlgParams[i].name);
			paramVals.push_back(new wxTextCtrl(this, ID_TEXT, initialString));
		} else if ( gateDef->dlgParams[i].type == "FILE_IN" ) {
			paramVals.push_back( new wxButton( this, ID_LOAD, "Load File" ) );
		} else if ( gateDef->dlgParams[i].type == "FILE_OUT" ) {
			paramVals.push_back( new wxButton( this, ID_SAVE, "Save File" ) );			
		}
		dlgSizer->Add( paramVals[paramVals.size()-1], 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 3 );
	}
	
	// Put in the standard dialog buttons
	ok = new wxButton( this, wxID_OK, "&OK" );
	dlgSizer->Add( ok, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	cancel = new wxButton( this, wxID_CANCEL, "&Cancel" );
	dlgSizer->Add( cancel, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	this->SetDefaultItem(ok);

	dlgSizer->Fit(this);
	dlgSizer->SetSizeHints(this);
}

paramDialog::~paramDialog() {
	// Delete the various objects
	//for (unsigned int i = 0; i < paramNames.size(); i++) delete paramNames[i];
}

void paramDialog::OnLoad( wxCommandEvent &evt ) {
	// Find the control
	unsigned int parmID = paramVals.size()+1;
	for (unsigned int i = 0; i < paramVals.size(); i++) {
		if (evt.GetEventObject() == paramVals[i]) {
			parmID = i;
			break;
		}
	}
	if (parmID == paramVals.size()+1) return;
	string paramName = (const char *)paramNames[parmID]->GetLabel().c_str(); // KAS
	
	wxString caption = "Open a memory file";
	//Edit by Joshua Lansford 1/24/06  Added the option to select Intel-hex files
	wxString wildcard = "CEDAR Memory files (*.cdm)|*.cdm|INTEL-HEX (*.hex)|*.hex";
	wxString defaultFilename = "";
	wxFileDialog dialog(this, caption, wxEmptyString, defaultFilename, wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	
	LibraryGate* gateDef = &(wxGetApp().libraries[gGate->getLibraryName()][gGate->getLibraryGateName()]);
	if (dialog.ShowModal() == wxID_OK) {
		wxString path = dialog.GetPath();
		string mempath = (const char *)path.c_str(); // KAS
		gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(gGate->getID(), gateDef->dlgParams[parmID].name, mempath)));
	}
}

void paramDialog::OnSave( wxCommandEvent &evt ) {
	// Find the control
	unsigned int parmID = paramVals.size()+1;
	for (unsigned int i = 0; i < paramVals.size(); i++) {
		if (evt.GetEventObject() == paramVals[i]) {
			parmID = i;
			break;
		}
	}
	if (parmID == paramVals.size()+1) return;
	string paramName = (const char *) paramNames[parmID]->GetLabel().c_str(); // KAS
	
	wxString caption = "Open a memory file";
	wxString wildcard = "CEDAR Memory files (*.cdm)|*.cdm";
	wxString defaultFilename = "";
	wxFileDialog dialog(this, caption, wxEmptyString, defaultFilename, wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	
	LibraryGate* gateDef = &(wxGetApp().libraries[gGate->getLibraryName()][gGate->getLibraryGateName()]);
	if (dialog.ShowModal() == wxID_OK) {
		wxString path = dialog.GetPath();
		string mempath = (const char *)path.c_str(); // KAS
		gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(gGate->getID(), gateDef->dlgParams[parmID].name, mempath)));
	}
}

void paramDialog::OnOK( wxCommandEvent &evt ) {
	// Are we all good?
	wxString msg;
	if (!validateData()) return;
	// Set up a params map and send it off to cmdSetParams
	map < string, string > lParamList;
	map < string, string > gParamList;
	LibraryGate* gateDef = &(wxGetApp().libraries[gGate->getLibraryName()][gGate->getLibraryGateName()]);
	for (unsigned int i = 0; i < gateDef->dlgParams.size(); i++) {
		string pValue;
		if ( gateDef->dlgParams[i].type == "INT" ) {
			ostringstream oss;
			// Check range:
			if (((wxSpinCtrl*)(paramVals[i]))->GetValue() < (int)(gateDef->dlgParams[i].Rmin) ||
				((wxSpinCtrl*)(paramVals[i]))->GetValue() > (int)(gateDef->dlgParams[i].Rmax)) {
				// ERROR
				cout << "ERROR: Parameter " << gateDef->dlgParams[i].textLabel << " is out of range.";
				wxMessageBox(msg, "Error", wxOK | wxICON_ERROR, NULL);
				return;
			}
			// Retrieve the integer value
			oss << ((wxSpinCtrl*)(paramVals[i]))->GetValue();
			pValue = oss.str();
		} else if ( gateDef->dlgParams[i].type == "STRING") {
			pValue = (string)((const char *)((wxTextCtrl*)(paramVals[i]))->GetValue().c_str());  // KAS
		} else if ( gateDef->dlgParams[i].type == "FLOAT" ) {
			ostringstream oss;
			// Check range:
			pValue = (string)((const char *)((wxTextCtrl*)(paramVals[i]))->GetValue().c_str()); // kAS
			istringstream iss(pValue);
			float fVal;
			iss >> fVal;
			if (fVal < gateDef->dlgParams[i].Rmin || fVal > gateDef->dlgParams[i].Rmax) {
				// ERROR
				cout << "ERROR: Parameter " << gateDef->dlgParams[i].textLabel << " is out of range.";
				wxMessageBox(msg, "Error", wxOK | wxICON_ERROR, NULL);
				return;
			}
		} else if ( gateDef->dlgParams[i].type == "BOOL" ) {
			pValue = ( ((wxCheckBox*)(paramVals[i]))->GetValue() ? "true" : "false" );
		}
		if (gateDef->dlgParams[i].isGui) gParamList[gateDef->dlgParams[i].name] = pValue;
		else lParamList[gateDef->dlgParams[i].name] = pValue;
	}
	// If no params are different, then don't submit a command
	bool allParamsSame = true;
	map < string, string >::iterator paramWalk = gParamList.begin();
	while (paramWalk != gParamList.end() && allParamsSame) {
		allParamsSame = (gGate->getGUIParam(paramWalk->first) == paramWalk->second);
		paramWalk++;
	}
	paramWalk = lParamList.begin();
	while (paramWalk != lParamList.end() && allParamsSame) {
		allParamsSame = (gGate->getLogicParam(paramWalk->first) == paramWalk->second);
		paramWalk++;
	}
	if (!allParamsSame) wxcmd->Submit( new cmdSetParams( gCircuit, gGate->getID(), paramSet(&gParamList, &lParamList) ) );
	// Make me go away forever!
	this->EndModal(wxID_OK);
}

bool paramDialog::validateData() {
	bool retVal = true;
	// Go through and validate the items we need to validate
	LibraryGate* gateDef = &(wxGetApp().libraries[gGate->getLibraryName()][gGate->getLibraryGateName()]);
	for (unsigned int i = 0; i < gateDef->dlgParams.size(); i++) {
		if (gateDef->dlgParams[i].type == "FLOAT") {
			
		}
	}
	return retVal;
}
