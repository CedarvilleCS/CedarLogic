#ifndef EDITREGDIALOG_H_
#define EDITREGDIALOG_H_

#include "MainApp.h"

#define ER_TITLE _T("Edit Registers")
#define ER_WIDTH 300
#define ER_HEIGHT 600
#define LIST_WIDTH 180
#define LIST_HEIGHT 600
#define ER_X_POS 20
#define ER_Y_POS 30

#define S_FLAG	0x80
#define Z_FLAG	0x40
#define H_FLAG	0x10
#define P_V_FLAG	0x04
#define N_FLAG	0x02
#define C_FLAG	0x01

class Z80PopupDialog;
class guiGateZ80;
class GUICircuit;

//This class is the special pop-up window that comes up when
//the "Edit Registers" button in the Z80 dialog box is clicked.
class EditRegDialog : public wxDialog{
public:
 	/** Constructor. Creates a new EditRegDialog */
	EditRegDialog( Z80PopupDialog* newZ80Popup, guiGateZ80* newM_z80GuiGate, GUICircuit* newGUICircuit, string params[] );


	void OnBtnCancel( wxCommandEvent& event );
	void OnBtnClear( wxCommandEvent& event );
	void OnBtnSave( wxCommandEvent& event );
	
	void ChangeRegValue( wxTextCtrl* currCtrl, string RegName, string RegVal, unsigned int RegSize );

protected:
	DECLARE_EVENT_TABLE()

private:
    GUICircuit* gUICircuit;
    guiGateZ80* m_guiGateZ80;
	Z80PopupDialog* z80Popup;
    
    wxButton* clearBtn;
    wxButton* cancelBtn;
    wxButton* saveBtn;
    
    wxTextCtrl* AReg;
    wxTextCtrl* BReg;
	wxTextCtrl* CReg;
	wxTextCtrl* DReg;
	wxTextCtrl* EReg;
	wxTextCtrl* HReg;
	wxTextCtrl* LReg;
	wxTextCtrl* IXReg;
	wxTextCtrl* IYReg;
	
	wxTextCtrl* AltAReg;
    wxTextCtrl* AltBReg;
	wxTextCtrl* AltCReg;
	wxTextCtrl* AltDReg;
	wxTextCtrl* AltEReg;
	wxTextCtrl* AltHReg;
	wxTextCtrl* AltLReg;
	
	wxTextCtrl* PCReg;
	wxTextCtrl* SPReg;
	wxTextCtrl* IReg;

	wxComboBox* IMode;
	wxComboBox* IFFStatus;
	
	wxComboBox* SFlag;
	wxComboBox* ZFlag;
	wxComboBox* HFlag;
	wxComboBox* PVFlag;
	wxComboBox* NFlag;
	wxComboBox* CFlag;
	
	wxComboBox* AltSFlag;
	wxComboBox* AltZFlag;
	wxComboBox* AltHFlag;
	wxComboBox* AltPVFlag;
	wxComboBox* AltNFlag;
	wxComboBox* AltCFlag;
	
};


#endif /*EDITREGDIALOG_H_*/
