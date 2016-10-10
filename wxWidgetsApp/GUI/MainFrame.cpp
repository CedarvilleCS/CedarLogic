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
#include "CircuitParse.h"
#include "OscopeFrame.h"
#include "wx/docview.h"
#include "commands.h"
#include "CircuitPrint.h"
#include "autoSaveThread.h"

DECLARE_APP(MainApp)

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_EXIT,  MainFrame::OnQuit)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
    EVT_MENU(wxID_HELP_CONTENTS, MainFrame::OnHelpContents)
    EVT_MENU(wxID_NEW, MainFrame::OnNew)
    EVT_MENU(wxID_OPEN, MainFrame::OnOpen)
    EVT_MENU(wxID_SAVE, MainFrame::OnSave)
    EVT_MENU(wxID_SAVEAS, MainFrame::OnSaveAs)
	EVT_MENU(wxID_PRINT, MainFrame::OnPrint)
	EVT_MENU(wxID_PREVIEW, MainFrame::OnPrintPreview)
	
	EVT_MENU(wxID_UNDO, MainFrame::OnUndo)
	EVT_MENU(wxID_REDO, MainFrame::OnRedo)
	EVT_MENU(wxID_COPY, MainFrame::OnCopy)
	EVT_MENU(wxID_PASTE, MainFrame::OnPaste)
	EVT_MENU(Edit_Export_BW, MainFrame::OnExportBitmapBW)
	EVT_MENU(Edit_Export_C, MainFrame::OnExportBitmapC)
	
    EVT_MENU(View_Oscope, MainFrame::OnOscope)
    EVT_MENU(View_Gridline, MainFrame::OnViewGridline)
    EVT_MENU(View_WireConn, MainFrame::OnViewWireConn)
    
	EVT_TOOL(Tool_Pause, MainFrame::OnPause)
	EVT_TOOL(Tool_Step, MainFrame::OnStep)
	EVT_TOOL(Tool_ZoomIn, MainFrame::OnZoomIn)
	EVT_TOOL(Tool_ZoomOut, MainFrame::OnZoomOut)
	EVT_SCROLL(MainFrame::OnTimeStepModSlider)
	EVT_TOOL(Tool_Lock, MainFrame::OnLock)
	
    //EVT_SIZE(MainFrame::OnSize)
    //EVT_MAXIMIZE(MainFrame::OnMaximize)
    
	EVT_TIMER(TIMER_ID, MainFrame::OnTimer)
	EVT_TIMER(IDLETIMER_ID, MainFrame::OnIdle)

	EVT_NOTEBOOK_PAGE_CHANGED(NOTEBOOK_ID, MainFrame::OnNotebookPage)
	
	EVT_CLOSE(MainFrame::OnClose)
END_EVENT_TABLE()

#define ID_TEXTCTRL 5001

// Global print data object:
wxPrintData *g_printData = (wxPrintData*) NULL;


