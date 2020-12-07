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
}

bool MainApp::OnInit()
{
#ifndef _PRODUCTION_
    logfile.open( "guilog.log" );
#endif
	loadSettings(appSettings.settingsInReg);
	
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
		// inserted the cast  KAS
		cmdFilename = (const char *)argv[1];
//		logfile << "cmdFilename = " << cmdFilename << endl;
	}
	//End of edit
	//**********************************
	

    // create the main application window
    MainFrame *frame = new MainFrame(VERSION_TITLE(), cmdFilename.c_str());
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

// Pedro Casanova (casanova@ujaen.es) 2020/04-10
// Not used, now in windows register
void MainApp::loadSettingsFile() {

	// Get app full path.
	HMODULE hModule = GetModuleHandle(NULL);
	CHAR path[MAX_PATH];
	GetModuleFileName(hModule, path, MAX_PATH);

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

	// Pedro Casanova (casanova@ujaen.es) 2020/04-10
	// To make test this is a problem
#ifndef _PRODUCTION_
	if (pathToExe.find("Debug") != string::npos || pathToExe.find("Release") != string::npos) {
		pathToExe = "";
	}
#endif
	
	string settingsIni = pathToExe + "settings.ini";
	ifstream iniFile( settingsIni.c_str(), ios::in );
	if (!iniFile) {
		// set defaults
		// Pedro Casanova (casanova@ujaen.es) 2020/04-10
		// cl_gatedefs.xm in Resources, added extern UserLib.xml
		appSettings.gateLibFile = pathToExe + "UserLib.xml";
		// Help is obsoleted - Pedro Casanova (casanova@ujaen.es) 2020/04-10
		appSettings.helpFile = pathToExe + "KLS_Logic.chm";
		// Now in resources - Pedro Casanova (casanova@ujaen.es) 2020/04-10
		if (appSettings.textFontFile == "res")
			appSettings.textFontFile = "res";
		else
			appSettings.textFontFile = pathToExe + "arial.glf";
		appSettings.mainFrameWidth = 1000;
		appSettings.mainFrameHeight = 800;
		appSettings.mainFrameLeft = appSettings.mainFrameTop = 20;
		appSettings.timePerStep = timeStepMod = 25; // ms
		appSettings.refreshRate = 60; // ms				
		appSettings.gridlineVisible = true;
		appSettings.wireConnVisible = false;	// Pedro Casanova (casanova@ujaen.es) 2020/04-10		(Changed to false)
		appSettings.componentCollVisible = false; // Pedro Casanova (casanova@ujaen.es) 2020/04-10	(Addded)
		appSettings.wireConnRadius = 0.175f; // Pedro Casanova (casanova@ujaen.es) 2020/04-10  (was 0.18)
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
		// Pedro Casanova (casanova@ujaen.es) 2020/04-10
		// Now in resources
		// textFontFile
		getline(iniFile, line, '\n');
		pos = line.find('=',0);
		if (line.substr(pos + 1, line.size() - (pos + 1)) == "res")
			appSettings.textFontFile = "res";
		else
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
		// grid visible
		getline(iniFile, line, '\n');
		pos = line.find('=', 0);
		line = line.substr(pos + 1, line.size() - (pos + 1));
		istringstream issGridVisible(line);
		issGridVisible >> appSettings.gridlineVisible;
		// wire connection visible
        getline(iniFile, line, '\n');
        pos = line.find('=',0);
        line = line.substr(pos+1,line.size()-(pos+1));
        istringstream issWireConnVisible(line);
        issWireConnVisible >> appSettings.wireConnVisible;
		// Pedro Casanova (casanova@ujaen.es) 2020/04-10
		// Component Collision Visible
		getline(iniFile, line, '\n');
		pos = line.find('=', 0);
		line = line.substr(pos + 1, line.size() - (pos + 1));
		istringstream issComponentCollVisible(line);
		issComponentCollVisible >> appSettings.componentCollVisible;
		// wire connection radius
		getline(iniFile, line, '\n');
		pos = line.find('=', 0);
		line = line.substr(pos + 1, line.size() - (pos + 1));
		istringstream issRadius(line);
		issRadius >> appSettings.wireConnRadius;
		// Pedro Casanova (casanova@ujaen.es) 2020/04-10
		// Change the min value from 0.18 to 0.05, default 0.1
		if (appSettings.wireConnRadius < 0.025f || appSettings.wireConnRadius > 0.3f) appSettings.wireConnRadius = 0.175f;

        // all done
        iniFile.close();
	}
}

