#include "wx/wx.h"

#include <ostream>
#include <sstream>
#include <iomanip>
#include <wx/sizer.h>
#include <wx/statline.h>
#include "Z80PopupDialog.h"
#include "EditRegDialog.h"
#include "guiGate.h"
#include "GUICircuit.h"
#include "MainFrame.h"
#include "../z80/Z_80LogicGate.h"
#include <stdlib.h>

using namespace std;

/*
const int H_KEY_0 = 0x06;
const int H_KEY_1 = 0x05;
const int H_KEY_2 = 0x01;
const int H_KEY_3 = 0x08;
const int H_KEY_4 = 0x0C;
const int H_KEY_5 = 0x0B;
const int H_KEY_6 = 0x07;
const int H_KEY_7 = 0x09;
const int H_KEY_8 = 0x12;
const int H_KEY_9 = 0x17;
const int H_KEY_A = 0x02;
const int H_KEY_B = 0x03;
const int H_KEY_C = 0x18;
const int H_KEY_D = 0x11;
const int H_KEY_E = 0x04;
const int H_KEY_F = 0x0A;
const int H_KEY_RESET = 0xFF;
*/

enum {
	ID_KEY_0 = wxID_HIGHEST + 1,
	ID_KEY_1 = wxID_HIGHEST + 2,
	ID_KEY_2 = wxID_HIGHEST + 3,
	ID_KEY_3 = wxID_HIGHEST + 4,
	ID_KEY_4 = wxID_HIGHEST + 5,
	ID_KEY_5 = wxID_HIGHEST + 6,
	ID_KEY_6 = wxID_HIGHEST + 7,
	ID_KEY_7 = wxID_HIGHEST + 8,
	ID_KEY_8 = wxID_HIGHEST + 9,
	ID_KEY_9 = wxID_HIGHEST + 10,
	ID_KEY_A = wxID_HIGHEST + 11,
	ID_KEY_B = wxID_HIGHEST + 12,
	ID_KEY_C = wxID_HIGHEST + 13,
	ID_KEY_D = wxID_HIGHEST + 14,
	ID_KEY_E = wxID_HIGHEST + 15,
	ID_KEY_F = wxID_HIGHEST + 16,
	ID_KEY_RESET = wxID_HIGHEST + 17,
	ID_KEY_GO   = wxID_HIGHEST + 18,
	ID_KEY_NEXT = wxID_HIGHEST + 19,
	ID_KEY_PREV = wxID_HIGHEST + 20,
	ID_KEY_STEP = wxID_HIGHEST + 21,
	ID_CONTSTEP = wxID_HIGHEST + 22,
	ID_ISTEP = wxID_HIGHEST + 23,
	ID_TSTEP = wxID_HIGHEST + 24,
	ID_EDITREG = wxID_HIGHEST + 25,
	ID_REGMODE = wxID_HIGHEST + 26,
	ID_ZADMODE = wxID_HIGHEST + 27,
	ID_SMARTMODE = wxID_HIGHEST + 28
};



DECLARE_APP(MainApp)

