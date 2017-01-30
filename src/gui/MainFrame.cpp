/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   MainFrame: Main frame object
*****************************************************************************/

#include "MainApp.h"
#include "MainFrame.h"
#include "wx/filedlg.h"
#include "wx/timer.h"
#include "wx/wfstream.h"
#include "wx/image.h"
#include "wx/thread.h"
#include "wx/toolbar.h"
#include "wx/clipbrd.h"
#include "wx/dataobj.h"
#include "parse/CircuitParse.h"
#include "OscopeFrame.h"
#include "wx/docview.h"
#include "commands.h"
#include "thread/autoSaveThread.h"
#include "../version.h"

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
	
	EVT_MENU(wxID_UNDO, MainFrame::OnUndo)
	EVT_MENU(wxID_REDO, MainFrame::OnRedo)
	EVT_MENU(wxID_COPY, MainFrame::OnCopy)
	EVT_MENU(wxID_PASTE, MainFrame::OnPaste)
	
    EVT_MENU(View_Oscope, MainFrame::OnOscope)
    EVT_MENU(View_Gridline, MainFrame::OnViewGridline)
    EVT_MENU(View_WireConn, MainFrame::OnViewWireConn)
    
	EVT_TOOL(Tool_Pause, MainFrame::OnPause)
	EVT_TOOL(Tool_Step, MainFrame::OnStep)
	EVT_TOOL(Tool_ZoomIn, MainFrame::OnZoomIn)
	EVT_TOOL(Tool_ZoomOut, MainFrame::OnZoomOut)
	EVT_SCROLL(MainFrame::OnTimeStepModSlider)
	EVT_TOOL(Tool_Lock, MainFrame::OnLock)
	EVT_TOOL(Tool_NewTab, MainFrame::OnNewTab)
	EVT_TOOL(Tool_BlackBox, MainFrame::OnBlackBox)

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

// Global print data object:
wxPrintData *g_printData = (wxPrintData*) NULL;


