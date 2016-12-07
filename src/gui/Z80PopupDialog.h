#ifndef Z80POPUPDIALOG_H_
#define Z80POPUPDIALOG_H_

//#include <wx/listctrl.h>

#define Z80_TITLE _T("Z80 Info")
#define Z80_WIDTH 600
#define Z80_HEIGHT 1000
#define LIST_WIDTH 180
#define LIST_HEIGHT 600
#define Z80_X_POS 20
#define Z80_Y_POS 30


#include "MainApp.h"

class guiGateZ80;
class GUICircuit;

//This class is the special pop-up window that comes up
//when a Z80 chip is double clicked.
class Z80PopupDialog : public wxDialog{
public:
 	/** Constructor. Creates a new Z80PopupDialog */
	Z80PopupDialog( guiGateZ80* newM_z80GuiGate, GUICircuit* newGUICircuit );


	void OnBtnClose( wxCommandEvent& event );
	void StepEdit_CONTINUOUS( wxCommandEvent& event );
	void StepEdit_ISTEP( wxCommandEvent& event );	
	void StepEdit_TSTEP( wxCommandEvent& event );
	
	void KPButton0_Action ( wxCommandEvent& event );
	void KPButton1_Action ( wxCommandEvent& event );
	void KPButton2_Action ( wxCommandEvent& event );
	void KPButton3_Action ( wxCommandEvent& event );
	void KPButton4_Action ( wxCommandEvent& event );
	void KPButton5_Action ( wxCommandEvent& event );
	void KPButton6_Action ( wxCommandEvent& event );
	void KPButton7_Action ( wxCommandEvent& event );
	void KPButton8_Action ( wxCommandEvent& event );
	void KPButton9_Action ( wxCommandEvent& event );
	void KPButtonA_Action ( wxCommandEvent& event );
	void KPButtonB_Action ( wxCommandEvent& event );
	void KPButtonC_Action ( wxCommandEvent& event );
	void KPButtonD_Action ( wxCommandEvent& event );
	void KPButtonE_Action ( wxCommandEvent& event );
	void KPButtonF_Action ( wxCommandEvent& event );
	void KPButtonReset_Action ( wxCommandEvent& event );
	void KPButtonGo_Action    ( wxCommandEvent& event );
	void KPButtonNext_Action  ( wxCommandEvent& event );
	void KPButtonPrev_Action  ( wxCommandEvent& event );
	void KPButtonStep_Action  ( wxCommandEvent& event );
	
	void REGModeAction ( wxCommandEvent& event );
	void ZADModeAction ( wxCommandEvent& event );	
	void SMARTModeAction ( wxCommandEvent& event );
	
	void EditReg ( wxCommandEvent& event );
	
	//This will notify the popup of a change in a parameter and call a
	//method to retreive the updated param info from the hash
	void NotifyOfRegChange ();
	
	int Z80PopupDialog::parseHex( string value );
	
	//This method is taken from Z_80LogicGate.
	//It will decode the parameter from the hash
	//into it's proper format for display in the
	//pop-up window.
//	string decodeParam( string param );
	
	
	//This is called by the guiGateZ80 when an item changes
//	void notifyItemChange( long itemIndex, long lastItemIndex );
//	void notifyAllChanged();

protected:
	DECLARE_EVENT_TABLE()

private:
	//this utility makes it so that the wxStaticText
	//gets red text if the value actually chantges
	void setLabel( wxStaticText* who, wxString* what );
	
	//This changes the labels back to black
	//for the next round of simulation;
	void blackenLabels();


    GUICircuit* gUICircuit;
    guiGateZ80* m_guiGateZ80;
    
    wxButton* closeBtn;
    wxBitmapButton* contBtn;
    wxBitmapButton* iStepBtn;
    wxBitmapButton* tStepBtn;
    wxButton* editRegBtn;
    
    wxRadioButton* REGMode;
    wxRadioButton* ZADMode;
    wxRadioButton* SMARTMode;
    
    wxStaticText* AReg;
    wxStaticText* BReg;
	wxStaticText* CReg;
	wxStaticText* DReg;
	wxStaticText* EReg;
	wxStaticText* HReg;
	wxStaticText* LReg;
	wxStaticText* IXReg;
	wxStaticText* IYReg;
	
	wxStaticText* AltAReg;
    wxStaticText* AltBReg;
	wxStaticText* AltCReg;
	wxStaticText* AltDReg;
	wxStaticText* AltEReg;
	wxStaticText* AltHReg;
	wxStaticText* AltLReg;
	
	wxStaticText* PCReg;
	wxStaticText* SPReg;
	wxStaticText* IReg;
	wxStaticText* IMode;
	wxStaticText* IFFStatus;
	
	wxStaticText* SFlag;
	wxStaticText* ZFlag;
	wxStaticText* HFlag;
	wxStaticText* PVFlag;
	wxStaticText* NFlag;
	wxStaticText* CFlag;
	
	wxStaticText* AltSFlag;
	wxStaticText* AltZFlag;
	wxStaticText* AltHFlag;
	wxStaticText* AltPVFlag;
	wxStaticText* AltNFlag;
	wxStaticText* AltCFlag;
	
	wxTextCtrl* CurrentInstruction;

	wxStaticText* hexDisp;
		
	wxButton* KPButton0;
	wxButton* KPButton1;
	wxButton* KPButton2;
	wxButton* KPButton3;
	wxButton* KPButton4;
	wxButton* KPButton5;
	wxButton* KPButton6;
	wxButton* KPButton7;
	wxButton* KPButton8;
	wxButton* KPButton9;
	wxButton* KPButtonA;
	wxButton* KPButtonB;
	wxButton* KPButtonC;
	wxButton* KPButtonD;
	wxButton* KPButtonE;
	wxButton* KPButtonF;
	wxButton* KPButtonReset;
	wxButton* KPButtonGo;
	wxButton* KPButtonNext;
	wxButton* KPButtonPrev;
	wxButton* KPButtonStep;
	
};

#endif /*Z80POPUPDIALOG_H_*/
