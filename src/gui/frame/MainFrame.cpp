/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   MainFrame: Main frame object
*****************************************************************************/

#include "MainFrame.h"
#include "wx/filedlg.h"
#include "wx/timer.h"
#include "wx/wfstream.h"
#include "wx/image.h"
#include "wx/thread.h"
#include "wx/toolbar.h"
#include "wx/clipbrd.h"
#include "wx/dataobj.h"
#include "wx/docview.h"

#include "../MainApp.h"
#include "../parse/CircuitParse.h"
#include "OscopeFrame.h"
#include "../commands.h"
#include "../thread/autoSaveThread.h"
#include "../../version.h"
#include "gui/dialog/ColorSettingsDialog.h"

DECLARE_APP(MainApp)

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_EXIT,  MainFrame::OnQuit)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
    EVT_MENU(wxID_HELP_CONTENTS, MainFrame::OnHelpContents)
    EVT_MENU(wxID_NEW, MainFrame::OnNew)
    EVT_MENU(wxID_OPEN, MainFrame::OnOpen)
    EVT_MENU(wxID_SAVE, MainFrame::OnSave)
    EVT_MENU(wxID_SAVEAS, MainFrame::OnSaveAs)
	EVT_MENU(File_Export, MainFrame::OnExportBitmap)
	EVT_MENU(File_ClipCopy, MainFrame::OnCopyToClipboard)
	EVT_MENU(File_Apply, MainFrame::OnApply)
	EVT_MENU(File_ApplyAll, MainFrame::OnApplyAll)
	EVT_MENU(File_Cancel, MainFrame::OnCancel)
	
	EVT_MENU(wxID_UNDO, MainFrame::OnUndo)
	EVT_MENU(wxID_REDO, MainFrame::OnRedo)
	EVT_MENU(wxID_COPY, MainFrame::OnCopy)
	EVT_MENU(wxID_PASTE, MainFrame::OnPaste)
	EVT_MENU(wxID_CUT, MainFrame::OnCut)
	
    EVT_MENU(View_Oscope, MainFrame::OnOscope)
    EVT_MENU(View_Gridline, MainFrame::OnViewGridline)
    EVT_MENU(View_WireConn, MainFrame::OnViewWireConn)
	EVT_MENU(View_Colors, MainFrame::OnEditColors)
    
	EVT_TOOL(Tool_Pause, MainFrame::OnPause)
	EVT_TOOL(Tool_Step, MainFrame::OnStep)
	EVT_TOOL(Tool_ZoomIn, MainFrame::OnZoomIn)
	EVT_TOOL(Tool_ZoomOut, MainFrame::OnZoomOut)
	EVT_SCROLL(MainFrame::OnTimeStepModSlider)
	EVT_TOOL(Tool_Lock, MainFrame::OnLock)
	EVT_TOOL(Tool_NewTab, MainFrame::OnNewTab)
	EVT_TOOL(Tool_BlackBox, MainFrame::OnBlackBox)
	EVT_MENU(Tool_AutoIncrement, MainFrame::OnAutoIncrement)

	EVT_MENU(Help_ReportABug, MainFrame::OnReportABug)
	EVT_MENU(Help_RequestAFeature, MainFrame::OnRequestAFeature)
	EVT_MENU(Help_DownloadLatestVersion, MainFrame::OnDownloadLatestVersion)
	
    //EVT_SIZE(MainFrame::OnSize)
    //EVT_MAXIMIZE(MainFrame::OnMaximize)
    
	EVT_TIMER(TIMER_ID, MainFrame::OnTimer)
	EVT_TIMER(IDLETIMER_ID, MainFrame::OnIdle)

	EVT_AUINOTEBOOK_PAGE_CHANGED(NOTEBOOK_ID, MainFrame::OnNotebookPage)
	EVT_AUINOTEBOOK_PAGE_CLOSE(NOTEBOOK_ID, MainFrame::OnDeleteTab)
	
	EVT_CLOSE(MainFrame::OnClose)
END_EVENT_TABLE()

#define ID_TEXTCTRL 5001

