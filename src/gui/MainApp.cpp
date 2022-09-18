/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   MainApp: Main application object
*****************************************************************************/

#include "MainApp.h"
#include "MainFrame.h"
#include "wx/cmdline.h"
#include "../version.h"
#include "wx/stdpaths.h"
#include "wx/fileconf.h"

IMPLEMENT_APP(MainApp)

static const wxCmdLineEntryDesc g_cmdLineDesc[] =
{
	{ wxCMD_LINE_PARAM, NULL, NULL, "input file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_NONE }
};

MainApp::MainApp()
     : m_semAllDone(), simulate(), readyToSend()
{
    m_waitingUntilAllDone = false;
    showDragImage = false;
    mainframe = NULL;
    doingBitmapExport = false;
	glContext = NULL;
#ifdef __WXGTK__
	// On Linux with wayland, wxGTK doesn't position glCanvas frames correctly.
	// This env var has to be set explicitly to instruct gtk to only use X11.
	::setenv("GDK_BACKEND", "x11", /* replace */ true);
#endif
}

bool MainApp::OnInit()
{
#ifndef _PRODUCTION_
    logfile.open( "guilog.log" );
#endif
	loadSettings();
	
    wxFileSystem::AddHandler( new wxZipFSHandler );
	helpController = new wxHelpController;
	helpController->Initialize(appSettings.helpFile);


	//*****************************************
	//Edit by Joshua Lansford 2/15/07
	//wxCmdLineParser is all fine and great,
	//but it is over kill.  Besides if you say
	//to windows to open a cdl file with cedarls,
	//it isn't going to prefix the file with anything
	//unless you do some special options which are
	//not necisary.  Therefore the argv can be used
	//directly without passing it into a cmdLineParser
	//
	//wxString cmdFilename;
	//wxCmdLineParser cmdParser(g_cmdLineDesc, argc, argv);
	//if (cmdParser.GetParamCount() > 0) {
	//	cmdFilename = cmdParser.GetParam(0);
	//	wxFileName fName(cmdFilename);
	//	fName.Normalize(wxPATH_NORM_LONG|wxPATH_NORM_DOTS|wxPATH_NORM_TILDE|wxPATH_NORM_ABSOLUTE);
	//	cmdFilename = fName.GetFullPath();
    //}	
    string cmdFilename;
	if( argc >= 2 ){
		cmdFilename = argv[1].ToStdString();
//		logfile << "cmdFilename = " << cmdFilename << endl;
	}
	//End of edit
	//**********************************
	

    // create the main application window
    MainFrame *frame = new MainFrame(VERSION_TITLE(), cmdFilename);
    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);
    
    //**********************************************************
    //Edit by Joshua Lansford 12/31/06
    //Acording to 
    //http://www.wxwidgets.org/manuals/2.6.3/wx_wxappoverview.html#wxappoverview
    //the following function should be called at this time
    SetTopWindow(frame);
    
    mainframe = frame;
    //End of edit***********************************************
    
    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately
	
    return true;
}

void MainApp::loadSettings() {

	// Get app full path.
	char path[MAX_PATH];
	GetModuleFileNameA(NULL, path, MAX_PATH);

	// Find path to exe so that files can be loaded relative to it
	// even when the program is run from somewhere else.
	pathToExe = path;
	while (!pathToExe.empty()) {
		if (pathToExe.back() != '/' && pathToExe.back() != '\\') {
			pathToExe.pop_back();
		}
		else {
			break;
		}
	}

	if (pathToExe.find("Debug") != string::npos || pathToExe.find("Release") != string::npos) {
		pathToExe = "";
	}

	wxStandardPaths::Get().SetFileLayout(wxStandardPaths::FileLayout_XDG);
	wxFileConfig *conf = new wxFileConfig("CedarLogic");
	wxConfigBase::Set(conf);
	wxConfigBase::DontCreateOnDemand();

	wxString str;
	conf->Read("GateLib", &str, "res/cl_gatedefs.xml");
	appSettings.gateLibFile = pathToExe + str;

	conf->Read("HelpFile", &str, "res/KLS_Logic.chm");
	appSettings.helpFile = pathToExe + str;

	conf->Read("TextFont", &str, "res/arial.glf");
	appSettings.textFontFile = pathToExe + str;

	conf->Read("LastDirectory", &str);
	appSettings.lastDir = str;

	conf->Read("FrameWidth", &appSettings.mainFrameWidth, 600);
	conf->Read("FrameHeight", &appSettings.mainFrameHeight, 600);
	conf->Read("FrameLeft", &appSettings.mainFrameLeft, 20);
	conf->Read("FrameTop", &appSettings.mainFrameTop, 20);
	conf->Read("RefreshRate", &appSettings.refreshRate, 60); // ms
	conf->Read("TimeStep", &appSettings.timePerStep, 25); // ms
	timeStepMod = appSettings.timePerStep;
	conf->Read("WireConnRadius", &appSettings.wireConnRadius, 0.18f);
	conf->Read("WireConnVisible", &appSettings.wireConnVisible, true);
	conf->Read("GridlineVisible", &appSettings.gridlineVisible, true);

	// check screen coords
	wxScreenDC sdc;
	if ( appSettings.mainFrameLeft + appSettings.mainFrameWidth > sdc.GetSize().GetWidth() ||
		appSettings.mainFrameTop + appSettings.mainFrameHeight > sdc.GetSize().GetHeight() ) {

		appSettings.mainFrameWidth = appSettings.mainFrameHeight = 600;
		appSettings.mainFrameLeft = appSettings.mainFrameTop = 20;
	}
}

void MainApp::SetCurrentCanvas(wxGLCanvas *canvas)
{
	if (!glContext)
		glContext = new wxGLContext(canvas);
	glContext->SetCurrent(*canvas);
}