Z80PopupDialog::Z80PopupDialog( guiGateZ80* newM_guiGateZ80, 
        GUICircuit* newGUICircuit )
	: wxDialog( wxGetApp().GetTopWindow(), -1, Z80_TITLE, 
	    wxPoint(Z80_X_POS, Z80_Y_POS), 
	    wxSize(Z80_WIDTH, Z80_HEIGHT),
	    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	m_guiGateZ80 = newM_guiGateZ80;
	gUICircuit = newGUICircuit;
	
	//Bitmaps
	wxBitmap bmpCont(wxBITMAP(cont));
	wxBitmap bmpIStep(wxBITMAP(step));
	wxBitmap bmpTStep(wxBITMAP(step));
	
	
	wxBoxSizer* topSizer = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer* buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* Row1Sizer = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer* Row2Sizer = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer* Row3Sizer = new wxBoxSizer( wxHORIZONTAL );	
	
//Interior Sizers
	wxGridSizer* DataRegSizer = new wxGridSizer( 0, 2, 0, 0);
	wxGridSizer* AltRegSizer = new wxGridSizer( 0, 2, 0, 0);
	wxGridSizer* CtlRegSizer = new wxGridSizer( 0, 2, 0, 0);
	wxGridSizer* FlagRegSizer = new wxGridSizer( 0, 2, 0, 0);
	wxGridSizer* AltFlagRegSizer = new wxGridSizer (0, 2, 0, 0);
	wxGridSizer* KeypadSizer = new wxGridSizer (0, 4, 0, 0);
	wxGridSizer* FunctionSizer = new wxGridSizer (0, 2, 0, 0);
	wxGridSizer* StepSizer = new wxGridSizer (0, 2, 0, 0);
	wxBoxSizer* Keypad_Reset_Sizer = new wxBoxSizer ( wxVERTICAL );
	wxBoxSizer* Step_Title_Sizer = new wxBoxSizer ( wxVERTICAL );
	wxBoxSizer* InstrSizer = new wxBoxSizer ( wxVERTICAL );
	
//Horizontal Separators
	wxStaticLine* HLine1 = new wxStaticLine (this, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	wxStaticLine* HLine2 = new wxStaticLine (this, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	wxStaticLine* HLine3 = new wxStaticLine (this, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	wxStaticLine* HLine4 = new wxStaticLine (this, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);

//Vertical Separators
	wxStaticLine* VLine1 = new wxStaticLine (this, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	wxStaticLine* VLine2 = new wxStaticLine (this, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	wxStaticLine* VLine3 = new wxStaticLine (this, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	wxStaticLine* VLine4 = new wxStaticLine (this, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	wxStaticLine* VLine5 = new wxStaticLine (this, -1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	
//Buttons
	closeBtn = new wxButton( this, wxID_CLOSE );
	wxStaticText* StepTitle = new wxStaticText(this, -1, (const wxChar *)"Step By:");  // cast here and following KAS
	//contBtn = new wxBitmapButton( this, ID_CONTSTEP, bmpCont );
	wxStaticText* contBtnLabel = new wxStaticText(this, -1, (const wxChar *)"Continuous");
	//iStepBtn = new wxBitmapButton( this, ID_ISTEP, bmpIStep);
	wxStaticText* iStepBtnLabel = new wxStaticText(this, -1, (const wxChar *)"Instruction");
	//tStepBtn = new wxBitmapButton( this, ID_TSTEP, bmpTStep);
	wxStaticText* tStepBtnLabel = new wxStaticText(this, -1, (const wxChar *)"T-State");
	editRegBtn = new wxButton(this, ID_EDITREG, (const wxChar *)"Edit Registers...");
	
	REGMode = new wxRadioButton(this, ID_REGMODE, (const wxChar *)"ZAD Mode off", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	ZADMode = new wxRadioButton(this, ID_ZADMODE, (const wxChar *)"ZAD Mode on", wxDefaultPosition, wxDefaultSize);
	SMARTMode = new wxRadioButton(this, ID_SMARTMODE, (const wxChar *)"ZAD Mode smart", wxDefaultPosition, wxDefaultSize);
	
	//default SMART mode to true
	SMARTMode->SetValue(true);
	
	wxStaticText* ALabel = new wxStaticText(this, -1, (const wxChar *)"A = ");
	AReg = new wxStaticText(this, -1, (const wxChar *)"XX");
	wxStaticText* BLabel = new wxStaticText(this, -1, (const wxChar *)"B = ");
	BReg = new wxStaticText(this, -1, (const wxChar *)"XX");
	wxStaticText* CLabel = new wxStaticText(this, -1, (const wxChar *)"C = ");
	CReg = new wxStaticText(this, -1, (const wxChar *)"XX");
	wxStaticText* DLabel = new wxStaticText(this, -1, (const wxChar *)"D = ");
	DReg = new wxStaticText(this, -1, (const wxChar *)"XX");
	wxStaticText* ELabel = new wxStaticText(this, -1, (const wxChar *)"E = ");
	EReg = new wxStaticText(this, -1, (const wxChar *)"XX");
	wxStaticText* HLabel = new wxStaticText(this, -1, (const wxChar *)"H = ");
	HReg = new wxStaticText(this, -1, (const wxChar *)"XX");
	wxStaticText* LLabel = new wxStaticText(this, -1, (const wxChar *)"L = ");
	LReg = new wxStaticText(this, -1, (const wxChar *)"XX");
	wxStaticText* IXLabel = new wxStaticText(this, -1, (const wxChar *)"IX = ");
	IXReg = new wxStaticText(this, -1, (const wxChar *)"XXXX");
	wxStaticText* IYLabel = new wxStaticText(this, -1, (const wxChar *)"IY = ");
	IYReg = new wxStaticText(this, -1, (const wxChar *)"XXXX");

	wxStaticText* AltALabel = new wxStaticText(this, -1, (const wxChar *)"A' = ");
	AltAReg = new wxStaticText(this, -1, (const wxChar *)"XX");
	wxStaticText* AltBLabel = new wxStaticText(this, -1, (const wxChar *)"B' = ");
	AltBReg = new wxStaticText(this, -1, (const wxChar *)"XX");
	wxStaticText* AltCLabel = new wxStaticText(this, -1, (const wxChar *)"C' = ");
	AltCReg = new wxStaticText(this, -1, (const wxChar *)"XX");
	wxStaticText* AltDLabel = new wxStaticText(this, -1, (const wxChar *)"D' = ");
	AltDReg = new wxStaticText(this, -1, (const wxChar *)"XX");
	wxStaticText* AltELabel = new wxStaticText(this, -1, (const wxChar *)"E' = ");
	AltEReg = new wxStaticText(this, -1, (const wxChar *)"XX");
	wxStaticText* AltHLabel = new wxStaticText(this, -1, (const wxChar *)"H' = ");
	AltHReg = new wxStaticText(this, -1, (const wxChar *)"XX");
	wxStaticText* AltLLabel = new wxStaticText(this, -1, (const wxChar *)"L' = ");
	AltLReg = new wxStaticText(this, -1, (const wxChar *)"XX");
	
	wxStaticText* PCLabel = new wxStaticText(this, -1, (const wxChar *)"PC = ");
	PCReg = new wxStaticText(this, -1, (const wxChar *)"XXXX");
	wxStaticText* SPLabel = new wxStaticText(this, -1, (const wxChar *)"SP = ");
	SPReg = new wxStaticText(this, -1, (const wxChar *)"XXXX");
	wxStaticText* ILabel = new wxStaticText(this, -1, (const wxChar *)"I = ");
	IReg = new wxStaticText(this, -1, (const wxChar *)"XX");
	wxStaticText* IMLabel = new wxStaticText(this, -1, (const wxChar *)"IM = ");
	IMode = new wxStaticText(this, -1, (const wxChar *)"Mode 0");
	wxStaticText* IFFLabel = new wxStaticText(this, -1, (const wxChar *)"IFF = ");
	IFFStatus = new wxStaticText(this, -1, (const wxChar *)"Disabled");
	
	//FLAGS
	//flags
	wxStaticText* SFlagLabel = new wxStaticText(this, -1, (const wxChar *)"S = ");
	SFlag = new wxStaticText(this, -1, (const wxChar *)"X");
	wxStaticText* ZFlagLabel = new wxStaticText(this, -1, (const wxChar *)"Z = ");
	ZFlag = new wxStaticText(this, -1, (const wxChar *)"X");
	wxStaticText* HFlagLabel = new wxStaticText(this, -1, (const wxChar *)"H = ");
	HFlag = new wxStaticText(this, -1, (const wxChar *)"X");
	wxStaticText* PVFlagLabel = new wxStaticText(this, -1, (const wxChar *)"P/V = ");
	PVFlag = new wxStaticText(this, -1, (const wxChar *)"X");
	wxStaticText* NFlagLabel = new wxStaticText(this, -1, (const wxChar *)"N = ");
	NFlag = new wxStaticText(this, -1, (const wxChar *)"X");
	wxStaticText* CFlagLabel = new wxStaticText(this, -1, (const wxChar *)"C = ");
	CFlag = new wxStaticText(this, -1, (const wxChar *)"X");
	
	//alternate flags
	wxStaticText* AltSFlagLabel = new wxStaticText(this, -1, (const wxChar *)"S' = ");
	AltSFlag = new wxStaticText(this, -1, (const wxChar *)"X");
	wxStaticText* AltZFlagLabel = new wxStaticText(this, -1, (const wxChar *)"Z' = ");
	AltZFlag = new wxStaticText(this, -1, (const wxChar *)"X");
	wxStaticText* AltHFlagLabel = new wxStaticText(this, -1, (const wxChar *)"H' = ");
	AltHFlag = new wxStaticText(this, -1, (const wxChar *)"X");
	wxStaticText* AltPVFlagLabel = new wxStaticText(this, -1, (const wxChar *)"P/V' = ");
	AltPVFlag = new wxStaticText(this, -1, (const wxChar *)"X");
	wxStaticText* AltNFlagLabel = new wxStaticText(this, -1, (const wxChar *)"N' = ");
	AltNFlag = new wxStaticText(this, -1, (const wxChar *)"X");
	wxStaticText* AltCFlagLabel = new wxStaticText(this, -1, (const wxChar *)"C' = ");
	AltCFlag = new wxStaticText(this, -1, (const wxChar *)"X");
	
	//current instruction
	wxSize* CISize = new wxSize (300, 100);
	
	wxStaticText* CurrInstructionTitle = new wxStaticText(this, -1, (const wxChar *)"Current Instruction");
	CurrentInstruction = new wxTextCtrl(this, -1, (const wxChar *)"", wxDefaultPosition, *CISize, wxTE_MULTILINE | wxTE_READONLY | wxTE_WORDWRAP);

	//hex display
	hexDisp = new wxStaticText(this, -1, (const wxChar *)"000000");
	wxFont* dispFont = new wxFont(30, wxFONTFAMILY_TELETYPE, -1, wxBOLD);
	hexDisp->SetFont(*dispFont);
	hexDisp->SetForegroundColour(0x3300FF);
	
	//keypad buttons
	wxSize* kpbuttonSize = new wxSize (20, 20);
	wxSize* twoWideButtonSize = new wxSize(40, 20);
	wxSize* kpresetbuttonSize = new wxSize (80, 20);
	
	KPButton0 = new wxButton(this, ID_KEY_0, (const wxChar *)"0", wxDefaultPosition, *kpbuttonSize);
	KPButton1 = new wxButton(this, ID_KEY_1, (const wxChar *)"1", wxDefaultPosition, *kpbuttonSize);
	KPButton2 = new wxButton(this, ID_KEY_2, (const wxChar *)"2", wxDefaultPosition, *kpbuttonSize);
	KPButton3 = new wxButton(this, ID_KEY_3, (const wxChar *)"3", wxDefaultPosition, *kpbuttonSize);
	
	KPButton4 = new wxButton(this, ID_KEY_4, (const wxChar *)"4", wxDefaultPosition, *kpbuttonSize);
	KPButton5 = new wxButton(this, ID_KEY_5, (const wxChar *)"5", wxDefaultPosition, *kpbuttonSize);
	KPButton6 = new wxButton(this, ID_KEY_6, (const wxChar *)"6", wxDefaultPosition, *kpbuttonSize);
	KPButton7 = new wxButton(this, ID_KEY_7, (const wxChar *)"7", wxDefaultPosition, *kpbuttonSize);
	
	KPButton8 = new wxButton(this, ID_KEY_8, (const wxChar *)"8", wxDefaultPosition, *kpbuttonSize);
	KPButton9 = new wxButton(this, ID_KEY_9, (const wxChar *)"9", wxDefaultPosition, *kpbuttonSize);
	KPButtonA = new wxButton(this, ID_KEY_A, (const wxChar *)"A", wxDefaultPosition, *kpbuttonSize);
	KPButtonB = new wxButton(this, ID_KEY_B, (const wxChar *)"B", wxDefaultPosition, *kpbuttonSize);
	
	KPButtonC = new wxButton(this, ID_KEY_C, (const wxChar *)"C", wxDefaultPosition, *kpbuttonSize);
	KPButtonD = new wxButton(this, ID_KEY_D, (const wxChar *)"D", wxDefaultPosition, *kpbuttonSize);
	KPButtonE = new wxButton(this, ID_KEY_E, (const wxChar *)"E", wxDefaultPosition, *kpbuttonSize);
	KPButtonF = new wxButton(this, ID_KEY_F, (const wxChar *)"F", wxDefaultPosition, *kpbuttonSize);
	
	KPButtonGo = new wxButton(this, ID_KEY_GO, (const wxChar *)"Go", wxDefaultPosition, *twoWideButtonSize);
	KPButtonStep = new wxButton(this, ID_KEY_STEP, (const wxChar *)"Step", wxDefaultPosition, *twoWideButtonSize);
	KPButtonNext = new wxButton(this, ID_KEY_NEXT, (const wxChar *)"Next", wxDefaultPosition, *twoWideButtonSize);
	KPButtonPrev = new wxButton(this, ID_KEY_PREV, (const wxChar *)"Prev", wxDefaultPosition, *twoWideButtonSize);
	KPButtonReset = new wxButton(this, ID_KEY_RESET, (const wxChar *)"Reset", wxDefaultPosition, *kpresetbuttonSize);
	
//*************DATA REGISTERS***************************************************
	DataRegSizer->Add( ALabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	DataRegSizer->Add( AReg,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	DataRegSizer->Add( BLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	DataRegSizer->Add( BReg,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	DataRegSizer->Add( CLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	DataRegSizer->Add( CReg,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	DataRegSizer->Add( DLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	DataRegSizer->Add( DReg,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	DataRegSizer->Add( ELabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	DataRegSizer->Add( EReg,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	DataRegSizer->Add( HLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	DataRegSizer->Add( HReg,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	DataRegSizer->Add( LLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	DataRegSizer->Add( LReg,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	DataRegSizer->Add( IXLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	DataRegSizer->Add( IXReg,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	DataRegSizer->Add( IYLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	DataRegSizer->Add( IYReg,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
//*******************************************************************************
	
//*************ALTERNATE REGISTERS***********************************************
	AltRegSizer->Add( AltALabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	AltRegSizer->Add( AltAReg,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	AltRegSizer->Add( AltBLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	AltRegSizer->Add( AltBReg,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	AltRegSizer->Add( AltCLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	AltRegSizer->Add( AltCReg,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	AltRegSizer->Add( AltDLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	AltRegSizer->Add( AltDReg,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	AltRegSizer->Add( AltELabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	AltRegSizer->Add( AltEReg,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	AltRegSizer->Add( AltHLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	AltRegSizer->Add( AltHReg,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	AltRegSizer->Add( AltLLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	AltRegSizer->Add( AltLReg,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
//*****************************************************************************

//*************CONTROL REGISTERS***********************************************
	CtlRegSizer->Add( PCLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	CtlRegSizer->Add( PCReg,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));

	CtlRegSizer->Add( SPLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	CtlRegSizer->Add( SPReg,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	CtlRegSizer->Add( ILabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	CtlRegSizer->Add( IReg,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	CtlRegSizer->Add( IMLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	CtlRegSizer->Add( IMode,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	CtlRegSizer->Add( IFFLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	CtlRegSizer->Add( IFFStatus,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
//*****************************************************************************

//*************FLAGS***********************************************************
//flags
	FlagRegSizer->Add( SFlagLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	FlagRegSizer->Add( SFlag,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	FlagRegSizer->Add( ZFlagLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	FlagRegSizer->Add( ZFlag,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	FlagRegSizer->Add( HFlagLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	FlagRegSizer->Add( HFlag,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	FlagRegSizer->Add( PVFlagLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	FlagRegSizer->Add( PVFlag,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	FlagRegSizer->Add( NFlagLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	FlagRegSizer->Add( NFlag,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	FlagRegSizer->Add( CFlagLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	FlagRegSizer->Add( CFlag,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
//alternate flags
	AltFlagRegSizer->Add( AltSFlagLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	AltFlagRegSizer->Add( AltSFlag,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	AltFlagRegSizer->Add( AltZFlagLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	AltFlagRegSizer->Add( AltZFlag,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	AltFlagRegSizer->Add( AltHFlagLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	AltFlagRegSizer->Add( AltHFlag,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	AltFlagRegSizer->Add( AltPVFlagLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	AltFlagRegSizer->Add( AltPVFlag,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	AltFlagRegSizer->Add( AltNFlagLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	AltFlagRegSizer->Add( AltNFlag,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
	
	AltFlagRegSizer->Add( AltCFlagLabel,wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 1 ));
	AltFlagRegSizer->Add( AltCFlag,wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 1 ));
//*****************************************************************************

//*************Keypad**********************************************************
	KeypadSizer->Add( KPButton0 ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	KeypadSizer->Add( KPButton1 ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	KeypadSizer->Add( KPButton2 ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	KeypadSizer->Add( KPButton3 ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));

	KeypadSizer->Add( KPButton4 ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	KeypadSizer->Add( KPButton5 ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	KeypadSizer->Add( KPButton6 ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	KeypadSizer->Add( KPButton7 ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	
	KeypadSizer->Add( KPButton8 ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	KeypadSizer->Add( KPButton9 ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	KeypadSizer->Add( KPButtonA ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	KeypadSizer->Add( KPButtonB ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	
	KeypadSizer->Add( KPButtonC ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	KeypadSizer->Add( KPButtonD ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	KeypadSizer->Add( KPButtonE ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	KeypadSizer->Add( KPButtonF ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	
	FunctionSizer->Add( KPButtonGo    ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	FunctionSizer->Add( KPButtonStep  ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	FunctionSizer->Add( KPButtonNext  ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	FunctionSizer->Add( KPButtonPrev  ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	
	Keypad_Reset_Sizer->Add( KeypadSizer,   wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	Keypad_Reset_Sizer->Add( FunctionSizer, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
	Keypad_Reset_Sizer->Add( KPButtonReset ,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0 ));
//*****************************************************************************

	InstrSizer->Add( CurrInstructionTitle, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));
	InstrSizer->Add( CurrentInstruction, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));
	InstrSizer->Add( REGMode, wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 5));
	InstrSizer->Add( ZADMode, wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 5));
	InstrSizer->Add( SMARTMode, wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 5));

	//StepSizer->Add( contBtn, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 1));
	StepSizer->Add( contBtnLabel, wxSizerFlags(0).Align(wxALIGN_LEFT).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 1));
	//StepSizer->Add( iStepBtn, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 1));
	StepSizer->Add( iStepBtnLabel, wxSizerFlags(0).Align(wxALIGN_LEFT).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 1));
	//StepSizer->Add( tStepBtn, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 1));
	StepSizer->Add( tStepBtnLabel, wxSizerFlags(0).Align(wxALIGN_LEFT).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 1));
	

	
	Step_Title_Sizer->Add( StepTitle, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));
	Step_Title_Sizer->Add( StepSizer, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));

	buttonSizer->Add( editRegBtn,wxSizerFlags(0).Align(wxALIGN_CENTER).Expand().Border(wxALL, 5 ));
	buttonSizer->Add( closeBtn,wxSizerFlags(0).Align(wxALIGN_CENTER).Expand().Border(wxALL, 5 ));
	
//****************************************************************************************************

	Row1Sizer->Add( DataRegSizer, wxSizerFlags(0).Align(wxALIGN_CENTER).Align(wxALIGN_TOP).Border(wxALL, 5));
	Row1Sizer->Add( VLine1, wxSizerFlags(0).Align(wxALIGN_CENTER).Expand().Border(wxALL, 5));
	Row1Sizer->Add( FlagRegSizer, wxSizerFlags(0).Align(wxALIGN_CENTER).Align(wxALIGN_TOP).Border(wxALL, 5));
	Row1Sizer->Add( VLine2, wxSizerFlags(0).Align(wxALIGN_CENTER).Expand().Border(wxALL, 5));
	Row1Sizer->Add( AltRegSizer, wxSizerFlags(0).Align(wxALIGN_CENTER).Align(wxALIGN_TOP).Border(wxALL, 5));
	Row1Sizer->Add( VLine3, wxSizerFlags(0).Align(wxALIGN_CENTER).Expand().Border(wxALL, 5));
	Row1Sizer->Add( AltFlagRegSizer, wxSizerFlags(0).Align(wxALIGN_CENTER).Align(wxALIGN_TOP).Border(wxALL, 5));
	
	Row2Sizer->Add( CtlRegSizer, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));
	Row2Sizer->Add( VLine4, wxSizerFlags(0).Align(wxALIGN_CENTER).Expand().Border(wxALL, 5));
	Row2Sizer->Add( Step_Title_Sizer, wxSizerFlags(0).Align(wxALIGN_CENTER).Expand().Border(wxALL, 5));
	Row2Sizer->Add( VLine5, wxSizerFlags(0).Align(wxALIGN_CENTER).Expand().Border(wxALL, 5));
	Row2Sizer->Add( Keypad_Reset_Sizer ,wxSizerFlags(0).Align(wxALIGN_CENTER).Expand().Border(wxALL, 5));
	
	Row3Sizer->Add( InstrSizer, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));
	
	topSizer->Add( Row1Sizer, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));
	topSizer->Add( HLine1, wxSizerFlags(0).Align(wxALIGN_CENTER).Expand().Border(wxALL, 5));
	topSizer->Add( Row2Sizer, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));
	topSizer->Add( HLine2, wxSizerFlags(0).Align(wxALIGN_CENTER).Expand().Border(wxALL, 5));
	topSizer->Add( Row3Sizer, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));
	topSizer->Add( HLine3, wxSizerFlags(0).Align(wxALIGN_CENTER).Expand().Border(wxALL, 5));
	topSizer->Add( hexDisp, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));
	topSizer->Add( HLine4, wxSizerFlags(0).Align(wxALIGN_CENTER).Expand().Border(wxALL, 5));
	topSizer->Add( buttonSizer,wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5 ));
	
	SetSizer( topSizer );
	topSizer->SetSizeHints( this );
	NotifyOfRegChange();
}

void Z80PopupDialog::OnBtnClose( wxCommandEvent& event ){
	Close(false);
}

void Z80PopupDialog::StepEdit_CONTINUOUS( wxCommandEvent& event){    
 	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
 	blackenLabels();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "RUN_MODE", "CONTINUOUS")));
}

void Z80PopupDialog::StepEdit_ISTEP( wxCommandEvent& event){    
 	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
 	
 	blackenLabels();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "RUN_MODE", "I_STEP")));
}

void Z80PopupDialog::StepEdit_TSTEP( wxCommandEvent& event ){    
 	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
 	blackenLabels();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "RUN_MODE", "T_STEP")));
}

//Keypad Button Action Functions
//Button 0
void Z80PopupDialog::KPButton0_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_0, true)));
	
	//wxMessageBox(_T("0 pressed"), _T("Success"), wxOK | wxICON_ERROR, NULL);
}
//Button 1
void Z80PopupDialog::KPButton1_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_1, true)));
	
	//wxMessageBox(_T("1 pressed"), _T("Success"), wxOK | wxICON_ERROR, NULL);
}
//Button 2
void Z80PopupDialog::KPButton2_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_2, true)));
	
	//wxMessageBox(_T("2 pressed"), _T("Success"), wxOK | wxICON_ERROR, NULL);
}
//Button 3
void Z80PopupDialog::KPButton3_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_3, true)));
	
	//wxMessageBox(_T("3 pressed"), _T("Success"), wxOK | wxICON_ERROR, NULL);
}
//Button 4
void Z80PopupDialog::KPButton4_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_4, true)));
	
	//wxMessageBox(_T("4 pressed"), _T("Success"), wxOK | wxICON_ERROR, NULL);
}
//Button 5
void Z80PopupDialog::KPButton5_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_5, true)));
	
	//wxMessageBox(_T("5 pressed"), _T("Success"), wxOK | wxICON_ERROR, NULL);
}
//Button 6
void Z80PopupDialog::KPButton6_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_6, true)));
	
	//wxMessageBox(_T("6 pressed"), _T("Success"), wxOK | wxICON_ERROR, NULL);
}
//Button 7
void Z80PopupDialog::KPButton7_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_7, true)));
	
	//wxMessageBox(_T("7 pressed"), _T("Success"), wxOK | wxICON_ERROR, NULL);
}
//Button 8
void Z80PopupDialog::KPButton8_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_8, true)));
	
	//wxMessageBox(_T("8 pressed"), _T("Success"), wxOK | wxICON_ERROR, NULL);
}
//Button 9
void Z80PopupDialog::KPButton9_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_9, true)));
	
	//wxMessageBox(_T("9 pressed"), _T("Success"), wxOK | wxICON_ERROR, NULL);
}
//Button A
void Z80PopupDialog::KPButtonA_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_A, true)));
	
	//wxMessageBox(_T("A pressed"), _T("Success"), wxOK | wxICON_ERROR, NULL);
}
//Button B
void Z80PopupDialog::KPButtonB_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_B, true)));
	
	//wxMessageBox(_T("B pressed"), _T("Success"), wxOK | wxICON_ERROR, NULL);
}
//Button C
void Z80PopupDialog::KPButtonC_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_C, true)));
	
	//wxMessageBox(_T("C pressed"), _T("Success"), wxOK | wxICON_ERROR, NULL);
}
//Button D
void Z80PopupDialog::KPButtonD_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_D, true)));
	
	//wxMessageBox(_T("D pressed"), _T("Success"), wxOK | wxICON_ERROR, NULL);
}
//Button E
void Z80PopupDialog::KPButtonE_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_E, true)));
	
	//wxMessageBox(_T("E pressed"), _T("Success"), wxOK | wxICON_ERROR, NULL);
}
//Button F
void Z80PopupDialog::KPButtonF_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_F, true)));
	
	//wxMessageBox(_T("F pressed"), _T("Success"), wxOK | wxICON_ERROR, NULL);
}
//Button RESET
void Z80PopupDialog::KPButtonReset_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_RESET, true)));
	
	
	//wxMessageBox(_T("reset pressed"), _T("Success"), wxOK | wxICON_ERROR, NULL);
}
//Button GO
void Z80PopupDialog::KPButtonGo_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_GO, true)));
}
//Button NEXT
void Z80PopupDialog::KPButtonNext_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_NEXT, true)));
}
//Button PREV
void Z80PopupDialog::KPButtonPrev_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_PREV, true)));
}
//Button STEP
void Z80PopupDialog::KPButtonStep_Action ( wxCommandEvent& event ){
	ostringstream oss;
 	
 	wxGetApp().mainframe->ResumeExecution();
 	
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "H_KEY", H_KEY_STEP, true)));
}



