#ifndef RAMPOPUPDIALOG_H_
#define RAMPOPUPDIALOG_H_

#include <wx/grid.h>

// Pedro Casanova (casanova@ujaen.es) 2020/04-10
// Change dimensions
#define RAM_TITLE "Ram Info"
#define RAM_WIDTH 700
#define RAM_HEIGHT 500
#define LIST_WIDTH 180
#define LIST_HEIGHT 600
#define RAM_X_POS 20
#define RAM_Y_POS 30


#include "MainApp.h"

class guiGateRAM;
class GUICircuit;

//This class is the special pop-up window that comes up
//when a RAM chip is double clicked.
class RamPopupDialog : public wxDialog{
public:
 	/** Constructor. Creates a new RamPopupDialog */
	// Pedro Casanova (casanova@ujaen.es) 2020/04-10
	// bitsInAddress eliminated, got it from paramaeters
	RamPopupDialog( guiGateRAM* newM_ramGuiGate, GUICircuit* newGUICircuit );


	void OnBtnClose( wxCommandEvent& event );
	void OnBtnLoad( wxCommandEvent& event );
	void OnBtnSave( wxCommandEvent& event );
	// Pedro Casanova (casanova@ujaen.es) 2020/04-10
	// Added Clear Button
	void OnBtnClear( wxCommandEvent& event );
	void OnBtnEdit( wxCommandEvent& event );
	void OnChkBox( wxCommandEvent& event );
	void OnSize();
	wxString OnGetItemText(long item, long column) const;
	
	//This is called by the guiGateRAM when an item changes
	void updateGridDisplay( );
	void notifyAllChanged();

protected:
	DECLARE_EVENT_TABLE()

private:
    GUICircuit* gUICircuit;
    guiGateRAM* m_guiGateRAM;
    wxGrid* memContents;
    wxButton* closeBtn;
    wxButton* loadBtn;
    wxButton* saveBtn;
	// Pedro Casanova (casanova@ujaen.es) 2020/04-10
	// Added Clear Button
	wxButton* clearBtn;
    wxCheckBox* hexOrDecCB;
    
};

//*******************************************************************************
//virtualGrid class

class virtualGrid : public wxGridTableBase {
public:
	
	virtualGrid (int addressSize, int dataSize, guiGateRAM* newM_ramGuiGate, GUICircuit* newGUICircuit, wxCheckBox* hexOrDecCBArg);
	
	virtual int GetNumberRows ();
	virtual int GetNumberCols ();
	virtual bool IsEmptyCell (int row, int col);
	virtual wxString GetValue (int row, int col);
	virtual void SetValue (int row, int col, const wxString& value);
	virtual wxGridCellAttr* GetAttr(int row, int col, wxGridCellAttr::wxAttrKind kind);
	virtual void SetAttr(wxGridCellAttr* attr, int row, int col);
	virtual wxString GetRowLabelValue(int row);
	virtual wxString GetColLabelValue(int col);
	
	void SetNumRows (int numRows);
	void SetNumCols (int numCols);
	
private:
	
	
	guiGateRAM* m_guiGateRAM;
	GUICircuit* gUICircuit;
	
	wxCheckBox* hexOrDecCB;
	
    int addressSize;
    int dataSize;
    
};

#endif /*RAMPOPUPDIALOG_H_*/