MainFrame::MainFrame(const wxString& title, string cmdFilename, MainFrame *parent, wxSize size)
       : wxFrame(parent, wxID_ANY, title, wxDefaultPosition, size)
{
	threadLogic *thread = CreateThread();
    // set the frame icon
    //SetIcon(wxICON(sample));
	currentCanvas = nullptr;
	isBlackBox = (parent != nullptr);

	// Set default locations
	if (wxGetApp().appSettings.lastDir == "") lastDirectory = wxGetHomeDir();
	else lastDirectory = wxGetApp().appSettings.lastDir;  // added cast KAS

	//////////////////////////////////////////////////////////////////////////
    // create a menu bar
	//////////////////////////////////////////////////////////////////////////
    wxMenu *fileMenu = buildFileMenu(); // FILE MENU
	
	wxMenu *editMenu = buildEditMenu(); // EDIT MENU

    wxMenu *viewMenu = buildViewMenu(); // VIEW MENU

    wxMenu *helpMenu = buildHelpMenu(); // HELP MENU
    
	
    // now append the freshly created menu to the menu bar...
    menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(editMenu, "&Edit");
    menuBar->Append(viewMenu, "&View");
    menuBar->Append(helpMenu, "&Help");
    
    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
    
	//////////////////////////////////////////////////////////////////////////
    // parse a gate library
	//////////////////////////////////////////////////////////////////////////

#ifndef _PRODUCTION_
	string libPath = wxGetApp().pathToExe + "res/cl_gatedefs.xml";
	//WARNING( "just so you know argv[0] == " );
	//WARNING( wxString(wxGetApp().argv[0]) );
#else
	string libPath = wxGetApp().appSettings.gateLibFile;
#endif

	if (!isBlackBox) {
		GateLibrary newLib(libPath);

		newLib.defineBlackBox(R"====(creategate 2 DA_FROM 30 -15
setparams 2 1,1 angle 0.0	JUNCTION_ID $IN1	
creategate 4 AA_AND2 37 -16
setparams 4 1,1 angle 0.0	INPUT_BITS 2	
creategate 5 AA_AND2 37 -21
setparams 5 1,1 angle 0.0	INPUT_BITS 2	
creategate 6 AA_AND2 45 -18.5
setparams 6 1,1 angle 0.0	INPUT_BITS 2	
creategate 7 DA_FROM 30 -17
setparams 7 1,1 angle 0.0	JUNCTION_ID BAD	
creategate 8 DA_FROM 30 -20
setparams 8 1,1 angle 0.0	JUNCTION_ID $IN2	
creategate 9 DA_FROM 30 -22
setparams 9 1,1 angle 0.0	JUNCTION_ID $IN3	
creategate 11 DE_TO 51 -18.5
setparams 11 1,1 angle 0.0	JUNCTION_ID $OUT1	
createwire 1 connectwire 1 4 OUT connectwire 1 6 IN_0
movewire 1 vsegment 0 41,-17.5,41,-16 isect -17.5 1 isect -16 2 hsegment 1 41,-17.5,42,-17.5 connection 6 IN_0 isect 41 0 hsegment 2 40,-16,41,-16 connection 4 OUT isect 41 0  done 
createwire 2 connectwire 2 6 IN_1 connectwire 2 5 OUT
movewire 2 vsegment 0 41,-21,41,-19.5 isect -21 1 isect -19.5 2 hsegment 1 40,-21,41,-21 connection 5 OUT isect 41 0 hsegment 2 41,-19.5,42,-19.5 connection 6 IN_1 isect 41 0  done 
createwire 3 connectwire 3 2 IN_0 connectwire 3 4 IN_0
connectwire 3 4 IN_1
connectwire 3 7 IN_0
movewire 3 hsegment 1 32,-15,34,-15 connection 4 IN_0 connection 2 IN_0 isect 33 8 vsegment 8 33,-17,33,-15 isect -17 10 isect -15 1 hsegment 10 32,-17,34,-17 connection 4 IN_1 connection 7 IN_0 isect 33 8  done 
createwire 5 connectwire 5 8 IN_0 connectwire 5 5 IN_0
movewire 5 hsegment 1 32,-20,34,-20 connection 5 IN_0 connection 8 IN_0  done 
createwire 6 connectwire 6 9 IN_0 connectwire 6 5 IN_1
movewire 6 hsegment 1 32,-22,34,-22 connection 5 IN_1 connection 9 IN_0  done 
createwire 7 connectwire 7 6 OUT connectwire 7 11 IN_0
movewire 7 hsegment 1 48,-18.5,49,-18.5 connection 6 OUT connection 11 IN_0  done 
)====");

		wxGetApp().libParser = newLib;
	}
	
	//////////////////////////////////////////////////////////////////////////
    // create a toolbar
	//////////////////////////////////////////////////////////////////////////
	toolBar = buildToolBar();

    CreateStatusBar(2);
    SetStatusText("");

	mainSizer = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer* leftPaneSizer = new wxBoxSizer( wxVERTICAL );
	
	// now a gate palette for the library
	gatePalette = new PaletteFrame(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	leftPaneSizer->Add( gatePalette, wxSizerFlags(1).Expand().Border(wxALL, 0) );
	leftPaneSizer->Show( gatePalette );
	miniMap = new klsMiniMap(this, wxID_ANY, wxDefaultPosition, wxSize(130, 100));
	leftPaneSizer->Add( miniMap, wxSizerFlags(0).Expand().Border(wxALL, 0) );
	mainSizer->Add( leftPaneSizer, wxSizerFlags(0).Expand().Border(wxALL, 0) );
	
	// set up the panel and make canvases
	gCircuit = new GUICircuit();
	commandProcessor = new wxCommandProcessor();
	gCircuit->setCommandProcessor(commandProcessor);
	commandProcessor->SetEditMenu(editMenu);
	commandProcessor->Initialize();

	GUICanvas* canvas;

	if (!isBlackBox) {
		canvasBook = new wxAuiNotebook(this, NOTEBOOK_ID, wxDefaultPosition, wxSize(400, 400), wxAUI_NB_CLOSE_ON_ACTIVE_TAB | wxAUI_NB_SCROLL_BUTTONS);
		mainSizer->Add(canvasBook, wxSizerFlags(1).Expand().Border(wxALL, 0));

		//add 1 tab: Left loop to allow for different default
		for (int i = 0; i < 1; i++) {
			canvases.push_back(new GUICanvas(canvasBook, gCircuit, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS));
			ostringstream oss;
			oss << "Page " << (i + 1);
			canvasBook->AddPage(canvases[i], oss.str(), (i == 0 ? true : false));  // KAS
		}
		currentCanvas = canvases[0];
	}
	else {
		canvas = new GUICanvas(this, gCircuit, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS);
		mainSizer->Add(canvas, wxSizerFlags(1).Expand().Border(wxALL, 0));
		currentCanvas = canvas;
	}

	gCircuit->setCurrentCanvas(currentCanvas);
	currentCanvas->setMinimap(miniMap);

	if (!isBlackBox) {
		mainSizer->Show(canvasBook);
	}
	else {
		mainSizer->Show(canvas);
	}
	
	currentCanvas->SetFocus();

	SetSizer( mainSizer);
		
	
	gCircuit->setLogicThread(thread);
	
    if ( thread->Run() != wxTHREAD_NO_ERROR )
    {
       wxLogError("Can't start thread!");
    }
	
	simTimer = new wxTimer(this, TIMER_ID);
	idleTimer = new wxTimer(this, IDLETIMER_ID);
	stopTimers();
	startTimers(20);

	// Setup the "Maximize Catch" flag:
	sizeChanged = false;
	
	gCircuit->setOscope(new OscopeFrame(this, "O-Scope", gCircuit));
	
	toolBar->Realize();
	
	if (!isBlackBox) {
		this->SetSize(wxGetApp().appSettings.mainFrameLeft, wxGetApp().appSettings.mainFrameTop, wxGetApp().appSettings.mainFrameWidth, wxGetApp().appSettings.mainFrameHeight);

		doOpenFile = (cmdFilename.size() > 0);
		this->openedFilename = cmdFilename;

		if (ifstream(CRASH_FILENAME)) {
			wxMessageDialog dialog(this, "Oops! It seems like there may have been a crash.\nWould you like to try to recover your work?", "Recover File", wxYES_DEFAULT | wxYES_NO | wxICON_QUESTION);
			if (dialog.ShowModal() == wxID_YES)
			{
				doOpenFile = false;
				openedFilename = "Recovered File";
				load(CRASH_FILENAME);
				this->SetTitle(VERSION_TITLE() + " - " + openedFilename);
			}
			removeTempFile();
		}

		autoSaveThread *autoThread = CreateSaveThread();
		if (autoThread->Run() != wxTHREAD_NO_ERROR)
		{
			wxLogError("Autosave thread not started!");
		}	
	}

	updateMenuOptions();

	handlingEvent = false;
	wxInitAllImageHandlers(); //Julian: Added to allow saving all types of image files

	// Colin: for testing dynamic gates
	//DynamicGate* dg = new DynamicGate(currentCanvas, gCircuit, gCircuit->getNextAvailableGateID(), 3, 0, 0, "AND");
}

wxMenu* MainFrame::buildFileMenu() {
	wxMenu *fileMenu = new wxMenu;

	if (!isBlackBox) {
		fileMenu->Append(wxID_NEW, "&New\tCtrl+N", "Create new circuit");
		fileMenu->Append(wxID_OPEN, "&Open\tCtrl+O", "Open circuit");
		fileMenu->Append(wxID_SAVE, "&Save\tCtrl+S", "Save circuit");
		fileMenu->Append(wxID_SAVEAS, "Save &As", "Save circuit");
		
	} else {
		fileMenu->Append(File_Apply, "&Apply", "Apply to this instance");
		fileMenu->Append(File_ApplyAll, "&Apply All", "Apply to this black box type");
		fileMenu->Append(File_Cancel, "&Cancel", "Cancel changes");
	}

	fileMenu->AppendSeparator();
	fileMenu->Append(File_Export, "Export to Image");
	fileMenu->Append(File_ClipCopy, "Copy Canvas to Clipboard");
	fileMenu->AppendSeparator();

	if (!isBlackBox) {
		fileMenu->Append(wxID_EXIT, "E&xit\tAlt+X", "Quit this program");
	} else {
		fileMenu->Append(wxID_EXIT, "&Close", "Close black box editor");
	}

	return fileMenu;
}
wxMenu* MainFrame::buildEditMenu() {
	wxMenu *editMenu = new wxMenu;
	editMenu->Append(wxID_UNDO, "Undo\tCtrl+Z", "Undo last operation");
	editMenu->Append(wxID_REDO, "Redo", "Redo last operation");
	editMenu->AppendSeparator();

	if (!isBlackBox) {
		editMenu->Append(Tool_NewTab, "New Tab\tCtrl+T", "New Tab");
		editMenu->AppendSeparator();
	}

	editMenu->Append(wxID_CUT, "Cut\tCtrl+X", "Cut selection to clipboard");
	editMenu->Append(wxID_COPY, "Copy\tCtrl+C", "Copy selection to clipboard");
	editMenu->Append(wxID_PASTE, "Paste\tCtrl+V", "Paste selection from clipboard");
	
	editMenu->AppendSeparator();

	editMenu->AppendCheckItem(Tool_AutoIncrement, "Auto Increment Labels", "Automatically increment label (To/From) subscripts");

	return editMenu;
}
wxMenu* MainFrame::buildViewMenu() {
	wxMenu* viewMenu = new wxMenu;
	viewMenu->Append(View_Oscope, "&Oscope\tCtrl+G", "Show the Oscope");
	wxMenu *settingsMenu = new wxMenu;
	settingsMenu->AppendCheckItem(View_Gridline, "Display Gridlines", "Toggle gridline display");
	settingsMenu->AppendCheckItem(View_WireConn, "Display Wire Connection Points", "Toggle wire connection points");
	settingsMenu->AppendSeparator();
	settingsMenu->Append(View_Colors, "Color Settings", "Color Settings");
	viewMenu->AppendSeparator();
	viewMenu->AppendSubMenu(settingsMenu, "Settings");
	return viewMenu;
}
wxMenu* MainFrame::buildHelpMenu() {
	wxMenu* helpMenu = new wxMenu;
	helpMenu->Append(wxID_HELP_CONTENTS, "&Contents...\tF1", "Show Help system");
	helpMenu->AppendSeparator();
	helpMenu->Append(Help_ReportABug, "Report a bug...");
	helpMenu->Append(Help_RequestAFeature, "Request a feature...");
	helpMenu->Append(Help_DownloadLatestVersion, "Download latest version...");
	helpMenu->AppendSeparator();
	helpMenu->Append(wxID_ABOUT, "&About...", "Show about dialog");
	return helpMenu;
}
wxToolBar* MainFrame::buildToolBar() {
	wxToolBar* tools = new wxToolBar(this, TOOLBAR_ID, wxPoint(0, 0), wxDefaultSize, wxTB_HORIZONTAL | wxNO_BORDER | wxTB_FLAT);

	// formerly, we were using a resource file to associate the toolbar bitmaps to the program.  I modified the code
	// to read the bitmaps from file directly, without the use of a resource file.  KAS
	string    bitmaps[] = { "new", "open", "save", "undo", "redo", "cut", "copy", "paste", "print", "help", "pause", "step", "zoomin", "zoomout", "locked", "newtab", "blackbox", "apply", "applyall", "cancel", "autoinc"};
	//						 0		1		2		3		4		5	   6	   7		8		 9		 10		  11	  12		13		   14		 15		   16		   17		18			19		  20
	wxBitmap *bmp[21];

	for (int i = 0; i < 21; i++) {
		bitmaps[i] = "res/bitmaps/" + bitmaps[i] + ".bmp";
		wxFileInputStream in(bitmaps[i]);
		bmp[i] = new wxBitmap(wxImage(in, wxBITMAP_TYPE_BMP));
	}

	int w = bmp[0]->GetWidth(),
		h = bmp[0]->GetHeight();
	tools->SetToolBitmapSize(wxSize(w, h));

	if (!isBlackBox) {
		tools->AddTool(wxID_NEW, "New", *bmp[0], "New");
		tools->AddTool(wxID_OPEN, "Open", *bmp[1], "Open");
		tools->AddTool(wxID_SAVE, "Save", *bmp[2], "Save");
		tools->AddTool(Tool_NewTab, "New Tab", *bmp[15], "New Tab");
	} else {
		tools->AddTool(File_Apply, "Apply", *bmp[17], "Apply");
		tools->AddTool(File_ApplyAll, "Apply All", *bmp[18], "Apply All");
		tools->AddTool(File_Cancel, "Cancel", *bmp[19], "Cancel");
	}
	tools->AddSeparator();
	tools->AddTool(wxID_UNDO, "Undo", *bmp[3], "Undo");
	tools->AddTool(wxID_REDO, "Redo", *bmp[4], "Redo");
	tools->AddSeparator();
	tools->AddTool(wxID_CUT, "Cut", *bmp[5], "Cut");
	tools->AddTool(wxID_COPY, "Copy", *bmp[6], "Copy");
	tools->AddTool(wxID_PASTE, "Paste", *bmp[7], "Paste");
	tools->AddSeparator();
	tools->AddTool(Tool_ZoomIn, "Zoom In", *bmp[12], "Zoom In");
	tools->AddTool(Tool_ZoomOut, "Zoom Out", *bmp[13], "Zoom Out");
	tools->AddSeparator();
	tools->AddTool(Tool_Pause, "Pause/Resume", *bmp[10], "Pause/Resume", wxITEM_CHECK);
	tools->AddTool(Tool_Step, "Step", *bmp[11], "Step");
	timeStepModSlider = new wxSlider(tools, wxID_ANY, wxGetApp().timeStepMod, 1, 500, wxDefaultPosition, wxSize(125, -1), wxSL_HORIZONTAL | wxSL_AUTOTICKS);
	ostringstream oss;
	oss << wxGetApp().timeStepMod << "ms";
	timeStepModVal = new wxStaticText(tools, wxID_ANY, oss.str(), wxDefaultPosition, wxSize(45, -1), wxSUNKEN_BORDER | wxALIGN_RIGHT | wxST_NO_AUTORESIZE);  // added cast KAS
	tools->AddControl(timeStepModSlider);
	tools->AddControl(timeStepModVal);
	tools->AddSeparator();
	tools->AddCheckTool(Tool_Lock, "Lock state", *bmp[14], wxNullBitmap, "Lock state");
	tools->AddSeparator();
	tools->AddCheckTool(Tool_AutoIncrement, "Toggle auto increment", *bmp[20], wxNullBitmap, "Toggle auto increment");
	if (!isBlackBox) {
		tools->AddTool(Tool_BlackBox, "Black Box", *bmp[16], "Black Box");
	}
	tools->AddSeparator();
	tools->AddTool(wxID_ABOUT, "About", *bmp[9], "About");
	SetToolBar(tools);
	tools->Show(true);

	//finished with the bitmaps, so we can release the pointers  KAS
	for (int i = 0; i < 21; i++) {
		delete bmp[i];
	}

	return tools;
}

MainFrame::~MainFrame() {

	stopTimers();

	logicThread->Delete();
	logicThread = NULL;

	if (!isBlackBox) {
		// Shut down the detached thread and wait for it to exit
		
		wxGetApp().saveThread->Delete();

		wxGetApp().m_semAllDone.Wait();

		// Delete the various objects
		delete wxGetApp().helpController;
		wxGetApp().helpController = NULL;

		saveSettings();
	}

	//Edit by Joshua Lansford 10/18/2007.
	//Commented out the delete on the toolbar.
	//wxWidets auto deletes toolBars.  See the destructor for
	//wxFrame.
	//delete toolBar;
	
	delete gCircuit;
	gCircuit = NULL;
	
	//Joshua Lansford Edit 10/18/07
	//Removed the delete of systemTime because it was causeing a
	//crash on close.  In stead, I changed it from a pointer
	//to a local var so that it would not need to be deleted.

	delete simTimer;
	simTimer = NULL;
	delete idleTimer;
	idleTimer = NULL;
}

threadLogic *MainFrame::CreateThread()
{
	threadLogic *thread = new threadLogic();
    if ( thread->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogError("Can't create thread!");
    }

	logicThread = thread;
	
    return thread;
}

autoSaveThread *MainFrame::CreateSaveThread()
{
	autoSaveThread *thread = new autoSaveThread();
	if (thread->Create() != wxTHREAD_NO_ERROR)
	{
		wxLogError("Can't create autosave thread!");
	}

	wxCriticalSectionLocker enter(wxGetApp().m_critsect);
	wxGetApp().saveThread = thread;

	return thread;
}


// event handlers

void MainFrame::OnClose(wxCloseEvent& event) {
	//Edit by Joshua Lansford 10/18/07
	//Calling Destroy is not what was crashing the system.
	//Deleting wxGetApp().appSystemTime in MainFrame::~MainFrame
	//was crashing the system.  This problem was solved by
	//replaceing the pointer appSystemTime with the non pointer
	//appSystemTime.  Now it doesn't need to be deleted, and
	//the system doesn't crash on close.
	
	//If Destroy is replaced with Close, then you get in
	//an infinite loop because Close calls this method we
	//are in right now.

	//This synopsis is wrong... See comment above. ~JEL 10/18/07
	
	// The call to destroy the application window was causing abnormal
	// termination.  I'm not sure why, but I'm guessing that after the
	// window was destroyed, there was another reference to the window
	// object (or one of its children) as the application closed down.
	// I modified this event handler to note the destroy request with
	// the static boolean variable below, and then in the presence of
	// such a request close the window.  This more gentle manner of
	// termination seems to allow time for whatever needs to clean up
	// so that the application terminates normally.  KAS 4/26/07
	static bool destroy = false;
	handlingEvent = true;
	
	pauseTimers();

	// Allow the user to save the file, unless we are in the midst of terminating the app!!, KAS 4/26/07	
	if (commandProcessor->IsDirty() && !destroy && !isBlackBox) {
		wxMessageDialog dialog( this, "Circuit has not been saved.  Would you like to save it?", "Save Circuit", wxYES_DEFAULT|wxYES_NO|wxCANCEL|wxICON_QUESTION);
		switch (dialog.ShowModal()) {
		case wxID_YES:
			OnSave(*((wxCommandEvent*)(&event)));
			destroy = true;  // postpone destruction until wxWidgets cleans up, KAS 4/26/07
			break;
		case wxID_NO:
			destroy = true;  // postpone destruction until wxWidgets cleans up, KAS 4/26/07
			break;
		case wxID_CANCEL:
			if (event.CanVeto()) event.Veto(); else destroy = true;
			break;
		}			
	} else {
		destroy = true;      // postpone destruction until wxWidgets cleans up, KAS 4/26/07
	}
	
	resumeTimers(20);

	if (destroy && !isBlackBox) {
		removeTempFile();
	}
	else {
		handlingEvent = false;
	}

	if (isBlackBox) {
		GetParent()->Enable();
		wxGetApp().SetTopWindow(this->GetParent());
	}

	//Edit by Joshua Lansford 10/18/07
	//KAS replaced the destroy method with a close method.
	//However the close method simply calls the method we
	//are currently in.  This causes an ininitue loop which
	//wxWidgets detects and terminates.
	//While this did remove the crash on close, it did so
	//by makeing the program simple give up before it ever got
	//to the crashing code.  The destructor of the MainFrame
	//never was being called and the save settings function
	//in it never was being called.
	//See 
	//http://www.wxwidgets.org/manuals/stable/wx_windowdeletionoverview.html
	if (destroy) this->Destroy();
}

void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) {
    // true is to force the frame to close, so pass false to allow OnClose to handle
    Close(false);
}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
    wxString msg;
    msg.Printf(wxString(VERSION_ABOUT_TEXT()));

    wxMessageBox(msg, "About", wxOK | wxICON_INFORMATION, this);
}