MainFrame::MainFrame(const wxString& title, string cmdFilename)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600,600))
{
    // set the frame icon
    //SetIcon(wxICON(sample));
	currentCanvas = nullptr;

	// Set default locations
	if (wxGetApp().appSettings.lastDir == "") lastDirectory = wxGetHomeDir() + _T("/My Documents");
	else lastDirectory = (const wxChar *)wxGetApp().appSettings.lastDir.c_str();  // added cast KAS

	//////////////////////////////////////////////////////////////////////////
    // create a menu bar
	//////////////////////////////////////////////////////////////////////////
    wxMenu *fileMenu = new wxMenu; // FILE MENU
	fileMenu->Append(wxID_NEW, _T("&New\tCtrl+N"), _T("Create new circuit"));
	fileMenu->Append(wxID_OPEN, _T("&Open\tCtrl+O"), _T("Open circuit"));
	fileMenu->Append(wxID_SAVE, _T("&Save\tCtrl+S"), _T("Save circuit"));
	fileMenu->Append(wxID_SAVEAS, _T("Save &As"), _T("Save circuit"));
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_PRINT, _T("&Print\tCtrl+P"), _T("Print circuit"));
	fileMenu->Append(wxID_PREVIEW, _T("P&rint Preview"), _T("Preview circuit printout"));
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT, _T("E&xit\tAlt+X"), _T("Quit this program"));

    wxMenu *viewMenu = new wxMenu; // VIEW MENU
    viewMenu->Append(View_Oscope, _T("&Oscope\tCtrl+G"), _T("Show the Oscope"));
    wxMenu *settingsMenu = new wxMenu;
    settingsMenu->AppendCheckItem(View_Gridline, _T("Display Gridlines"), _T("Toggle gridline display"));
    settingsMenu->AppendCheckItem(View_WireConn, _T("Display Wire Connection Points"), _T("Toggle wire connection points"));
    viewMenu->AppendSeparator();
    viewMenu->AppendSubMenu(settingsMenu, _T("Settings"));
    
    wxMenu *helpMenu = new wxMenu; // HELP MENU
    helpMenu->Append(wxID_HELP_CONTENTS, _T("&Contents...\tF1"), _T("Show Help system"));
	helpMenu->AppendSeparator();
    helpMenu->Append(wxID_ABOUT, _T("&About..."), _T("Show about dialog"));

	wxMenu *editMenu = new wxMenu; // EDIT MENU
	editMenu->Append(wxID_UNDO, _T("Undo\tCtrl+Z"), _T("Undo last operation"));
	editMenu->Append(wxID_REDO, _T("Redo"), _T("Redo last operation"));
	editMenu->AppendSeparator();
	editMenu->Append(wxID_COPY, _T("Copy\tCtrl+C"), _T("Copy selection to clipboard"));
	editMenu->Append(wxID_PASTE, _T("Paste\tCtrl+V"), _T("Paste selection from clipboard"));
	editMenu->AppendSeparator();
	wxMenu *exportMenu = new wxMenu;
	exportMenu->Append(Edit_Export_BW, _T("Black and White"), _T("Export B&W circuit bitmap to clipboard"));
	exportMenu->Append(Edit_Export_C, _T("Color"), _T("Export color circuit bitmap to clipboard"));
	editMenu->AppendSubMenu(exportMenu, _T("Export bitmap"));
	
    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(editMenu, _T("&Edit"));
    menuBar->Append(viewMenu, _T("&View"));
    menuBar->Append(helpMenu, _T("&Help"));

    // set checkmarks on settings menu
    menuBar->Check(View_Gridline, wxGetApp().appSettings.gridlineVisible);
    menuBar->Check(View_WireConn, wxGetApp().appSettings.wireConnVisible);
    
    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
    
	//////////////////////////////////////////////////////////////////////////
    // parse a gate library
	//////////////////////////////////////////////////////////////////////////