void Z80PopupDialog::EditReg ( wxCommandEvent& event ) {
	
	//This array will be used to pass current register information
	//to the editRegDialog to populate the boxes with current values
	string Z80Params[] = {(const char *)AReg->GetLabel().c_str(),				//params[0]
		(const char *)BReg->GetLabel().c_str(),				//params[1]
		(const char *)CReg->GetLabel().c_str(),				//params[2]
		(const char *)DReg->GetLabel().c_str(),				//params[3]
		(const char *)EReg->GetLabel().c_str(),				//params[4]
		(const char *)HReg->GetLabel().c_str(),				//params[5]
		(const char *)LReg->GetLabel().c_str(),				//params[6]
		(const char *)IXReg->GetLabel().c_str(),			//params[7]
		(const char *)IYReg->GetLabel().c_str(),			//params[8]
		(const char *)AltAReg->GetLabel().c_str(),			//params[9]
		(const char *)AltBReg->GetLabel().c_str(),			//params[10]
		(const char *)AltCReg->GetLabel().c_str(),			//params[11]
		(const char *)AltDReg->GetLabel().c_str(),			//params[12]
		(const char *)AltEReg->GetLabel().c_str(),			//params[13]
		(const char *)AltHReg->GetLabel().c_str(),			//params[14]
		(const char *)AltLReg->GetLabel().c_str(),			//params[15]
		(const char *)SFlag->GetLabel().c_str(),			//params[16]
		(const char *)ZFlag->GetLabel().c_str(),			//params[17]
		(const char *)HFlag->GetLabel().c_str(),			//params[18]
		(const char *)PVFlag->GetLabel().c_str(),			//params[19]
		(const char *)NFlag->GetLabel().c_str(),			//params[20]
		(const char *)CFlag->GetLabel().c_str(),			//params[21]
		(const char *)AltSFlag->GetLabel().c_str(),			//params[22]
		(const char *)AltZFlag->GetLabel().c_str(),			//params[23]
		(const char *)AltHFlag->GetLabel().c_str(),			//params[24]
		(const char *)AltPVFlag->GetLabel().c_str(),		//params[25]
		(const char *)AltNFlag->GetLabel().c_str(),			//params[26]
		(const char *)AltCFlag->GetLabel().c_str(),			//params[27]
		(const char *)PCReg->GetLabel().c_str(),			//params[28]
		(const char *)SPReg->GetLabel().c_str(),			//params[29]
		(const char *)IReg->GetLabel().c_str(),				//params[30]
		(const char *)IMode->GetLabel().c_str(),			//params[31]
		(const char *)IFFStatus->GetLabel().c_str()			//params[32]
						  };
	
	EditRegDialog* editRegDialog = new EditRegDialog(this, m_guiGateZ80, gUICircuit, Z80Params);
	editRegDialog->ShowModal();
}