void MainFrame::OnNew(wxCommandEvent& event) {
	handlingEvent = true;

	if (commandProcessor->IsDirty()) {
		wxMessageDialog dialog( this, "Circuit has not been saved.  Would you like to save it?", "Save Circuit", wxYES_DEFAULT|wxYES_NO|wxCANCEL|wxICON_QUESTION);
		switch (dialog.ShowModal()) {
		case wxID_YES:
			OnSave(event);
			break;
		case wxID_CANCEL:
			return;
		}			
	}

	pauseTimers();

	logicThread->clearAllMessages();

	for (unsigned int i = 0; i < canvases.size(); i++) canvases[i]->clearCircuit();
	gCircuit->reInitializeLogicCircuit();
	commandProcessor->ClearCommands();
	commandProcessor->SetMenuStrings();
	//JV - Added so that new starts with one tab
	for (unsigned int j = canvases.size() - 1; j > 0; j--) {
		canvasBook->DeletePage(j);
		canvases.erase(canvases.end() - 1);
	}
	currentCanvas = canvases[0];
	currentCanvas->Update(); // Render();
	this->SetTitle(VERSION_TITLE()); // KAS
	removeTempFile();
    openedFilename = "";

	resumeTimers(20);

	handlingEvent = false;
}

void MainFrame::OnOpen(wxCommandEvent& event) {
	
	handlingEvent = true;

	currentCanvas->getCircuit()->setSimulate(false);
	if (commandProcessor->IsDirty()) {
		wxMessageDialog dialog( this, "Circuit has not been saved.  Would you like to save it?", "Save Circuit", wxYES_DEFAULT|wxYES_NO|wxCANCEL|wxICON_QUESTION);
		switch (dialog.ShowModal()) {
		case wxID_YES:
			OnSave(event);
			break;
		case wxID_CANCEL:
			currentCanvas->getCircuit()->setSimulate(true);
			handlingEvent = false;
			return;
		}			
	}
	
	pauseTimers();

	wxString caption = "Open a circuit";
	wxString wildcard = "Circuit files (*.cdl)|*.cdl";
	wxString defaultFilename = "";
	wxFileDialog dialog(this, caption, wxEmptyString, defaultFilename, wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	dialog.SetDirectory(lastDirectory);
	
	
	if (dialog.ShowModal() == wxID_OK) {
		lastDirectory = dialog.GetDirectory();
		loadCircuitFile(dialog.GetPath().ToStdString());
	}
	
    currentCanvas->Update(); // Render();
	currentCanvas->getCircuit()->setSimulate(true);

	resumeTimers(20);

	handlingEvent = false;
}
//Edit by Joshua Lansford 2/15/07
//Purpose of edit:  by obstracting the loading of
//circuit files out of the onOpen rutine,
//I can now make it so that if a circuit file
//is specified as an argument to cedarls when
//it starts, that cedarls can load that file
//by calling this method.
void MainFrame::loadCircuitFile( string fileName ){
	wxString path = fileName;
	
	openedFilename = path;
	this->SetTitle(VERSION_TITLE() + " - " + path );
	
	logicThread->clearAllMessages();
	
	for (unsigned int i = 0; i < canvases.size(); i++) canvases[i]->clearCircuit();

	gCircuit->reInitializeLogicCircuit();
	commandProcessor->ClearCommands();
	commandProcessor->SetMenuStrings();

	for (unsigned int j = canvases.size(); j > 0; j--) {
		canvases.pop_back();
		canvasBook->DeletePage(j-1);
	}

	canvases.push_back(new GUICanvas(canvasBook, gCircuit, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS));
	ostringstream oss;
	oss << "Page " << (1);
	canvasBook->AddPage(canvases[0], oss.str(), (true));

	currentCanvas = canvases[0];
	gCircuit->setCurrentCanvas(currentCanvas);
	currentCanvas->setMinimap(miniMap);
	currentCanvas->SetFocus();

	CircuitParse cirp(path.ToStdString(), canvases);
	canvases = cirp.parseFile();

	//JV - Put pages back into canvas book
	for (unsigned int i = 1; i < canvases.size(); i++) 
	{
		ostringstream oss;
		oss << "Page " << (i + 1);
		canvasBook->AddPage(canvases[i], oss.str(), (i == 0 ? true : false));
	}

/*	currentCanvas = canvases[0];
	gCircuit->setCurrentCanvas(currentCanvas);
	currentCanvas->setMinimap(miniMap);
	mainSizer->Show(canvasBook);
	currentCanvas->SetFocus();*/

	removeTempFile();
}

void MainFrame::OnSave(wxCommandEvent& event) {
	if (openedFilename == "") OnSaveAs(event);
	else {
		commandProcessor->MarkAsSaved();
		save((string)openedFilename);
	}
}

void MainFrame::OnSaveAs(wxCommandEvent& WXUNUSED(event)) {
	handlingEvent = true;

	wxString caption = "Save circuit";
	wxString wildcard = "Circuit files (*.cdl)|*.cdl";
	wxString defaultFilename = "";
	wxFileDialog dialog(this, caption, wxEmptyString, defaultFilename, wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	dialog.SetDirectory(lastDirectory);
	if (dialog.ShowModal() == wxID_OK) {
		removeTempFile();
		wxString path = dialog.GetPath();
		openedFilename = path;
		this->SetTitle(VERSION_TITLE() + " - " + path );
		commandProcessor->MarkAsSaved();
		save((string)openedFilename);
	}
	handlingEvent = false;
}

void MainFrame::OnOscope(wxCommandEvent& WXUNUSED(event)) {
	gCircuit->getOscope()->Show(true);
}

void MainFrame::OnViewGridline(wxCommandEvent& event) {
	wxGetApp().appSettings.gridlineVisible = event.IsChecked();
	if (currentCanvas != NULL) currentCanvas->Update();
	updateMenuOptions();
	if (isBlackBox) {
		((MainFrame*)GetParent())->updateMenuOptions();
	}
}

void MainFrame::OnViewWireConn(wxCommandEvent& event) {
	wxGetApp().appSettings.wireConnVisible = event.IsChecked();
	if (currentCanvas != NULL) currentCanvas->Update();
	updateMenuOptions();
	if (isBlackBox) {
		((MainFrame*)GetParent())->updateMenuOptions();
	}
}

void MainFrame::OnAutoIncrement(wxCommandEvent& event) {
	wxGetApp().appSettings.autoIncrement = event.IsChecked();
	updateMenuOptions();
	if (isBlackBox) {
		((MainFrame*)GetParent())->updateMenuOptions();
	}
}

void MainFrame::OnTimer(wxTimerEvent& event) {
	ostringstream oss;
	if (!(currentCanvas->getCircuit()->getSimulate())) {
		return;
	}
	if (wxGetApp().appSystemTime.Time() < wxGetApp().appSettings.refreshRate) return;
	wxGetApp().appSystemTime.Pause();
	if (gCircuit->panic) return;
	// Do function of number of milliseconds that passed since last step
	gCircuit->lastTime = wxGetApp().appSystemTime.Time();
	gCircuit->sendMessageToCore(new Message_STEPSIM(wxGetApp().appSystemTime.Time() / wxGetApp().timeStepMod));
	currentCanvas->getCircuit()->setSimulate(false);
	wxGetApp().appSystemTime.Start(wxGetApp().appSystemTime.Time() % wxGetApp().timeStepMod);
}

void MainFrame::OnIdle(wxTimerEvent& event) {
	//wxCriticalSectionLocker locker(wxGetApp().m_critsect);
	//while (wxGetApp().mexMessages.TryLock() == wxMUTEX_BUSY) wxYield();
	while (logicThread->hasGuiMessage()) {
		Message *message = logicThread->popGuiMessage();
		gCircuit->parseMessage(message);
	}

	if (mainSizer == NULL) return;
	
	if ( !isBlackBox && doOpenFile ) {
		doOpenFile = false;
		load((string)openedFilename);
		this->SetTitle(VERSION_TITLE() + " - " + openedFilename );
	}
	
	if ( gCircuit->panic ) {
		gCircuit->panic = false;
		toolBar->ToggleTool( Tool_Pause, true );
		simTimer->Stop();
		wxGetApp().appSystemTime.Start(0);
		wxGetApp().appSystemTime.Pause();
		//Edit by Joshua Lansford 11/24/06
		//I have overloaded the meaning of panic
		//panic is now also used to pause the system.
		//thus we don't want to shout if we are just pausing
		//This edit was made so that the Z_80LogicGate
		//can 'step' through instructions.
		//see the location were pausing is set to true
		//for further explination in GUICircuit::parseMessage
		if( !gCircuit->pausing ){
			wxMessageBox("Overloading simulator: please increase time per step and then resume simulation.", "Error - overload", wxOK | wxICON_ERROR, NULL);
		}
		gCircuit->pausing = false;
	}

	if( sizeChanged ) {	
		sizeChanged = false;
		wxSizeEvent temp;
	}
}
void MainFrame::OnSize(wxSizeEvent& event) {
	if (currentCanvas != NULL) currentCanvas->Update();
	if (mainSizer != NULL) mainSizer->Layout();
}

void MainFrame::OnMaximize(wxMaximizeEvent& event) {
	// Setup the "Maximize Catch" flag:
	sizeChanged = true;
}

void MainFrame::OnNotebookPage(wxAuiNotebookEvent& event) {
	long canvasID = event.GetSelection();
	if (currentCanvas == NULL || canvases[canvasID] == currentCanvas) return;
	//**********************************
	//Edit by Joshua Lansford 4/9/07
	//This edit is to make the minimap
	//only be controled by the current
	//Canvase.
	//This will avoid the minimap
	//spazing out when the mainFrame is
	//resized
	currentCanvas->setMinimap( NULL );
	//End of Edit*********************
	currentCanvas = canvases[canvasID];
	gCircuit->setCurrentCanvas(currentCanvas);
	currentCanvas->setMinimap(miniMap);
	currentCanvas->SwapBuffers(); //Reduce flashing when using different background colors
	currentCanvas->Update();
	currentCanvas->SetFocus();
}

void MainFrame::OnUndo(wxCommandEvent& event) {
	commandProcessor->Undo();
	currentCanvas->Update();
}

void MainFrame::OnRedo(wxCommandEvent& event) {
	commandProcessor->Redo();
	currentCanvas->Update();
}

void MainFrame::OnCut(wxCommandEvent& event)
{
	currentCanvas->copyBlockToClipboard();
	currentCanvas->deleteSelection();
	currentCanvas->Update();
}

void MainFrame::OnCopy(wxCommandEvent& event) {
	currentCanvas->copyBlockToClipboard();
}

void MainFrame::OnPaste(wxCommandEvent& event) {
	currentCanvas->pasteBlockFromClipboard();
}

void MainFrame::OnExportBitmap(wxCommandEvent& event) {
	bool showGrid = false;
	wxMessageDialog gridDialog(this, "Export with Grid?", "Export", wxYES_DEFAULT | wxYES_NO | wxCANCEL | wxICON_QUESTION);
	switch (gridDialog.ShowModal()) {
	case wxID_YES:
		showGrid = true;
		break;
	case wxID_CANCEL:
		return;
	}

	wxBitmap bitmap = getBitmap(showGrid);

	wxString caption = "Export Circuit";
	wxString wildcard = "PNG (*.png)|*.png|JPEG (*.jpg;*.jpeg)|*.jpg;*.jpeg|Bitmap (*.bmp)|*.bmp";
	wxString defaultFilename = "";
	wxFileDialog saveDialog(this, caption, wxEmptyString, defaultFilename, wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	saveDialog.SetDirectory(lastDirectory);
	if (saveDialog.ShowModal() == wxID_OK) {
		wxString path = saveDialog.GetPath();
		wxBitmapType fileType;
		if (path.SubString(path.find_last_of(".") + 1, path.length()) == "bmp")
		{
			fileType = wxBITMAP_TYPE_BMP;
		}
		else if (path.SubString(path.find_last_of(".") + 1, path.length()) == "png")
		{
			fileType = wxBITMAP_TYPE_PNG;
		}
		else
		{
			fileType = wxBITMAP_TYPE_JPEG;
		}

		bitmap.SaveFile(path,fileType);
	}
	else
	{
		return;
	}
}

void MainFrame::OnCopyToClipboard(wxCommandEvent& event) {
	bool showGrid = false;
	wxMessageDialog gridDialog(this, "Copy with Grid?", "Copy to Clipboard", wxYES_DEFAULT | wxYES_NO | wxCANCEL | wxICON_QUESTION);
	switch (gridDialog.ShowModal()) {
	case wxID_YES:
		showGrid = true;
		break;
	case wxID_CANCEL:
		return;
	}

	wxBitmap bitmap = getBitmap(showGrid);

	if (wxTheClipboard->Open()) {
		wxTheClipboard->SetData(new wxBitmapDataObject(bitmap));
		wxTheClipboard->Close();
	}
}

wxBitmap MainFrame::getBitmap(bool withGrid) {
	bool gridlineVisible = wxGetApp().appSettings.gridlineVisible;
	wxGetApp().appSettings.gridlineVisible = withGrid;
	wxGetApp().doingBitmapExport = true;

	// render the image
	wxSize imageSize = currentCanvas->GetClientSize();
	wxImage circuitImage = currentCanvas->renderToImage(imageSize.GetWidth(), imageSize.GetHeight(), 32);
	wxBitmap circuitBitmap(circuitImage);

	// restore grid display setting
	wxGetApp().appSettings.gridlineVisible = gridlineVisible;
	wxGetApp().doingBitmapExport = false;

	return circuitBitmap;
}

void MainFrame::updateMenuOptions()
{
	menuBar->Check(View_Gridline, wxGetApp().appSettings.gridlineVisible);
	menuBar->Check(View_WireConn, wxGetApp().appSettings.wireConnVisible);
	menuBar->Check(Tool_AutoIncrement, wxGetApp().appSettings.autoIncrement);
	toolBar->FindById(Tool_AutoIncrement)->Toggle(wxGetApp().appSettings.autoIncrement);
	toolBar->Realize();
}

void MainFrame::OnPause(wxCommandEvent& event) {
	PauseSim();
}

void MainFrame::OnStep(wxCommandEvent& event) {
	if (!(currentCanvas->getCircuit()->getSimulate())) {
		return;
	}
	gCircuit->sendMessageToCore(new Message_STEPSIM(1));
	currentCanvas->getCircuit()->setSimulate(false);
}

void MainFrame::OnLock(wxCommandEvent& event) {
	if (toolBar->GetToolState(Tool_Lock)) {
		lock();
	} else {
		unlock();
	}
}

void MainFrame::OnZoomIn(wxCommandEvent& event) {
	//TODO: There is no way to check if currentCanvas is valid first!!!
	currentCanvas->zoomIn();
}

void MainFrame::OnZoomOut(wxCommandEvent& event) {
	//TODO: There is no way to check if currentCanvas is valid first!!!
	currentCanvas->zoomOut();
}

void MainFrame::OnHelpContents(wxCommandEvent& event) {
	wxGetApp().helpController->DisplayContents();
}

void MainFrame::OnTimeStepModSlider(wxScrollEvent& event) {
	ostringstream oss;
	oss << wxGetApp().timeStepMod << "ms";
	wxGetApp().timeStepMod = timeStepModSlider->GetValue();
	timeStepModVal->SetLabel(oss.str()); // KAS
}


void MainFrame::saveSettings() {
	//Edit by Joshua Lansford 2/15/07
	//making the execution of cedarls indipendent of were
	//it was executed from.  However the settings.ini file still
	//needs to be relative.
	//adding substring on the end of the relative paths to knock
	//of the part I put on.
	int numCharAbsolute = wxGetApp().pathToExe.length();
	
	string settingsIni = wxGetApp().pathToExe + "res/settings.ini";
	
	ofstream iniFile(settingsIni, ios::out);
	iniFile << "GateLib=" << wxGetApp().appSettings.gateLibFile.substr(numCharAbsolute) << endl;
	iniFile << "HelpFile=" << wxGetApp().appSettings.helpFile.substr(numCharAbsolute) << endl;
	iniFile << "TextFont=" << wxGetApp().appSettings.textFontFile.substr(numCharAbsolute) << endl;
	iniFile << "FrameWidth=" << this->GetSize().GetWidth() << endl;
	iniFile << "FrameHeight=" << this->GetSize().GetHeight() << endl;
	iniFile << "FrameLeft=" << this->GetPosition().x << endl;
	iniFile << "FrameTop=" << this->GetPosition().y << endl;
	iniFile << "TimeStep=" << wxGetApp().timeStepMod << endl;
	iniFile << "RefreshRate=" << wxGetApp().appSettings.refreshRate << endl;
	iniFile << "LastDirectory=" << lastDirectory << endl;
	iniFile << "WireConnRadius=" << wxGetApp().appSettings.wireConnRadius << endl;
	iniFile << "WireConnVisible=" << wxGetApp().appSettings.wireConnVisible << endl;
	iniFile << "GridlineVisible=" << wxGetApp().appSettings.gridlineVisible << endl;
	iniFile << "AutoIncrement=" << wxGetApp().appSettings.autoIncrement << endl;
	iniFile.close();
}

void MainFrame::ResumeExecution() {
	if (toolBar->GetToolState(Tool_Pause)) {
		toolBar->ToggleTool(Tool_Pause, false);
		PauseSim();
	}
	else {
		//do nothing
	}
}

void MainFrame::PauseSim() {	
	if (toolBar->GetToolState(Tool_Pause)) {
		simTimer->Stop();
		wxGetApp().appSystemTime.Start(0);
		wxGetApp().appSystemTime.Pause();
	}
	else {
		wxGetApp().appSystemTime.Start(0);
		simTimer->Start(20);
	}
}

//Julian: Added to simplify timer use.
void MainFrame::stopTimers() {
	simTimer->Stop();
	idleTimer->Stop();
}

void MainFrame::startTimers(int at) {
	if (!(toolBar->GetToolState(Tool_Pause)))
	{
		simTimer->Start(at);
	}
	idleTimer->Start(at);
}

void MainFrame::pauseTimers() {
	wxGetApp().appSystemTime.Pause();
	stopTimers();
}

void MainFrame::resumeTimers(int at) {
	if (!(toolBar->GetToolState(Tool_Pause)))
	{
		wxGetApp().appSystemTime.Start(0);
		simTimer->Start(at);
	}
	idleTimer->Start(at);
}

//Julian: All of the following functions were added to support autosave functionality.

void MainFrame::autosave() {
	save(CRASH_FILENAME);
}

void MainFrame::save(string filename) {
	//Pause system so that user can't modify during save
	lock();
	gCircuit->setSimulate(false);
	
	// Disabling timers from autosave thread caused an assertion fail.
	//pauseTimers();

	//Save file
	CircuitParse cirp(currentCanvas);
	cirp.saveCircuit(filename, canvases);

	// Disabling timers from autosave thread caused an assertion fail.
	//Resume system
	//resumeTimers(20);

	gCircuit->setSimulate(true);
	if (!(toolBar->GetToolState(Tool_Lock))) {
		unlock();
	}
}

bool MainFrame::fileIsDirty() {
	return commandProcessor->IsDirty();
}

void MainFrame::removeTempFile() {
	remove(CRASH_FILENAME.c_str());
}

bool MainFrame::isHandlingEvent() {
	return handlingEvent;
}

void MainFrame::lock() {
	if (!isBlackBox) {
		for (unsigned int i = 0; i < canvases.size(); i++) {
			canvases[i]->lockCanvas();
		}
	}
	else {
		currentCanvas->lockCanvas();
	}
}

void MainFrame::unlock() {
	if (!isBlackBox) {
		for (unsigned int i = 0; i < canvases.size(); i++) {
			canvases[i]->unlockCanvas();
		}
	}
	else {
		currentCanvas->unlockCanvas();
	}
}

void MainFrame::load(string filename) {
	loadCircuitFile(filename);
}

//JV - Make new canvas and add it to canvases and canvasBook
//TODO - Find a way to put a tab button in correct place
void MainFrame::OnNewTab(wxCommandEvent& event) {
	int canSize = canvases.size();

	if (canSize < 42) {
		commandProcessor->Submit((wxCommand*)new cmdAddTab(gCircuit, canvasBook, &canvases));
	}
	else {
		wxMessageBox("You have reached the maximum number of tabs.", "Close", wxOK);
	}
	 
/*	canvases.push_back(new GUICanvas(canvasBook, gCircuit, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS));
	ostringstream oss;
	oss << "Page " << canvases.size();
	canvasBook->AddPage(canvases[canvases.size()-1], oss.str(), (false));*/

}

//JV - Handle deletetab event. Remove tab and decrement all following tabs numbers
void MainFrame::OnDeleteTab(wxAuiNotebookEvent& event) {
	int canvasID = event.GetSelection();
	int canSize = canvases.size();

	
	if (canSize > 1) {
		if (!canvases[canvasID]->getGateList()->empty()) {
			wxMessageDialog dialog(this, "All work on this tab will be lost. Would you like to close it?", "Close Tab", wxYES_DEFAULT | wxYES_NO | wxICON_QUESTION);
			switch (dialog.ShowModal()) {
				case wxID_YES:
					break;
				case wxID_NO:
					event.Veto();
					return;
			}
		}
		commandProcessor->Submit((wxCommand*)(new cmdDeleteTab(gCircuit, currentCanvas, canvasBook, &canvases, canvasID)));
/*		canvases.erase(canvases.begin() + canvasID);

		if (canvasID < (canSize - 1)) {
			for (unsigned int i = canvasID; i < canSize; i++) {
				string text = "Page " + to_string(i);
				canvasBook->SetPageText(i, text);
			}
		}*/
		//canvasBook->RemovePage(canvasID - 1);
		event.Veto();
	}
	else {
		wxMessageBox("Tab cannot be closed", "Close", wxOK);
		event.Veto();
	}
}

void MainFrame::OnBlackBox(wxCommandEvent& event) {

	/*
	child = new MainFrame("Black Box Editor", "", this);
	child->Show(true);
	wxGetApp().SetTopWindow(child);
	this->Disable();
	*/
}

void MainFrame::OnApply(wxCommandEvent& event) {

}

void MainFrame::OnApplyAll(wxCommandEvent& event) {

}

void MainFrame::OnCancel(wxCommandEvent& event) {

}

void MainFrame::OnEditColors(wxCommandEvent& event) {
	ColorSettingsDialog dialog(this);
	dialog.ShowModal();
}

void MainFrame::OnReportABug(wxCommandEvent& event) {
	wxLaunchDefaultBrowser("https://cedar.to/XoQJpX", 0);
}

void MainFrame::OnRequestAFeature(wxCommandEvent& event) {
	wxLaunchDefaultBrowser("https://cedar.to/6IlP8c", 0);
}

void MainFrame::OnDownloadLatestVersion(wxCommandEvent& event) {
	wxLaunchDefaultBrowser("https://cedar.to/vjyQw7", 0);
}
