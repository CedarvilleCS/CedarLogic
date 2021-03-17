#ifndef FSMPARAMDIALOG_H_
#define FSMPARAMDIALOG_H_

#include <wx/grid.h>

#include "MainApp.h"

class guiGateFSM;
class GUICircuit;

// Pedro Casanova (casanova@ujaen.es) 2021/01-03
// This class is the special pop-up window that comes up when a FSM chip is double clicked.
class FSMParamDialog : public wxDialog{
public:
 	/** Constructor. Creates a new FSMParamDialog */
	FSMParamDialog( guiGateFSM* newM_fsmGuiGate, GUICircuit* newGUICircuit, wxCommandProcessor* wxcmd );

	void OnBtnOK( wxCommandEvent& event );
	void OnBtnCancel(wxCommandEvent& event);
	void OnBtnClear(wxCommandEvent& event);

	wxTextCtrl* stateTX;
	wxTextCtrl* infoTX;
	
protected:
	DECLARE_EVENT_TABLE()

private:	

    GUICircuit* guiCircuit;
    guiGateFSM* m_guiGateFSM;
	wxCommandProcessor* wxcmd;

    wxButton* okBtn;
	wxButton* cancelBtn;
	wxButton* clearBtn;
   
};

#endif /*FSMPARAMDIALOG_H_*/