//This will notify the popup of a change in a parameter and call a
//method to retreive the updated param info from the hash
void Z80PopupDialog::NotifyOfRegChange (){
	wxString xxString = (const wxChar *)"XX";
	wxString xxxxString = (const wxChar *)"XXXX";
	wxString oneString = (const wxChar *)"1";
	wxString zeroString = (const wxChar *)"0";
	
	
	//capture data registers
	wxString aParam = (const wxChar *)m_guiGateZ80->getLogicParam("A").c_str();
	wxString bParam = (const wxChar *)m_guiGateZ80->getLogicParam("B").c_str();
	wxString cParam = (const wxChar *)m_guiGateZ80->getLogicParam("C").c_str();
	wxString dParam = (const wxChar *)m_guiGateZ80->getLogicParam("D").c_str();
	wxString eParam = (const wxChar *)m_guiGateZ80->getLogicParam("E").c_str();
	wxString hParam = (const wxChar *)m_guiGateZ80->getLogicParam("H").c_str();
	wxString lParam = (const wxChar *)m_guiGateZ80->getLogicParam("L").c_str();
	wxString ixParam = (const wxChar *)m_guiGateZ80->getLogicParam("IX").c_str();
	wxString iyParam = (const wxChar *)m_guiGateZ80->getLogicParam("IY").c_str();
	
	//set data registers in popup
	if (aParam == (const wxChar *)""){
		setLabel( AReg, &xxString  );
	} else { setLabel( AReg, &aParam  ); }
	if (bParam == (const wxChar *)""){
		setLabel( BReg, &xxString  );
	} else { setLabel( BReg, &bParam  ); }
	if (cParam == (const wxChar *)""){
		setLabel( CReg, &xxString  );
	} else { setLabel( CReg, &cParam  ); }
	if (dParam == (const wxChar *)""){
		setLabel( DReg, &xxString  );
	} else { setLabel( DReg, &dParam  ); }
	if (eParam == (const wxChar *)""){
		setLabel( EReg, &xxString  );
	} else { setLabel( EReg, &eParam  ); }
	if (hParam == (const wxChar *)""){
		setLabel( HReg, &xxString  );
	} else { setLabel( HReg, &hParam  ); }
	if (lParam == (const wxChar *)""){
		setLabel( LReg, &xxString  );
	} else { setLabel( LReg, &lParam  ); }
	if (ixParam == (const wxChar *)""){
		setLabel( IXReg, &xxxxString  );
	} else { setLabel( IXReg, &ixParam  ); }
	if (iyParam == (const wxChar *)""){
		setLabel( IYReg, &xxxxString  );
	} else { setLabel( IYReg, &iyParam  ); }
	
	//capture alternate registers
	wxString AltaParam = (const wxChar *)m_guiGateZ80->getLogicParam("A'").c_str();
	wxString AltbParam = (const wxChar *)m_guiGateZ80->getLogicParam("B'").c_str();
	wxString AltcParam = (const wxChar *)m_guiGateZ80->getLogicParam("C'").c_str();
	wxString AltdParam = (const wxChar *)m_guiGateZ80->getLogicParam("D'").c_str();
	wxString AlteParam = (const wxChar *)m_guiGateZ80->getLogicParam("E'").c_str();
	wxString AlthParam = (const wxChar *)m_guiGateZ80->getLogicParam("H'").c_str();
	wxString AltlParam = (const wxChar *)m_guiGateZ80->getLogicParam("L'").c_str();
	
	//set alternate registers in popup
	if (AltaParam == (const wxChar *)""){
		setLabel( AltAReg, &xxString  );
	} else { setLabel( AltAReg, &AltaParam  ); }
	if (AltbParam == (const wxChar *)""){
		setLabel( AltBReg, &xxString  );
	} else { setLabel( AltBReg, &AltbParam  ); }
	if (AltcParam == (const wxChar *)""){
		setLabel( AltCReg, &xxString  );
	} else { setLabel( AltCReg, &AltcParam  ); }
	if (AltdParam == (const wxChar *)""){
		setLabel( AltDReg, &xxString  );
	} else { setLabel( AltDReg, &AltdParam  ); }
	if (AlteParam == (const wxChar *)""){
		setLabel( AltEReg, &xxString  );
	} else { setLabel( AltEReg, &AlteParam  ); }
	if (AlthParam == (const wxChar *)""){
		setLabel( AltHReg, &xxString  );
	} else { setLabel( AltHReg, &AlthParam  ); }
	if (AltlParam == (const wxChar *)""){
		setLabel( AltLReg, &xxString  );
	} else { setLabel( AltLReg, &AltlParam  ); }
	
	//capture control registers
	wxString pcParam = (const wxChar *)m_guiGateZ80->getLogicParam("PC").c_str();
	wxString spParam = (const wxChar *)m_guiGateZ80->getLogicParam("SP").c_str();
	wxString iParam = (const wxChar *)m_guiGateZ80->getLogicParam("I").c_str();
	wxString imParam = (const wxChar *)m_guiGateZ80->getLogicParam("IMF").c_str();
	wxString iffParam = (const wxChar *)m_guiGateZ80->getLogicParam("IFF1").c_str();
	
	//set control registers in popup
	if (pcParam == (const wxChar *)""){
		setLabel( PCReg, &xxxxString );
		//PCReg->SetLabel( xxString );
	} else { 
		//PCReg->SetLabel( pcParam ); 
		setLabel( PCReg, &pcParam );
	}
	if (spParam == (const wxChar *)""){
		setLabel( SPReg, &xxxxString  );
	} else { setLabel( SPReg, &spParam  ); }
	if (iParam == (const wxChar *)""){
		setLabel( IReg, &xxString  );
	} else { setLabel( IReg, &iParam  ); }
	if (imParam == (const wxChar *)"2") { //2 is the encoding for 1
		wxString mode1 = (const wxChar *)"Mode 1";
		setLabel( IMode, &mode1  );
	}
	else if (imParam == (const wxChar *)"3") { //3 is the encoding for 2
		wxString mode2 = (const wxChar *)"Mode 2";
		setLabel( IMode, &mode2  );
	} else { 
		wxString mode0 = (const wxChar *)"Mode 0";
		setLabel( IMode, &mode0  ); 
	}
	if (iffParam == oneString) {
		wxString enabled = (const wxChar *)"Enabled";
		setLabel( IFFStatus, &enabled  );
	} else { 
		wxString disabled = (const wxChar *)"Disabled";
		setLabel( IFFStatus, &disabled );
	}

	//capture flags
		
	string fParam = m_guiGateZ80->getLogicParam("F").c_str();
	string AltfParam = m_guiGateZ80->getLogicParam("F'").c_str();
	
	if (fParam == "") {
		fParam = "00000000";
	}
	if (AltfParam == "") {
		AltfParam = "00000000";
	}
	
	int FlagReg = parseHex( fParam );
	int AltFlagReg = parseHex( AltfParam );
	
	//set flags in popup
	if ((FlagReg&C_FLAG) == C_FLAG) {
		setLabel( CFlag, &oneString  );
	} else {
		setLabel( CFlag, &zeroString  );
	}
	if ((FlagReg&N_FLAG) == N_FLAG) {
		setLabel( NFlag, &oneString  );
	} else {
		setLabel( NFlag, &zeroString  );
	}
	if ((FlagReg&P_V_FLAG) == P_V_FLAG) {
		setLabel( PVFlag, &oneString  );
	} else {
		setLabel( PVFlag, &zeroString  );
	}
	if ((FlagReg&H_FLAG) == H_FLAG) {
		setLabel( HFlag, &oneString  );
	} else {
		setLabel( HFlag, &zeroString  );
	}
	if ((FlagReg&Z_FLAG) == Z_FLAG) {
		setLabel( ZFlag, &oneString  );
	} else {
		setLabel( ZFlag, &zeroString  );
	}
	if ((FlagReg&S_FLAG) == S_FLAG) {
		setLabel( SFlag, &oneString  );
	} else {
		setLabel( SFlag, &zeroString  );
	}
	
	//set alternate flags in popup
	if ((AltFlagReg&C_FLAG) == C_FLAG) {
		setLabel( AltCFlag, &oneString  );
	} else {
		setLabel( AltCFlag, &zeroString  );
	}
	if ((AltFlagReg&N_FLAG) == N_FLAG) {
		setLabel( AltNFlag, &oneString  );
	} else {
		setLabel( AltNFlag, &zeroString  );
	}
	if ((AltFlagReg&P_V_FLAG) == P_V_FLAG) {
		setLabel( AltPVFlag, &oneString  );
	} else {
		setLabel( AltPVFlag, &zeroString  );
	}
	if ((AltFlagReg&H_FLAG) == H_FLAG) {
		setLabel( AltHFlag, &oneString  );
	} else {
		setLabel( AltHFlag, &zeroString  );
	}
	if ((AltFlagReg&Z_FLAG) == Z_FLAG) {
		setLabel( AltZFlag, &oneString  );
	} else {
		setLabel( AltZFlag, &zeroString  );
	}
	if ((AltFlagReg&S_FLAG) == S_FLAG) {
		setLabel( AltSFlag, &oneString  );
	} else {
		setLabel( AltSFlag, &zeroString  );
	}
	//capture next intruction
	//captures a string object from hash,
	//decodes it using method from Z_80LogicGate,
	//and saves it in currInstr
	wxString currInstr = (const wxChar *)decodeParam(m_guiGateZ80->getLogicParam("INFO_STRING")).c_str();
	
	//set next instruction in popup
	if (currInstr == (const wxChar *)"") {
		CurrentInstruction->SetValue((const wxChar *)"No Instruction");
	} else { CurrentInstruction->SetValue( currInstr ); }
	
	//capture current mode
	
	wxString currMode = (const wxChar *)m_guiGateZ80->getLogicParam("ZAD_MODE").c_str();
	
	//set current mode in popup
	if (currMode == (const wxChar *)"SMART") {
		SMARTMode->SetValue(true);
	}
	else if (currMode == (const wxChar *)"ON") {
		ZADMode->SetValue(true);
	}
	else if (currMode == (const wxChar *)"OFF") {
		REGMode->SetValue(true);
	} else {
		//no change
	}
	
	//capture display
	wxString dispParam = (const wxChar *)m_guiGateZ80->getLogicParam("HEX_DISP").c_str();
	
	//set display in popup
	if (dispParam == (const wxChar *)""){
		wxString zerros = (const wxChar *)"000000";
		setLabel( hexDisp, &zerros  );
	} else { setLabel( hexDisp, &dispParam  ); }
}


