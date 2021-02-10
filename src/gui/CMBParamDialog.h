#ifndef CMBPARAMDIALOG_H_
#define CMBPARAMDIALOG_H_

#include <wx/grid.h>

#include "MainApp.h"

//bool chkBits(string* bits, bool X=false);

class guiGateCMB;
class GUICircuit;

// Pedro Casanova (casanova@ujaen.es) 2021/01-02
// This class is the special pop-up window that comes up when a FSM chip is double clicked.
class CMBParamDialog : public wxDialog{
public:
 	/** Constructor. Creates a new FSMParamDialog */
	CMBParamDialog( guiGateCMB* newM_cmbGuiGate, GUICircuit* newGUICircuit, wxCommandProcessor* wxcmd );

	void OnBtnOK( wxCommandEvent& event );
	void OnBtnCancel(wxCommandEvent& event);
	void OnBtnClear(wxCommandEvent& event);

	wxTextCtrl* functionTX;
	wxTextCtrl* infoTX;
	
protected:
	DECLARE_EVENT_TABLE()

private:
    GUICircuit* guiCircuit;
    guiGateCMB* m_guiGateCMB;
	wxCommandProcessor* wxcmd;

    wxButton* okBtn;
	wxButton* cancelBtn;
	wxButton* clearBtn;
   
};

#endif /*CMBPARAMDIALOG_H_*/