MainFrame::MainFrame(const wxString& title, string cmdFilename)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1800,900))
{
    // set the frame icon
    //SetIcon(wxICON(sample));
	currentCanvas = nullptr;

	// Set default locations
	if (wxGetApp().appSettings.lastDir == "") lastDirectory = wxGetHomeDir();
	else lastDirectory = wxGetApp().appSettings.lastDir;  // added cast KAS

	//////////////////////////////////////////////////////////////////////////
    // create a menu bar
	//////////////////////////////////////////////////////////////////////////
    wxMenu *fileMenu = new wxMenu; // FILE MENU
	fileMenu->Append(wxID_NEW, "&New\tCtrl+N", "Create new circuit");
	fileMenu->Append(wxID_OPEN, "&Open\tCtrl+O", "Open circuit");
	fileMenu->Append(wxID_SAVE, "&Save\tCtrl+S", "Save circuit");
	fileMenu->Append(wxID_SAVEAS, "Save &As", "Save circuit");
	fileMenu->AppendSeparator();
	fileMenu->Append(File_Export, "Export to Image");
	fileMenu->Append(File_ClipCopy, "Copy Canvas to Clipboard");
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT, "E&xit\tAlt+X", "Quit this program");

    wxMenu *viewMenu = new wxMenu; // VIEW MENU
    viewMenu->Append(View_Oscope, "&Oscope\tCtrl+G", "Show the Oscope");
    wxMenu *settingsMenu = new wxMenu;
    settingsMenu->AppendCheckItem(View_Gridline, "Display Gridlines", "Toggle gridline display");
    settingsMenu->AppendCheckItem(View_WireConn, "Display Wire Connection Points", "Toggle wire connection points");
    viewMenu->AppendSeparator();
    viewMenu->AppendSubMenu(settingsMenu, "Settings");
    
    wxMenu *helpMenu = new wxMenu; // HELP MENU
    helpMenu->Append(wxID_HELP_CONTENTS, "&Contents...\tF1", "Show Help system");
	helpMenu->AppendSeparator();
	helpMenu->Append(Help_ReportABug, "Report a bug...");
	helpMenu->Append(Help_RequestAFeature, "Request a feature...");
	helpMenu->Append(Help_DownloadLatestVersion, "Download latest version...");
	helpMenu->AppendSeparator();
    helpMenu->Append(wxID_ABOUT, "&About...", "Show about dialog");

	wxMenu *editMenu = new wxMenu; // EDIT MENU
	editMenu->Append(wxID_UNDO, "Undo\tCtrl+Z", "Undo last operation");
	editMenu->Append(wxID_REDO, "Redo", "Redo last operation");
	editMenu->AppendSeparator();
	editMenu->Append(Tool_NewTab, "New Tab\tCtrl+T", "New Tab");
	editMenu->AppendSeparator();
	editMenu->Append(wxID_COPY, "Copy\tCtrl+C", "Copy selection to clipboard");
	editMenu->Append(wxID_PASTE, "Paste\tCtrl+V", "Paste selection from clipboard");
	
    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(editMenu, "&Edit");
    menuBar->Append(viewMenu, "&View");
    menuBar->Append(helpMenu, "&Help");

    // set checkmarks on settings menu
    menuBar->Check(View_Gridline, wxGetApp().appSettings.gridlineVisible);
    menuBar->Check(View_WireConn, wxGetApp().appSettings.wireConnVisible);
    
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
	LibraryParse newLib(libPath);
	wxGetApp().libParser = newLib;
	
	//////////////////////////////////////////////////////////////////////////
    // create a toolbar
	//////////////////////////////////////////////////////////////////////////
	toolBar = new wxToolBar(this, TOOLBAR_ID, wxPoint(0,0), wxDefaultSize, wxTB_HORIZONTAL|wxNO_BORDER| wxTB_FLAT);

	// formerly, we were using a resource file to associate the toolbar bitmaps to the program.  I modified the code
	// to read the bitmaps from file directly, without the use of a resource file.  KAS
	string    bitmaps[] = {"new", "open", "save", "undo", "redo", "copy", "paste", "print", "help", "pause", "step", "zoomin", "zoomout", "locked", "newtab", "blackbox", "confirm", "cancel", "applyall"};
	wxBitmap *bmp[19];

	for (int  i = 0; i < 19; i++) {
		bitmaps[i] = "res/bitmaps/" + bitmaps[i] + ".bmp";
		wxFileInputStream in(bitmaps[i]);
		bmp[i] = new wxBitmap(wxImage(in, wxBITMAP_TYPE_BMP));
	}

    int w = bmp[0]->GetWidth(),
        h = bmp[0]->GetHeight();
    toolBar->SetToolBitmapSize(wxSize(w, h));
	toolBar->AddTool(wxID_NEW, "New", *bmp[0], "New");
	toolBar->AddTool(wxID_OPEN, "Open", *bmp[1], "Open");
	toolBar->AddTool(wxID_SAVE, "Save", *bmp[2], "Save"); 
	toolBar->AddTool(Tool_NewTab, "New Tab", *bmp[14], "New Tab");
	toolBar->AddSeparator();
	toolBar->AddTool(wxID_UNDO, "Undo", *bmp[3], "Undo");
	toolBar->AddTool(wxID_REDO, "Redo", *bmp[4], "Redo");
	toolBar->AddSeparator();
	toolBar->AddTool(wxID_COPY, "Copy", *bmp[5], "Copy");
	toolBar->AddTool(wxID_PASTE, "Paste", *bmp[6], "Paste");
	toolBar->AddSeparator();
	toolBar->AddTool(Tool_ZoomIn, "Zoom In", *bmp[11], "Zoom In");
	toolBar->AddTool(Tool_ZoomOut, "Zoom Out", *bmp[12], "Zoom Out");
	toolBar->AddSeparator();
	toolBar->AddTool(Tool_Pause, "Pause/Resume", *bmp[9], "Pause/Resume", wxITEM_CHECK);
	toolBar->AddTool(Tool_Step, "Step", *bmp[10], "Step");
	timeStepModSlider = new wxSlider(toolBar, wxID_ANY, wxGetApp().timeStepMod, 1, 500, wxDefaultPosition, wxSize(125,-1), wxSL_HORIZONTAL|wxSL_AUTOTICKS);
	ostringstream oss;
	oss << wxGetApp().timeStepMod << "ms";
	timeStepModVal = new wxStaticText(toolBar, wxID_ANY, (const wxChar *)oss.str().c_str(), wxDefaultPosition, wxSize(45, -1), wxSUNKEN_BORDER | wxALIGN_RIGHT | wxST_NO_AUTORESIZE);  // added cast KAS
	toolBar->AddControl( timeStepModSlider );
	toolBar->AddControl( timeStepModVal );
	toolBar->AddSeparator();
	toolBar->AddTool(Tool_Lock, "Lock state", *bmp[13], "Lock state", wxITEM_CHECK);
	toolBar->AddSeparator();
	toolBar->AddTool(Tool_BlackBox, "Black Box", *bmp[15], "Black Box");
	toolBar->AddSeparator();
	toolBar->AddTool(wxID_ABOUT, "About", *bmp[8], "About");
	SetToolBar(toolBar);
	toolBar->Show(true);

	//finished with the bitmaps, so we can release the pointers  KAS
	for (int i = 0; i < 19; i++) {
		delete bmp[i];
	}

    CreateStatusBar(2);
    SetStatusText("");

	mainSizer = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer* leftPaneSizer = new wxBoxSizer( wxVERTICAL );
	wxSize sz = this->GetClientSize();
	
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
	gCircuit->SetCommandProcessor(commandProcessor);
	gCircuit->GetCommandProcessor()->SetEditMenu(editMenu);
	gCircuit->GetCommandProcessor()->Initialize();

	canvasBook = new wxAuiNotebook(this, NOTEBOOK_ID, wxDefaultPosition, wxSize(400,400), wxAUI_NB_CLOSE_ON_ACTIVE_TAB| wxAUI_NB_SCROLL_BUTTONS);
	mainSizer->Add( canvasBook, wxSizerFlags(1).Expand().Border(wxALL, 0) );

	//add 1 tab: Left loop to allow for different default
	for (int i = 0; i < 1; i++) {
		canvases.push_back(new GUICanvas(canvasBook, gCircuit, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS));
		ostringstream oss;
		oss << "Page " << (i+1);
		canvasBook->AddPage(canvases[i], (const wxChar *)oss.str().c_str(), (i == 0 ? true : false));  // KAS
	}

	currentCanvas = canvases[0];
	gCircuit->setCurrentCanvas(currentCanvas);
	currentCanvas->setMinimap(miniMap);
	mainSizer->Show(canvasBook);
	currentCanvas->SetFocus();

	SetSizer( mainSizer);
		
	threadLogic *thread = CreateThread();
	autoSaveThread *autoThread = CreateSaveThread();
	
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

	// Create the print data object:
	g_printData = new wxPrintData;
	g_printData->SetOrientation(wxLANDSCAPE);
	
	this->SetSize( wxGetApp().appSettings.mainFrameLeft, wxGetApp().appSettings.mainFrameTop, wxGetApp().appSettings.mainFrameWidth, wxGetApp().appSettings.mainFrameHeight );
	
	doOpenFile = (cmdFilename.size() > 0);
	this->openedFilename = (const wxChar *)cmdFilename.c_str(); // KAS

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

	if (autoThread->Run() != wxTHREAD_NO_ERROR)
	{
		wxLogError("Autosave thread not started!");
	}
	currentTempNum = 0;
	handlingEvent = false;
	wxInitAllImageHandlers(); //Julian: Added to allow saving all types of image files

	// Colin: for testing dynamic gates
	//DynamicGate* dg = new DynamicGate(currentCanvas, gCircuit, gCircuit->getNextAvailableGateID(), 3, 0, 0, "AND");
}