//this utility makes it so that the wxStaticText
//gets red text if the value actually chantges
void Z80PopupDialog::setLabel( wxStaticText* who, wxString* what ){
	if( (*what) != who->GetLabel() ){
		who->SetLabel( *what );
		who->SetForegroundColour( *wxRED );
	}
}

//This changes the labels back to black
//for the next round of simulation;
void Z80PopupDialog::blackenLabels(){
	AReg->SetForegroundColour( *wxBLACK );
    BReg->SetForegroundColour( *wxBLACK );
	CReg->SetForegroundColour( *wxBLACK );
	DReg->SetForegroundColour( *wxBLACK );
	EReg->SetForegroundColour( *wxBLACK );
	HReg->SetForegroundColour( *wxBLACK );
	LReg->SetForegroundColour( *wxBLACK );
	IXReg->SetForegroundColour( *wxBLACK );
	IYReg->SetForegroundColour( *wxBLACK );
	
	AltAReg->SetForegroundColour( *wxBLACK );
    AltBReg->SetForegroundColour( *wxBLACK );
	AltCReg->SetForegroundColour( *wxBLACK );
	AltDReg->SetForegroundColour( *wxBLACK );
	AltEReg->SetForegroundColour( *wxBLACK );
	AltHReg->SetForegroundColour( *wxBLACK );
	AltLReg->SetForegroundColour( *wxBLACK );
	
	PCReg->SetForegroundColour( *wxBLACK );
	SPReg->SetForegroundColour( *wxBLACK );
	IReg->SetForegroundColour( *wxBLACK );
	IMode->SetForegroundColour( *wxBLACK );
	IFFStatus->SetForegroundColour( *wxBLACK );
	
	SFlag->SetForegroundColour( *wxBLACK );
	ZFlag->SetForegroundColour( *wxBLACK );
	HFlag->SetForegroundColour( *wxBLACK );
	PVFlag->SetForegroundColour( *wxBLACK );
	NFlag->SetForegroundColour( *wxBLACK );
	CFlag->SetForegroundColour( *wxBLACK );
	
	AltSFlag->SetForegroundColour( *wxBLACK );
	AltZFlag->SetForegroundColour( *wxBLACK );
	AltHFlag->SetForegroundColour( *wxBLACK );
	AltPVFlag->SetForegroundColour( *wxBLACK );
	AltNFlag->SetForegroundColour( *wxBLACK );
	AltCFlag->SetForegroundColour( *wxBLACK );
	
	Refresh(); //get the black to actually work :-P
}

	
int Z80PopupDialog::parseHex( string value ){
	istringstream stream( value );
	int newNum = 0;
	stream >> hex >> newNum;
	return newNum;
}