#ifndef _PRODUCTION_
	string libPath = wxGetApp().pathToExe + "../GUI/cl_gatedefs.xml";
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
	toolBar = new wxToolBar(this, TOOLBAR_ID, wxPoint(0,0), wxDefaultSize, wxTB_HORIZONTAL|wxNO_BORDER);

	// formerly, we were using a resource file to associate the toolbar bitmaps to the program.  I modified the code
	// to read the bitmaps from file directly, without the use of a resource file.  KAS
	string    bitmaps[] = {"new", "open", "save", "undo", "redo", "copy", "paste", "print", "help", "pause", "step", "zoomin", "zoomout", "locked"};
	wxBitmap *bmp[14];

	for (int  i = 0; i < 14; i++) {
		bitmaps[i] = "GUI/bitmaps/" + bitmaps[i] + ".bmp";
		wxFileInputStream in(bitmaps[i]);
		bmp[i] = new wxBitmap(wxImage(in, wxBITMAP_TYPE_BMP));
	}

    int w = bmp[0]->GetWidth(),
        h = bmp[0]->GetHeight();
    toolBar->SetToolBitmapSize(wxSize(w, h));
	toolBar->AddTool(wxID_NEW, _T("New"), *bmp[0], _T("New"));
	toolBar->AddTool(wxID_OPEN, _T("Open"), *bmp[1], wxT("Open"));
	toolBar->AddTool(wxID_SAVE, _T("Save"), *bmp[2], wxT("Save")); 
	toolBar->AddSeparator();
	toolBar->AddTool(wxID_PRINT, _T("Print"), *bmp[7], wxT("Print"));
	toolBar->AddSeparator();
	toolBar->AddTool(wxID_UNDO, _T("Undo"), *bmp[3], wxT("Undo"));
	toolBar->AddTool(wxID_REDO, _T("Redo"), *bmp[4], wxT("Redo"));
	toolBar->AddSeparator();
	toolBar->AddTool(wxID_COPY, _T("Copy"), *bmp[5], wxT("Copy"));
	toolBar->AddTool(wxID_PASTE, _T("Paste"), *bmp[6], wxT("Paste"));
	toolBar->AddSeparator();
	toolBar->AddTool(Tool_ZoomIn, _T("Zoom In"), *bmp[11], wxT("Zoom In"));
	toolBar->AddTool(Tool_ZoomOut, _T("Zoom Out"), *bmp[12], wxT("Zoom Out"));
	toolBar->AddSeparator();
	toolBar->AddTool(Tool_Pause, _T("Pause/Resume"), *bmp[9], wxT("Pause/Resume"), wxITEM_CHECK);
	toolBar->AddTool(Tool_Step, _T("Step"), *bmp[10], wxT("Step"));
	timeStepModSlider = new wxSlider(toolBar, wxID_ANY, wxGetApp().timeStepMod, 1, 500, wxDefaultPosition, wxSize(125,-1), wxSL_HORIZONTAL|wxSL_AUTOTICKS);
	ostringstream oss;
	oss << wxGetApp().timeStepMod << "ms";
	timeStepModVal = new wxStaticText(toolBar, wxID_ANY, (const wxChar *)oss.str().c_str(), wxDefaultPosition, wxSize(45, -1), wxSUNKEN_BORDER | wxALIGN_RIGHT | wxST_NO_AUTORESIZE);  // added cast KAS
	toolBar->AddControl( timeStepModSlider );
	toolBar->AddControl( timeStepModVal );
	toolBar->AddSeparator();
	toolBar->AddTool(Tool_Lock, _T("Lock state"), *bmp[13], wxT("Lock state"), wxITEM_CHECK);
	toolBar->AddSeparator();
	toolBar->AddTool(wxID_ABOUT, _T("About"), *bmp[8], wxT("About"));
	SetToolBar(toolBar);
	toolBar->Show(true);

	//finished with the bitmaps, so we can release the pointers  KAS
	for (int i = 0; i < 4; i++) {
		delete bmp[i];
	}

    CreateStatusBar(2);
    SetStatusText(_T(""));

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

	canvasBook = new wxNotebook(this, NOTEBOOK_ID, wxDefaultPosition, wxSize(400,400));
	mainSizer->Add( canvasBook, wxSizerFlags(1).Expand().Border(wxALL, 0) );

	//add 10 tabs
	for (int i = 0; i < 10; i++) {
		canvases.push_back(new GUICanvas(canvasBook, gCircuit, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS));
		ostringstream oss;
		oss << "Page " << (i+1);
		canvasBook->AddPage(canvases[i], (const wxChar *)oss.str().c_str(), (i == 0 ? true : false));  // KAS
	}
	
	currentCanvas = canvases[0];
	gCircuit->setCurrentCanvas(currentCanvas);
	currentCanvas->setMinimap(miniMap);
	mainSizer->Show( canvasBook );
	currentCanvas->SetFocus();

	SetSizer( mainSizer);
		
	threadLogic *thread = CreateThread();
	autoSaveThread *autoThread = CreateSaveThread();
	
    if ( thread->Run() != wxTHREAD_NO_ERROR )
    {
       wxLogError(wxT("Can't start thread!"));
    }
	
	// Setup the "Maximize Catch" flag:
	sizeChanged = false;
	
	gCircuit->setOscope(new OscopeFrame(this, _T("O-Scope"), gCircuit));
	
	toolBar->Realize();

	// Create the print data object:
	g_printData = new wxPrintData;
	g_printData->SetOrientation(wxLANDSCAPE);
	
	this->SetSize( wxGetApp().appSettings.mainFrameLeft, wxGetApp().appSettings.mainFrameTop, wxGetApp().appSettings.mainFrameWidth, wxGetApp().appSettings.mainFrameHeight );
	
	doOpenFile = (cmdFilename.size() > 0);
	this->openedFilename = (const wxChar *)cmdFilename.c_str(); // KAS

	if (autoThread->Run() != wxTHREAD_NO_ERROR)
	{
		wxLogError(wxT("Autosave thread not started!"));
	}
	currentTempNum = 0;
	handlingEvent = false;
}

