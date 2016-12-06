/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   MainFrame: Main frame object
*****************************************************************************/

#ifndef MAINFRAME_H_
#define MAINFRAME_H_

#include "MainApp.h"
#include "PaletteFrame.h"
#include "wx/wxprec.h"
#include "wx/thread.h"
#include "wx/toolbar.h"
#include "wx/gbsizer.h"
#include "wx/notebook.h"
#include "wx/aui/auibook.h"
#include "wx/slider.h"
#include "threadLogic.h"
#include "GUICanvas.h"
#include "GUICircuit.h"
//#include "OscopeFrame.h"
class OscopeFrame;
#include "klsMiniMap.h"
#include "autoSaveThread.h"

enum
{
	File_Export = 5901, // out of range of wxWidgets constants
	File_ClipCopy,
	
	View_Oscope,
	View_Gridline,
	View_WireConn,
	
    TIMER_ID,
    IDLETIMER_ID,
    TOOLBAR_ID,
    NOTEBOOK_ID,
    
    Tool_Pause,
    Tool_Step,
    Tool_ZoomIn,
    Tool_ZoomOut,
    Tool_Lock,
	Tool_NewTab,
	Tool_DeleteTab
};

class MainFrame : public wxFrame {
public:
    // ctor(s)
    MainFrame(const wxString& title, string cmdFilename = "");
	virtual ~MainFrame();
	
    // event handlers (these functions should _not_ be virtual)
    void OnClose(wxCloseEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnHelpContents(wxCommandEvent& event);
    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
	void OnExportBitmap(wxCommandEvent& event);
	void OnCopyToClipboard(wxCommandEvent& event);
	void OnTimer(wxTimerEvent& event);
	void OnIdle(wxTimerEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnNotebookPage(wxAuiNotebookEvent& event);
	void OnMaximize(wxMaximizeEvent& event);
	void OnUndo(wxCommandEvent& event);
	void OnRedo(wxCommandEvent& event);
	void OnCopy(wxCommandEvent& event);
	void OnPaste(wxCommandEvent& event);	
	void OnOscope(wxCommandEvent& event);
	void OnViewGridline(wxCommandEvent& event);
	void OnViewWireConn(wxCommandEvent& event);
	void OnPause(wxCommandEvent& event);
	void OnStep(wxCommandEvent& event);
	void OnZoomIn(wxCommandEvent& event);
	void OnZoomOut(wxCommandEvent& event);
	void OnTimeStepModSlider(wxScrollEvent& event);
	void OnLock(wxCommandEvent& event);
	void OnNewTab(wxCommandEvent& event);
	void OnDeleteTab(wxAuiNotebookEvent& event);
	
	void saveSettings( void );
	
	void ResumeExecution ( void );
	
	void PauseSim( void );
	
	void loadCircuitFile( string fileName );


	//Julian: Added functions to help with auto save functionality
	void autosave();
	bool fileIsDirty();
	void removeTempFile();
	bool isHandlingEvent();
	void lock();
	void unlock();
	void save(string filename);
	void load(string filename);

	//Julian: Added to simplify exporting and copying to clipboard
	wxBitmap getBitmap(bool withGrid);
	
private:
    // helper function - creates a new thread (but doesn't run it)
	threadLogic *CreateThread();
	autoSaveThread *CreateSaveThread(); //Julian
	

	vector< GUICanvas* > canvases;
	GUICircuit* gCircuit;
	GUICanvas* currentCanvas;
	klsMiniMap* miniMap;
	
	wxCommandProcessor* commandProcessor;

	wxPanel* mainPanel;
	wxToolBar* toolBar;

	//JV - Changed to AuiNoteBook to allow for close tab button
	wxAuiNotebook* canvasBook;
	
	// Instance variables
	bool sizeChanged;
	bool doOpenFile;
	wxString lastDirectory;
	wxString openedFilename;
	unsigned int currentTempNum;

	bool handlingEvent; //Julian: Prevents autosaving from occuring during an open/new/saveas/etc...
	const string CRASH_FILENAME = "crashfile.temp"; //Julian: Filename to check.
	
	wxSlider* timeStepModSlider;
	wxStaticText* timeStepModVal;
	PaletteFrame* gatePalette;
	
	wxBoxSizer* mainSizer;
	
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

#endif /*MAINFRAME_H_*/
