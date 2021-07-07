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

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Select user library
	Select_Library,

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	//File_ClipCopy,
	Copy_Color,
	Copy_Greyscale,
	Copy_Monochrome,
	
	View_Oscope,
	View_Gridline,
	View_WireConn,
	View_WideOutline,

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	View_ComponentColl,
	Adjust_Bitmap,
	Mark_Deprecated,
	
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
	Tool_DeleteTab,

	Help_ReportABug,
	Help_RequestAFeature,
	Help_DownloadLatestVersion
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
	// Pedro Casanova (casanova@ujaen.es) 2020705
	// Select user library
	void OnSelectLibrary(wxCommandEvent& event);
	// Pedro Casanova (casanova@ujaen.es) 2020705
	// Moved to edit menu
	//void OnCopyToClipboard(wxCommandEvent& event);
	void OnTimer(wxTimerEvent& event);
	void OnIdle(wxTimerEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnMove(wxMoveEvent& event);
	void OnNotebookPage(wxAuiNotebookEvent& event);
	void OnMaximize(wxMaximizeEvent& event);
	void OnUndo(wxCommandEvent& event);
	void OnRedo(wxCommandEvent& event);
	void OnCopy(wxCommandEvent& event);
	void OnPaste(wxCommandEvent& event);
	// Pedro Casanova(casanova@ujaen.es) 2020/04-12
	// Separate options to copy bitmap to clipboard
	void OnCopyColor(wxCommandEvent& event);
	void OnCopyGreyscale(wxCommandEvent& event);
	void OnCopyMonochrome(wxCommandEvent& event);
	void OnOscope(wxCommandEvent& event);
	void OnViewGridline(wxCommandEvent& event);
	void OnViewWireConn(wxCommandEvent& event);
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// To show/hide Wide Outliones
	void OnViewWideOutline(wxCommandEvent& event);
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// To show/hide components collisions
	void OnViewComponentCollision(wxCommandEvent& event);
	void OnAdjustBitmap(wxCommandEvent& event);
	void OnMarkDeprecated(wxCommandEvent& event);
	void OnPause(wxCommandEvent& event);
	void OnStep(wxCommandEvent& event);
	void OnZoomIn(wxCommandEvent& event);
	void OnZoomOut(wxCommandEvent& event);
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Any Slider scroll
	void OnSlider(wxScrollEvent& event);
	void OnLock(wxCommandEvent& event);
	void OnNewTab(wxCommandEvent& event);
	void OnDeleteTab(wxAuiNotebookEvent& event);
	void OnReportABug(wxCommandEvent& event);
	void OnRequestAFeature(wxCommandEvent& event);
	void OnDownloadLatestVersion(wxCommandEvent& event);
	
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Settings now in windows register
	void saveSettingsFile(void);
	void saveSettingsReg(void);
	void saveSettings(bool Reg);
	
	void ResumeExecution ( void );
	
	void PauseSim( void );
	
	void loadCircuitFile( string fileName );

	//Julian: Added to simplify timer use
	void stopTimers();
	void startTimers(int at);
	void pauseTimers();
	void resumeTimers(int at);

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
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12		Added color
	wxBitmap getBitmap(bool withGrid, bool color = true);
	
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
	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	wxMenuBar* menuBar;

	//Julian: Re-added timers to fix refresh error
	wxTimer* simTimer;
	wxTimer* idleTimer;

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

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Slider to select wireConnRadius
	wxSlider* wireConnRadiusSlider;
	wxStaticText* wireConnRadiusVal;

	PaletteFrame* gatePalette;
	
	wxBoxSizer* mainSizer;

	bool cancelSave;
	
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

#endif /*MAINFRAME_H_*/