MainFrame::~MainFrame() {
	
	saveSettings();
	
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
}

threadLogic *MainFrame::CreateThread()
{
	threadLogic *thread = new threadLogic();
    if ( thread->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogError(wxT("Can't create thread!"));
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
		wxLogError(wxT("Can't create autosave thread!"));
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
	
	wxGetApp().appSystemTime.Pause();

	// Allow the user to save the file, unless we are in the midst of terminating the app!!, KAS 4/26/07	
	if (commandProcessor->IsDirty() && !destroy) {
		wxMessageDialog dialog( this, wxT("Circuit has not been saved.  Would you like to save it?"), wxT("Save Circuit"), wxYES_DEFAULT|wxYES_NO|wxCANCEL|wxICON_QUESTION);
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
	
	if (!(toolBar->GetToolState(Tool_Pause))) {
		wxGetApp().appSystemTime.Start(0);
	}
	
	removeTempFile();

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
    msg.Printf( _T("CEDAR Logic Simulator 1.5a\nCopyright 2007 Cedarville University, Matt Lewellyn, \n\tDavid Knierim, Ben Sprague, Joshua Lansford\n\tand Nathan Harro\n\nFont rendering thanks to GLFont library (created by Brad Fish, bhf5@email.byu.edu)\n\n All rights reserved\nSee license.txt for details."));

    wxMessageBox(msg, _T("About"), wxOK | wxICON_INFORMATION, this);
}

void MainFrame::OnNew(wxCommandEvent& event) {
	handlingEvent = true;

	if (commandProcessor->IsDirty()) {
		wxMessageDialog dialog( this, wxT("Circuit has not been saved.  Would you like to save it?"), wxT("Save Circuit"), wxYES_DEFAULT|wxYES_NO|wxCANCEL|wxICON_QUESTION);
		switch (dialog.ShowModal()) {
		case wxID_YES:
			OnSave(event);
			break;
		case wxID_CANCEL:
			return;
		}			
	}
	wxGetApp().appSystemTime.Pause();
	wxGetApp().dGUItoLOGIC.clear();
	wxGetApp().dLOGICtoGUI.clear();
	for (unsigned int i = 0; i < canvases.size(); i++) canvases[i]->clearCircuit();
	gCircuit->reInitializeLogicCircuit();
	commandProcessor->ClearCommands();
	commandProcessor->SetMenuStrings();
	currentCanvas->Update(); // Render();
	this->SetTitle((const wxChar *)"CEDAR Logic Simulator"); // KAS
	removeTempFile();
	currentTempNum++;
    openedFilename = _T("");
	if (!(toolBar->GetToolState(Tool_Pause))) {
		wxGetApp().appSystemTime.Start(0);
	}

	handlingEvent = false;
}

void MainFrame::OnOpen(wxCommandEvent& event) {
	
	handlingEvent = true;

	currentCanvas->getCircuit()->setSimulate(false);
	if (commandProcessor->IsDirty()) {
		wxMessageDialog dialog( this, wxT("Circuit has not been saved.  Would you like to save it?"), wxT("Save Circuit"), wxYES_DEFAULT|wxYES_NO|wxCANCEL|wxICON_QUESTION);
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
	wxGetApp().appSystemTime.Pause();

	wxString caption = wxT("Open a circuit");
	wxString wildcard = wxT("Circuit files (*.cdl)|*.cdl");
	wxString defaultFilename = wxT("");
	wxFileDialog dialog(this, caption, wxEmptyString, defaultFilename, wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	dialog.SetDirectory(lastDirectory);
	
	
	if (dialog.ShowModal() == wxID_OK) {
		lastDirectory = dialog.GetDirectory();
		loadCircuitFile((const char *)dialog.GetPath().c_str());  // KAS
	}
    currentCanvas->Update(); // Render();
	currentCanvas->getCircuit()->setSimulate(true);
	if (!(toolBar->GetToolState(Tool_Pause))) {
		wxGetApp().appSystemTime.Start(0);
	}

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
	removeTempFile();
	openedFilename = path;
	this->SetTitle( _T("CEDAR Logic Simulator - ") + path );
	while (!(wxGetApp().dGUItoLOGIC.empty())) wxGetApp().dGUItoLOGIC.pop_front();
	while (!(wxGetApp().dLOGICtoGUI.empty())) wxGetApp().dLOGICtoGUI.pop_front();
	for (unsigned int i = 0; i < canvases.size(); i++) canvases[i]->clearCircuit();
	gCircuit->reInitializeLogicCircuit();
	commandProcessor->ClearCommands();
	commandProcessor->SetMenuStrings();
    CircuitParse cirp((const char *)path.c_str(), canvases); // KAS
	cirp.parseFile();
}

void MainFrame::OnSave(wxCommandEvent& event) {
	if (openedFilename == _T("")) OnSaveAs(event);
	else {
		gCircuit->setSimulate(false);
		commandProcessor->MarkAsSaved();
		wxGetApp().appSystemTime.Pause();
		CircuitParse cirp(currentCanvas);
		cirp.saveCircuit((string)(const char*)openedFilename.c_str(), canvases); //currentCanvas->getGateList(), currentCanvas->getWireList());
		if (!(toolBar->GetToolState(Tool_Pause))) {
			wxGetApp().appSystemTime.Start(0);
		}
		gCircuit->setSimulate(true);		
	}
}

void MainFrame::OnSaveAs(wxCommandEvent& WXUNUSED(event)) {
	handlingEvent = true;

	gCircuit->setSimulate(false);
	wxGetApp().appSystemTime.Pause();
	wxString caption = wxT("Save circuit");
	wxString wildcard = wxT("Circuit files (*.cdl)|*.cdl");
	wxString defaultFilename = wxT("");
	wxFileDialog dialog(this, caption, wxEmptyString, defaultFilename, wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	dialog.SetDirectory(lastDirectory);
	if (dialog.ShowModal() == wxID_OK) {
		removeTempFile();
		wxString path = dialog.GetPath();
		openedFilename = path;
		this->SetTitle( _T("CEDAR Logic Simulator - ") + path );
		commandProcessor->MarkAsSaved();
		CircuitParse cirp(currentCanvas);
		cirp.saveCircuit((string)(const char*)openedFilename.c_str(), canvases); //currentCanvas->getGateList(), currentCanvas->getWireList());
	}
	if (!(toolBar->GetToolState(Tool_Pause))) {
		wxGetApp().appSystemTime.Start(0);
	}
	gCircuit->setSimulate(true);

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
	gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_STEPSIM, new klsMessage::Message_STEPSIM(wxGetApp().appSystemTime.Time() / wxGetApp().timeStepMod)));
	currentCanvas->getCircuit()->setSimulate(false);
	wxGetApp().appSystemTime.Start(wxGetApp().appSystemTime.Time() % wxGetApp().timeStepMod);
}

void MainFrame::OnIdle(wxTimerEvent& event) {
	wxCriticalSectionLocker locker(wxGetApp().m_critsect);
	while (wxGetApp().mexMessages.TryLock() == wxMUTEX_BUSY) wxYield();
	while (wxGetApp().dLOGICtoGUI.size() > 0) {
		gCircuit->parseMessage(wxGetApp().dLOGICtoGUI.front());
		wxGetApp().dLOGICtoGUI.pop_front();
	}
	wxGetApp().mexMessages.Unlock();

	if (mainSizer == NULL) return;
	
	if ( doOpenFile ) {
		doOpenFile = false;
		CircuitParse cirp((const char *)openedFilename.c_str(), canvases); // KAS
    	cirp.parseFile();
	    currentCanvas->Update(); // Render();
		this->SetTitle( _T("CEDAR Logic Simulator - ") + openedFilename );
	}
	
	if ( gCircuit->panic ) {
		gCircuit->panic = false;
		toolBar->ToggleTool( Tool_Pause, true );
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
			wxMessageBox(_T("Overloading simulator: please increase time per step and then resume simulation."), _T("Error - overload"), wxOK | wxICON_ERROR, NULL);
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

void MainFrame::OnNotebookPage(wxNotebookEvent& event) {
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

void MainFrame::OnPrint(wxCommandEvent& WXUNUSED(event)) {
    wxPrintDialogData printDialogData(* g_printData);

    wxPrinter printer(& printDialogData);
    CircuitPrint printout(currentCanvas, openedFilename, _T("Logic Circuit"));

    if (!printer.Print(this, &printout, true /*prompt*/))
    {
        if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
            wxMessageBox(_T("There was a problem printing.\nPerhaps your current printer is not set correctly?"), _T("Printing"), wxOK);
//        else
//            wxMessageBox(_T("You canceled printing"), _T("Printing"), wxOK);
    }
    else
    {
        //(*g_printData) = printer.GetPrintDialogData().GetPrintData();
	}
}

void MainFrame::OnPrintPreview(wxCommandEvent& WXUNUSED(event)) {
	wxPrintDialogData printDialogData(* g_printData);
	CircuitPrint* printoutPreview = new CircuitPrint(currentCanvas, openedFilename, _T("Logic Circuit"));
	CircuitPrint* printoutPrinter = new CircuitPrint(currentCanvas, openedFilename, _T("Logic Circuit"));
	wxPrintPreview *preview = new wxPrintPreview(printoutPreview, printoutPrinter, &printDialogData);
    if (!preview->Ok())
    {
        delete preview;
        wxMessageBox(_T("There was a problem previewing.\nPerhaps your current printer is not set correctly?"), _T("Previewing"), wxOK);
        return;
    }
//
    wxPreviewFrame *frame = new wxPreviewFrame(preview, this, _T("Print Preview"), wxPoint(100, 100), wxSize(600, 650));
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show();
}

void MainFrame::OnExportBitmapBW(wxCommandEvent& event) {
	// disable the grid display
	bool gridlineVisible = wxGetApp().appSettings.gridlineVisible;
	wxGetApp().appSettings.gridlineVisible = false;
	wxGetApp().doingBitmapExport = true;
	// render the image
	wxSize imageSize = currentCanvas->GetClientSize();
	wxImage circuitImage = currentCanvas->renderToImage(imageSize.GetWidth()*2, imageSize.GetHeight()*2, 32, true);
	wxBitmap circuitBitmap(circuitImage);
	if (wxTheClipboard->Open()) {
		wxTheClipboard->SetData(new wxBitmapDataObject(circuitBitmap));
		wxTheClipboard->Close();
	}
	// restore grid display setting
	wxGetApp().appSettings.gridlineVisible = gridlineVisible;
	wxGetApp().doingBitmapExport = false;
}

void MainFrame::OnExportBitmapC(wxCommandEvent& event) {
	// disable the grid display
	bool gridlineVisible = wxGetApp().appSettings.gridlineVisible;
	wxGetApp().appSettings.gridlineVisible = false;
	wxGetApp().doingBitmapExport = true;
	// render the image
	wxSize imageSize = currentCanvas->GetClientSize();
	wxImage circuitImage = currentCanvas->renderToImage(imageSize.GetWidth()*2, imageSize.GetHeight()*2, 32);
	wxBitmap circuitBitmap(circuitImage);
	if (wxTheClipboard->Open()) {
		wxTheClipboard->SetData(new wxBitmapDataObject(circuitBitmap));
		wxTheClipboard->Close();
	}
	// restore grid display setting
	wxGetApp().appSettings.gridlineVisible = gridlineVisible;
	wxGetApp().doingBitmapExport = false;
}

void MainFrame::OnPause(wxCommandEvent& event) {
	PauseSim();
}

void MainFrame::OnStep(wxCommandEvent& event) {
	if (!(currentCanvas->getCircuit()->getSimulate())) {
		return;
	}
	gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_STEPSIM, new klsMessage::Message_STEPSIM(1)));
	currentCanvas->getCircuit()->setSimulate(false);
}

void MainFrame::OnLock(wxCommandEvent& event) {
	if (toolBar->GetToolState(Tool_Lock)) {
		for (unsigned int i = 0; i < canvases.size(); i++) canvases[i]->lockCanvas();
	} else {
		for (unsigned int i = 0; i < canvases.size(); i++) canvases[i]->unlockCanvas();
	}
}

void MainFrame::OnZoomIn(wxCommandEvent& event) {
	//TODO: There is no way to check if currentCanvas is valid first!!!
	currentCanvas->zoomInOrOut( true );
}

void MainFrame::OnZoomOut(wxCommandEvent& event) {
	//TODO: There is no way to check if currentCanvas is valid first!!!
	currentCanvas->zoomInOrOut( false );
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
	
	#ifdef _PRODUCTION_
		string settingsIni = wxGetApp().pathToExe + "./settings.ini";
	#else
		string settingsIni = wxGetApp().pathToExe + "../settings.ini";
	#endif
	
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
		wxGetApp().appSystemTime.Start(0);
		wxGetApp().appSystemTime.Pause();
	}
	else {
		wxGetApp().appSystemTime.Start(0);
	}
}

//Julian: I'm aware this is very bad. This is strictly for testing purposes.
//The code is copied from OnSaveAs to see if I can invoke it from a thread.
void MainFrame::OnThreadSave()
{
	if (openedFilename == _T(""))
	{
		gCircuit->setSimulate(false);
		wxGetApp().appSystemTime.Pause();
		CircuitParse cirp(currentCanvas);
		cirp.saveCircuit("Circuit" + to_string(currentTempNum) + ".temp", canvases);
		if (!(toolBar->GetToolState(Tool_Pause))) {
			wxGetApp().appSystemTime.Start(0);
		}
		gCircuit->setSimulate(true);
	}

	else
	{
		gCircuit->setSimulate(false);
		wxGetApp().appSystemTime.Pause();
		CircuitParse cirp(currentCanvas);
		cirp.saveCircuit((string)(const char*)openedFilename.c_str() + ".temp", canvases);
		if (!(toolBar->GetToolState(Tool_Pause))) {
			wxGetApp().appSystemTime.Start(0);
		}
		gCircuit->setSimulate(true);
	}
}

bool MainFrame::FileIsDirty()
{
	return commandProcessor->IsDirty();
}

void MainFrame::removeTempFile()
{
	if (openedFilename == _T(""))
	{
		remove(("Circuit" + to_string(currentTempNum) + ".temp").c_str());
	}
	else
	{
		remove((openedFilename + ".temp").c_str());
	}
}

bool MainFrame::isHandlingEvent()
{
	return handlingEvent;
}