MainFrame::~MainFrame() {
	
	saveSettings();
	
	stopTimers();

	// Shut down the detached thread and wait for it to exit
	wxGetApp().logicThread->Delete();
	wxGetApp().saveThread->Delete();

	
	wxGetApp().m_semAllDone.Wait();
	
	
	
	// Delete the various objects
	delete wxGetApp().helpController;
	wxGetApp().helpController = NULL;
	
	
	
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

    wxCriticalSectionLocker enter(wxGetApp().m_critsect);
	wxGetApp().logicThread = thread;
	
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
	if (commandProcessor->IsDirty() && !destroy) {
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

	if (destroy)
	{
		removeTempFile();
	}
	else
	{
		handlingEvent = false;
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
    msg.Printf(VERSION_ABOUT_TEXT().c_str());

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

	wxGetApp().dGUItoLOGIC.clear();
	wxGetApp().dLOGICtoGUI.clear();

	for (unsigned int i = 0; i < canvases.size(); i++) canvases[i]->clearCircuit();
	gCircuit->reInitializeLogicCircuit();
	commandProcessor->ClearCommands();
	commandProcessor->SetMenuStrings();
	//JV - Added so that new starts with one tab
	for (unsigned int j = canvases.size() - 1; j > 0; j--) {
		canvasBook->DeletePage(j);
		canvases.erase(canvases.end() - 1);
	}

	currentCanvas->Update(); // Render();
	this->SetTitle(VERSION_TITLE()); // KAS
	removeTempFile();
	currentTempNum++;
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
		loadCircuitFile((const char *)dialog.GetPath().c_str());  // KAS
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
	wxString path = (const wxChar *)fileName.c_str();  // KAS
	
	openedFilename = path;
	this->SetTitle(VERSION_TITLE() + " - " + path );
	while (!(wxGetApp().dGUItoLOGIC.empty())) wxGetApp().dGUItoLOGIC.pop_front();
	while (!(wxGetApp().dLOGICtoGUI.empty())) wxGetApp().dLOGICtoGUI.pop_front();
	for (unsigned int i = 0; i < canvases.size(); i++) canvases[i]->clearCircuit();
	gCircuit->reInitializeLogicCircuit();
	commandProcessor->ClearCommands();
	commandProcessor->SetMenuStrings();
	//JV - Delete all but the first tab
	for (unsigned int j = canvases.size() - 1; j > 0; j--) {
		canvasBook->DeletePage(j);
		canvases.erase(canvases.end()-1);
	}
	
    CircuitParse cirp((const char *)path.c_str(), canvases); // KAS
	canvases = cirp.parseFile();
	
	//JV - Put pages back into canvas book
	for (unsigned int i = 1; i < canvases.size(); i++) 
	{
		ostringstream oss;
		oss << "Page " << (i + 1);
		canvasBook->AddPage(canvases[i], (const wxChar *)oss.str().c_str(), (i == 0 ? true : false));
	}
	currentCanvas = canvases[0];
	gCircuit->setCurrentCanvas(currentCanvas);
	currentCanvas->setMinimap(miniMap);
	mainSizer->Show(canvasBook);
	currentCanvas->SetFocus();

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
}

void MainFrame::OnViewWireConn(wxCommandEvent& event) {
	wxGetApp().appSettings.wireConnVisible = event.IsChecked();
	if (currentCanvas != NULL) currentCanvas->Update();
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
	gCircuit->lastTimeMod = wxGetApp().timeStepMod;
	gCircuit->lastNumSteps = wxGetApp().appSystemTime.Time() / wxGetApp().timeStepMod;
	gCircuit->sendMessageToCore(new Message_STEPSIM(wxGetApp().appSystemTime.Time() / wxGetApp().timeStepMod));
	currentCanvas->getCircuit()->setSimulate(false);
	wxGetApp().appSystemTime.Start(wxGetApp().appSystemTime.Time() % wxGetApp().timeStepMod);
}

void MainFrame::OnIdle(wxTimerEvent& event) {
	wxCriticalSectionLocker locker(wxGetApp().m_critsect);
	while (wxGetApp().mexMessages.TryLock() == wxMUTEX_BUSY) wxYield();
	while (wxGetApp().dLOGICtoGUI.size() > 0) {
		Message *message = wxGetApp().dLOGICtoGUI.front();
		wxGetApp().dLOGICtoGUI.pop_front();
		gCircuit->parseMessage(message);
		delete message;
	}
	wxGetApp().mexMessages.Unlock();

	if (mainSizer == NULL) return;
	
	if ( doOpenFile ) {
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
	currentCanvas->SetFocus();
	currentCanvas->Update();
}

void MainFrame::OnUndo(wxCommandEvent& event) {
	commandProcessor->Undo();
	currentCanvas->Update();
}

void MainFrame::OnRedo(wxCommandEvent& event) {
	commandProcessor->Redo();
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
	wxImage circuitImage = currentCanvas->renderToImage(imageSize.GetWidth() * 2, imageSize.GetHeight() * 2, 32);
	wxBitmap circuitBitmap(circuitImage);

	// restore grid display setting
	wxGetApp().appSettings.gridlineVisible = gridlineVisible;
	wxGetApp().doingBitmapExport = false;

	return circuitBitmap;
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
	timeStepModVal->SetLabel((const wxChar *)oss.str().c_str()); // KAS
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
	
	ofstream iniFile(settingsIni.c_str(), ios::out);
	iniFile << "GateLib=" << wxGetApp().appSettings.gateLibFile.substr(numCharAbsolute) << endl;
	iniFile << "HelpFile=" << wxGetApp().appSettings.helpFile.substr(numCharAbsolute) << endl;
	iniFile << "TextFont=" << wxGetApp().appSettings.textFontFile.substr(numCharAbsolute) << endl;
	iniFile << "FrameWidth=" << this->GetSize().GetWidth() << endl;
	iniFile << "FrameHeight=" << this->GetSize().GetHeight() << endl;
	iniFile << "FrameLeft=" << this->GetPosition().x << endl;
	iniFile << "FrameTop=" << this->GetPosition().y << endl;
	iniFile << "TimeStep=" << wxGetApp().timeStepMod << endl;
	iniFile << "RefreshRate=" << wxGetApp().appSettings.refreshRate << endl;
	iniFile << "LastDirectory=" << lastDirectory.c_str() << endl;
	iniFile << "WireConnRadius=" << wxGetApp().appSettings.wireConnRadius << endl;
	iniFile << "WireConnVisible=" << wxGetApp().appSettings.wireConnVisible << endl;
	iniFile << "GridlineVisible=" << wxGetApp().appSettings.gridlineVisible << endl;
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
	for (unsigned int i = 0; i < canvases.size(); i++) {
		canvases[i]->lockCanvas();
	}
}

void MainFrame::unlock() {
	for (unsigned int i = 0; i < canvases.size(); i++) {
		canvases[i]->unlockCanvas();
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
		gCircuit->GetCommandProcessor()->Submit((wxCommand*)new cmdAddTab(gCircuit, canvasBook, &canvases));
	}
	else {
		wxMessageBox("You have reached the maximum number of tabs.", "Close", wxOK);
	}
	 
/*	canvases.push_back(new GUICanvas(canvasBook, gCircuit, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS));
	ostringstream oss;
	oss << "Page " << canvases.size();
	canvasBook->AddPage(canvases[canvases.size()-1], (const wxChar *)oss.str().c_str(), (false));*/

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
		gCircuit->GetCommandProcessor()->Submit((wxCommand*)(new cmdDeleteTab(gCircuit, currentCanvas, canvasBook, &canvases, canvasID)));
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