// Pedro Casanova (casanova@ujaen.es) 2020/04-10
// Settings in windows register
void MainApp::loadSettingsReg() {

	// Get app full path.
	HMODULE hModule = GetModuleHandle(NULL);
	CHAR AppFilePath[MAX_PATH];
	GetModuleFileName(hModule, AppFilePath, MAX_PATH);

	// Find path to exe so that files can be loaded relative to it
	// even when the program is run from somewhere else.
	pathToExe = AppFilePath;
	while (!pathToExe.empty()) {
		if (pathToExe.back() != '/' && pathToExe.back() != '\\') {
			pathToExe.pop_back();
		}
		else {
			break;
		}
	}

	// Pedro Casanova (casanova@ujaen.es) 2020/04-10
	// To make test this is a problem
#ifndef _PRODUCTION_
	if (pathToExe.find("Debug") != string::npos || pathToExe.find("Release") != string::npos) {
		pathToExe = "";
	}
#endif

	HKEY hKey;
	char Value_C[MAX_PATH];
	int Value=0;
	unsigned int Length=MAX_PATH;
	bool newVersion = true;
	if (RegOpenKey(HKEY_CURRENT_USER, "Software\\CedarLogic", &hKey) == ERROR_SUCCESS) {
		Length = MAX_PATH;
		if (RegQueryValue(hKey, NULL, Value_C, (PLONG)&Length) == ERROR_SUCCESS)
			if (!strcmp(Value_C, VERSION_TITLE().c_str())) {
				newVersion = false;
				Length = MAX_PATH;
				if (RegQueryValueEx(hKey, "GateLib", NULL, NULL, (BYTE*)Value_C, (LPDWORD)&Length) == ERROR_SUCCESS)
					appSettings.gateLibFile = pathToExe + Value_C;
				else
					appSettings.gateLibFile = pathToExe + "UserLib.xml";
				Length = MAX_PATH;
				if (RegQueryValueEx(hKey, "HelpFile", NULL, NULL, (BYTE*)Value_C, (LPDWORD)&Length) == ERROR_SUCCESS)
					appSettings.helpFile = pathToExe + Value_C;
				else
					appSettings.helpFile = pathToExe + "KLS_Logic.chm";
				Length = MAX_PATH;
				if (RegQueryValueEx(hKey, "LastDirectory", NULL, NULL, (BYTE*)Value_C, (LPDWORD)&Length) == ERROR_SUCCESS)
					appSettings.lastDir = Value_C;
				else
				{
					// Pedro Casanova (casanova@ujaen.es) 2020/04-10
					// Set default directory to "My Documents"
					char myDocs[MAX_PATH] = "";
					SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, myDocs);
					appSettings.lastDir = myDocs;
					//appSettings.lastDir = wxGetHomeDir();
				}
				Length = 4;
				if (RegQueryValueEx(hKey, "FrameWidth", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = 1000;
				appSettings.mainFrameWidth = (unsigned int)Value;
				Length = 4;
				if (RegQueryValueEx(hKey, "FrameHeight", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = 800;
				appSettings.mainFrameHeight = (unsigned int)Value;
				Length = 4;
				if (RegQueryValueEx(hKey, "FrameLeft", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = 20;
				appSettings.mainFrameLeft = Value;
				Length = 4;
				if (RegQueryValueEx(hKey, "FrameTop", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = 20;
				appSettings.mainFrameTop = Value;
				Length = 4;
				if (RegQueryValueEx(hKey, "TimeStep", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = 25;
				appSettings.timePerStep = timeStepMod = (unsigned int)Value;
				Length = 4;
				if (RegQueryValueEx(hKey, "RefreshRate", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = 60;
				appSettings.refreshRate = Value;
				if (appSettings.refreshRate <= 0) appSettings.refreshRate = 60;
				Length = 4;
				if (RegQueryValueEx(hKey, "WireConnRadius", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = 175;
				appSettings.wireConnRadius = ((float)Value) / 1000.0f;
				if (appSettings.wireConnRadius < 0.025f || appSettings.wireConnRadius > 0.3f) appSettings.wireConnRadius = 0.175f;
				Length = 4;
				if (RegQueryValueEx(hKey, "GridLineVisible", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = 1;
				appSettings.gridlineVisible = (Value == 0) ? false : true;
				Length = 4;
				if (RegQueryValueEx(hKey, "WireConnVisible", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = 0;
				appSettings.wireConnVisible = (Value == 0) ? false : true;
				Length = 4;
				if (RegQueryValueEx(hKey, "ComponentCollVisible", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = 0;
				appSettings.componentCollVisible = (Value == 0) ? false : true;
			}
		RegCloseKey(hKey);
		if (newVersion)
			RegDeleteKey(HKEY_CURRENT_USER, "Software\\CedarLogic");
	}
	if (newVersion) {
		// set defaults
		// Pedro Casanova (casanova@ujaen.es) 2020/04-10
		// cl_gatedefs.xml in Resources, added extern UserLib.xml
		appSettings.gateLibFile = pathToExe + "UserLib.xml";
		// Help is obsoleted - Pedro Casanova (casanova@ujaen.es) 2020/04-10
		appSettings.helpFile = pathToExe + "KLS_Logic.chm";
		// Now in resources - Pedro Casanova (casanova@ujaen.es) 2020/04-10
		if (appSettings.textFontFile!="res")
			appSettings.textFontFile = pathToExe + "arial.glf";
		appSettings.mainFrameWidth = 1000;
		appSettings.mainFrameHeight = 800;
		appSettings.mainFrameLeft = 20;
		appSettings.mainFrameTop = 20;
		appSettings.timePerStep = timeStepMod = 25; // ms
		appSettings.refreshRate = 60; // ms				
		appSettings.gridlineVisible = true;
		appSettings.wireConnVisible = false;		// Changed to false -  Pedro Casanova (casanova@ujaen.es) 2020/04-10
		appSettings.componentCollVisible = false;	// Pedro Casanova (casanova@ujaen.es) 2020/04-10
		appSettings.wireConnRadius = 0.175f;		// Pedro Casanova (casanova@ujaen.es) 2020/04-10  (was 0.18f)
	}

	// Pedro Casanova (casanova@ujaen.es) 2020/04-10
	// .CDL extension file association
	if (RegCreateKey(HKEY_CURRENT_USER, "Software\\Classes\\.cdl", &hKey) == ERROR_SUCCESS)
	{
		RegSetValue(hKey, "", REG_SZ, ".cdl_auto_file", 0);
		RegCloseKey(hKey);
		if (RegCreateKey(HKEY_CURRENT_USER, "Software\\Classes\\.cdl_auto_file\\shell\\open\\command", &hKey) == ERROR_SUCCESS)
		{
			strcat_s(AppFilePath, " \"%1\"");
			RegSetValue(hKey, "", REG_SZ, AppFilePath, 0);
			RegCloseKey(hKey);
		}
		else
			RegDeleteKey(HKEY_CURRENT_USER, "Software\\Classes\\.cdl");
	}
}

void MainApp::loadSettings(bool Reg) {

	// Get Settings
	if (Reg)
		loadSettingsReg();
	else
		loadSettingsFile();

	// Pedro Casanova (casanova@ujaen.es) 2020/04-10
	// Initial Settings
	/*_MSG1("GATELIB %s", appSettings.gateLibFile.c_str());
	_MSG1("HELPFILE %s", appSettings.helpFile.c_str());
	_MSG1("LASTDIR %s", appSettings.lastDir.c_str());
	_MSG1("FONT %s", appSettings.textFontFile.c_str());
	_MSG1("WIDTH %d", appSettings.mainFrameWidth);
	_MSG1("HEIGHT %d", appSettings.mainFrameHeight);
	_MSG1("LEFT %d", appSettings.mainFrameLeft);
	_MSG1("TOP %d", appSettings.mainFrameTop);
	_MSG1("TIME %d", appSettings.timePerStep);
	_MSG1("RADIUS %1.3f", appSettings.wireConnRadius);
	_MSG1("GRID %d", appSettings.gridlineVisible);
	_MSG1("CONN %d", appSettings.wireConnVisible);
	_MSG1("COLL %d", appSettings.componentCollVisible);	*/


	// check screen coords
	wxScreenDC sdc;

	if (appSettings.mainFrameLeft < 0 || appSettings.mainFrameTop <0)
		appSettings.mainFrameLeft = appSettings.mainFrameTop = 20;
	if (appSettings.mainFrameLeft + (signed int)(appSettings.mainFrameWidth) > (signed int)(sdc.GetSize().GetWidth()) ||
		appSettings.mainFrameTop + (signed int)(appSettings.mainFrameHeight) > (signed int)(sdc.GetSize().GetHeight())) {

		appSettings.mainFrameWidth = 1000;
		appSettings.mainFrameHeight = 800;
		appSettings.mainFrameLeft = appSettings.mainFrameTop = 20;
	}
}