void Z80PopupDialog::REGModeAction (wxCommandEvent& event) {
	ostringstream oss;
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "ZAD_MODE", "OFF")));
}

void Z80PopupDialog::ZADModeAction (wxCommandEvent& event) {
	ostringstream oss;
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "ZAD_MODE", "ON")));
}

void Z80PopupDialog::SMARTModeAction (wxCommandEvent& event) {
	ostringstream oss;
	gUICircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(m_guiGateZ80->getID(), "ZAD_MODE", "SMART")));
}

BEGIN_EVENT_TABLE(Z80PopupDialog, wxDialog)
	EVT_BUTTON(wxID_CLOSE, Z80PopupDialog::OnBtnClose)
	EVT_BUTTON(ID_CONTSTEP, Z80PopupDialog::StepEdit_CONTINUOUS)
	EVT_BUTTON(ID_ISTEP, Z80PopupDialog::StepEdit_ISTEP)
	EVT_BUTTON(ID_TSTEP, Z80PopupDialog::StepEdit_TSTEP)
	EVT_BUTTON(ID_EDITREG, Z80PopupDialog::EditReg)
	EVT_BUTTON(ID_KEY_0, Z80PopupDialog::KPButton0_Action)
	EVT_BUTTON(ID_KEY_1, Z80PopupDialog::KPButton1_Action)
	EVT_BUTTON(ID_KEY_2, Z80PopupDialog::KPButton2_Action)
	EVT_BUTTON(ID_KEY_3, Z80PopupDialog::KPButton3_Action)
	EVT_BUTTON(ID_KEY_4, Z80PopupDialog::KPButton4_Action)
	EVT_BUTTON(ID_KEY_5, Z80PopupDialog::KPButton5_Action)
	EVT_BUTTON(ID_KEY_6, Z80PopupDialog::KPButton6_Action)
	EVT_BUTTON(ID_KEY_7, Z80PopupDialog::KPButton7_Action)
	EVT_BUTTON(ID_KEY_8, Z80PopupDialog::KPButton8_Action)
	EVT_BUTTON(ID_KEY_9, Z80PopupDialog::KPButton9_Action)
	EVT_BUTTON(ID_KEY_A, Z80PopupDialog::KPButtonA_Action)
	EVT_BUTTON(ID_KEY_B, Z80PopupDialog::KPButtonB_Action)
	EVT_BUTTON(ID_KEY_C, Z80PopupDialog::KPButtonC_Action)
	EVT_BUTTON(ID_KEY_D, Z80PopupDialog::KPButtonD_Action)
	EVT_BUTTON(ID_KEY_E, Z80PopupDialog::KPButtonE_Action)
	EVT_BUTTON(ID_KEY_F, Z80PopupDialog::KPButtonF_Action)
	EVT_BUTTON(ID_KEY_RESET, Z80PopupDialog::KPButtonReset_Action)
	EVT_BUTTON(ID_KEY_GO,    Z80PopupDialog::KPButtonGo_Action)
	EVT_BUTTON(ID_KEY_NEXT,  Z80PopupDialog::KPButtonNext_Action)
	EVT_BUTTON(ID_KEY_PREV,  Z80PopupDialog::KPButtonPrev_Action)
	EVT_BUTTON(ID_KEY_STEP,  Z80PopupDialog::KPButtonStep_Action)
	EVT_RADIOBUTTON(ID_REGMODE, Z80PopupDialog::REGModeAction)
	EVT_RADIOBUTTON(ID_ZADMODE, Z80PopupDialog::ZADModeAction)
	EVT_RADIOBUTTON(ID_SMARTMODE, Z80PopupDialog::SMARTModeAction)
END_EVENT_TABLE()

