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
	
	string settingsIni = pathToExe + "res/settings.ini";
	ifstream iniFile( settingsIni.c_str(), ios::in );
	
	if (!iniFile) {
		// set defaults
		appSettings.gateLibFile = pathToExe + "res/cl_gatedefs.xml";
		appSettings.helpFile = pathToExe + "res/KLS_Logic.chm";
		appSettings.textFontFile = pathToExe + "res/arial.glf";
		appSettings.mainFrameWidth = appSettings.mainFrameHeight = 600;
		appSettings.mainFrameLeft = appSettings.mainFrameTop = 20;
		appSettings.timePerStep = timeStepMod = 25; // ms
		appSettings.refreshRate = 60; // ms
		appSettings.wireConnRadius = 0.18f;
		appSettings.wireConnVisible = true;
		appSettings.gridlineVisible = true;
	} else {
		// load from the file
		string line;
		// gateLibFile
		getline(iniFile, line, '\n');
		int pos = line.find('=',0);
		appSettings.gateLibFile = pathToExe + line.substr(pos+1,line.size()-(pos+1));
		// helpFile
		getline(iniFile, line, '\n');
		pos = line.find('=',0);
		appSettings.helpFile = pathToExe + line.substr(pos+1,line.size()-(pos+1));
		// textFontFile
		getline(iniFile, line, '\n');
		pos = line.find('=',0);
		appSettings.textFontFile = pathToExe + line.substr(pos+1,line.size()-(pos+1));
		// frame width
		getline(iniFile, line, '\n');
		pos = line.find('=',0);
		line = line.substr(pos+1,line.size()-(pos+1));
		istringstream issWidth(line);
		issWidth >> appSettings.mainFrameWidth;
		// frame height
		getline(iniFile, line, '\n');
		pos = line.find('=',0);
		line = line.substr(pos+1,line.size()-(pos+1));
		istringstream issHeight(line);
		issHeight >> appSettings.mainFrameHeight;
		// frame left
		getline(iniFile, line, '\n');
		pos = line.find('=',0);
		line = line.substr(pos+1,line.size()-(pos+1));
		istringstream issLeft(line);
		issLeft >> appSettings.mainFrameLeft;
		// frame top
		getline(iniFile, line, '\n');
		pos = line.find('=',0);
		line = line.substr(pos+1,line.size()-(pos+1));
		istringstream issTop(line);
		issTop >> appSettings.mainFrameTop;
		// time per step
		getline(iniFile, line, '\n');
		pos = line.find('=',0);
		line = line.substr(pos+1,line.size()-(pos+1));
		istringstream issTime(line);
		issTime >> appSettings.timePerStep;
		timeStepMod = appSettings.timePerStep;
		// force refresh rate
		getline(iniFile, line, '\n');
		pos = line.find('=',0);
		line = line.substr(pos+1,line.size()-(pos+1));
		istringstream issRefresh(line);
		issRefresh >> appSettings.refreshRate;
		if (appSettings.refreshRate <= 0) appSettings.refreshRate = 60; // Check for valid refresh rate (0 or less is bad)
		// LastDirectory
		getline(iniFile, line, '\n');
		pos = line.find('=',0);
		appSettings.lastDir = line.substr(pos+1,line.size()-(pos+1));
        // wire connection radius
        getline(iniFile, line, '\n');
        pos = line.find('=',0);
        line = line.substr(pos+1,line.size()-(pos+1));
        istringstream issRadius(line);
        issRadius >> appSettings.wireConnRadius;
        if (appSettings.wireConnRadius <= 0 || appSettings.wireConnRadius > 0.3) appSettings.wireConnRadius = 0.18f;
        // wire connection visible
        getline(iniFile, line, '\n');
        pos = line.find('=',0);
        line = line.substr(pos+1,line.size()-(pos+1));
        istringstream issWireConnVisible(line);
        issWireConnVisible >> appSettings.wireConnVisible;
        // grid visible
        getline(iniFile, line, '\n');
        pos = line.find('=',0);
        line = line.substr(pos+1,line.size()-(pos+1));
        istringstream issGridVisible(line);
        issGridVisible >> appSettings.gridlineVisible;

        // all done
        iniFile.close();
		
		// check screen coords
		wxScreenDC sdc;
		if ( appSettings.mainFrameLeft + (signed int)(appSettings.mainFrameWidth) > (signed int)(sdc.GetSize().GetWidth()) ||
			appSettings.mainFrameTop + (signed int)(appSettings.mainFrameHeight) > (signed int)(sdc.GetSize().GetHeight()) ) {
		
			appSettings.mainFrameWidth = appSettings.mainFrameHeight = 600;
			appSettings.mainFrameLeft = appSettings.mainFrameTop = 20;	
		}

	}
}

void MainApp::SetCurrentCanvas(wxGLCanvas *canvas)
{
	if (!glContext)
		glContext = new wxGLContext(canvas);
	glContext->SetCurrent(*canvas);
}
