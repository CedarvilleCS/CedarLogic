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

// Pedro Casanova (casanova@ujaen.es) 2020/01-02
// Functions used in some modules

// Pedro Casanova (casanova@ujaen.es) 2021/01-03
// Check only numeric digits are in string
bool chkDigits(string number)
{
	string numbers = "0123456789";
	for (unsigned int i = 0; i < number.size(); i++)
	{
		if ((int)numbers.find(number.substr(i, 1)) >= 0) continue;
		return false;
	}
	return true;
}

// Pedro Casanova (casanova@ujaen.es) 2021/01-03
// Check only hexadecimal digits are in string
bool chkHexDigits(string* number, bool uppercase)
{
	string numbers = "0123456789ABCDEFabcdef";
	for (unsigned int i = 0; i < number->size(); i++)
	{
		if ((int)numbers.find(number->substr(i, 1)) >= 0) continue;
		return false;
	}	
	if (uppercase) {
		for (unsigned int i = 0; i < number->size(); i++) {
			string c = number->substr(i, 1);
			number[0][i] = toupper(c[0]);
		}
	}
	return true;
}

// Pedro Casanova (casanova@ujaen.es) 2021/01-03
// Check only "0", "1" and optionally "X" are in string
bool chkBits(string* bits, bool uppercase)
{
	for (unsigned int i = 0; i < bits->size(); i++)
	{
		if (bits->substr(i, 1) == "0" || bits->substr(i, 1) == "1") continue;
		if (uppercase && (bits->substr(i, 1) == "x") || uppercase && (bits->substr(i, 1) == "X")) { bits[0][i] = 'X'; continue; }
		return false;
	}
	return true;
}

// Pedro Casanova (casanova@ujaen.es) 2021/01-03
// Remove spaces at the begininig and end of a string
void removeSpaces(string* text) {
	while (text->substr(0, 1) == " ")
		*text = text->substr(1);
	while (text->length()>0) {
		if (text->substr(text->length() - 1, 1) == " ")
			*text = text->substr(0, text->length() - 1);
		else
			break;
	}
}

// Pedro Casanova (casanova@ujaen.es) 2021/01-03
// Round 0.5 precision
float Round_half(float v)
{
	float retVal;
	if (v < 0)
		retVal = -int(-v * 2.0f + 0.5f) / 2.0f;
	else
		retVal = int(v * 2.0f + 0.5f) / 2.0f;

	return retVal;
}

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// Round 0.1 precision
float Round(float v)
{
	float retVal;
	if (v < 0)
		retVal = -int(-v * 10.0f + 0.5f) / 10.0f;
	else
		retVal = int(v * 10.0f + 0.5f) / 10.0f;
	return retVal;
}

// Pedro Casanova (casanova@ujaen.es) 2021/01-03
// Get the state of a wire
string GetStringState(vector<StateType> states)
{	
	if (states.size() != 1) {
		ostringstream oss;
		oss << "BUS " << states.size();
		return oss.str();
	}
	StateType st = states[0];
	if (st == ZERO) return "0";
	if (st == ONE) return "1";
	if (st == HI_Z) return "Z";
	if (st == CONFLICT) return "X";
	if (st == UNKNOWN) return "?";
	return "";
}

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

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
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

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
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
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// cl_gatedefs.xml in Resources, added extern UserLib.xml
		if (DEFAULT_MAINLIBFILE != "res")
			appSettings.mainGateLibFile = pathToExe + DEFAULT_MAINLIBFILE;
		else
			appSettings.mainGateLibFile = "res";
		appSettings.userGateLibFile = pathToExe + DEFAULT_USERLIBFILE;
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// Help file is outdated
		if (DEFAULT_HELPFILE != "")
			appSettings.helpFile = pathToExe + DEFAULT_HELPFILE;
		else
			appSettings.helpFile = "";
		// Now in resources - Pedro Casanova (casanova@ujaen.es) 2020/04-12
		if (DEFAULT_TEXTFONTFILE != "res")
			appSettings.textFontFile = pathToExe + DEFAULT_TEXTFONTFILE;
		else
			appSettings.textFontFile = "res";
		appSettings.mainFrameWidth = DEFAULT_MAINFRAMEWIDTH;
		appSettings.mainFrameHeight = DEFAULT_MAINFRAMEHEIGHT;
		appSettings.mainFrameLeft = DEFAULT_MAINFRAMELEFT;
		appSettings.mainFrameTop = DEFAULT_MAINFRAMETOP;
		appSettings.timePerStep = timeStepMod = DEFAULT_TIMEPERSTEP;		// ms
		appSettings.refreshRate = DEFAULT_REFRESHRATE;						// ms
		appSettings.wireConnRadius = DEFAULT_WIRECONNRADIUS;				// Pedro Casanova (casanova@ujaen.es) 2020/04-12	(was 0.18)
		appSettings.gridlineVisible = DEFAULT_GRIDLINEVISIBLE;		
		appSettings.wireConnVisible = DEFAULT_WIRECONNVISIBLE;				// Pedro Casanova (casanova@ujaen.es) 2020/04-12	(Changed to false)
		appSettings.wideOutline = DEFAULT_WIDEOUTLINE;						// Pedro Casanova (casanova@ujaen.es) 2020/04-12	(Addded)
		appSettings.componentCollVisible = DEFAULT_COMPONENTCOLLVISIBLE;	// Pedro Casanova (casanova@ujaen.es) 2020/04-12	(Addded)
		appSettings.adjustBitmap = DEFAULT_ADJUSTBITMAP;					// Pedro Casanova (casanova@ujaen.es) 2020/04-12	(Addded)
		appSettings.markDeprecated = DEFAULT_MARKDEPRECATED;				// Pedro Casanova (casanova@ujaen.es) 2020/04-12	(Addded)
		appSettings.mainFrameMaximized = DEFAULT_MAINFRAMEMAXIMIZE;			// Pedro Casanova (casanova@ujaen.es) 2021/01-03	(Addded)
	} else {
		// load from the file
		string line;
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// Now in resources
		// mainGateLibFile
		getline(iniFile, line, '\n');
		int pos = line.find('=', 0);
		if (line.substr(pos + 1, line.size() - (pos + 1)) == "res")
			appSettings.mainGateLibFile = "res";
		else
			appSettings.mainGateLibFile = pathToExe + line.substr(pos + 1, line.size() - (pos + 1));
		// userGateLibFile
		getline(iniFile, line, '\n');
		pos = line.find('=',0);
		appSettings.userGateLibFile = line.substr(pos+1,line.size()-(pos+1));
		// helpFile
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// Help file is outdated
		getline(iniFile, line, '\n');
		pos = line.find('=',0);
		if (line.size() - (pos + 1) != 0)
			appSettings.helpFile = pathToExe + line.substr(pos + 1, line.size() - (pos + 1));
		else
			appSettings.helpFile = "";		
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// Now in resources
		// textFontFile
		getline(iniFile, line, '\n');
		pos = line.find('=',0);
		if (line.substr(pos + 1, line.size() - (pos + 1)) == "res")
			appSettings.textFontFile = "res";
		else
			appSettings.textFontFile = pathToExe + line.substr(pos+1,line.size()-(pos+1));
		// frame maximized
		getline(iniFile, line, '\n');
		pos = line.find('=', 0);
		line = line.substr(pos + 1, line.size() - (pos + 1));
		istringstream issMaximized(line);
		issMaximized >> appSettings.mainFrameMaximized;
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
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// Wide Outlines Visible
		getline(iniFile, line, '\n');
		pos = line.find('=', 0);
		line = line.substr(pos + 1, line.size() - (pos + 1));
		istringstream isswideOutline(line);
		isswideOutline >> appSettings.wideOutline;
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// Component Collision Visible
		getline(iniFile, line, '\n');
		pos = line.find('=', 0);
		line = line.substr(pos + 1, line.size() - (pos + 1));
		istringstream issComponentCollVisible(line);
		issComponentCollVisible >> appSettings.componentCollVisible;
		// Adjust bitmaps
		getline(iniFile, line, '\n');
		pos = line.find('=', 0);
		line = line.substr(pos + 1, line.size() - (pos + 1));
		istringstream issAdjustBitmap(line);
		issAdjustBitmap >> appSettings.adjustBitmap;
		// Show Deprecated
		getline(iniFile, line, '\n');
		pos = line.find('=', 0);
		line = line.substr(pos + 1, line.size() - (pos + 1));
		istringstream issMarkDeprecated(line);
		issMarkDeprecated >> appSettings.markDeprecated;
		// wire connection radius
		getline(iniFile, line, '\n');
		pos = line.find('=', 0);
		line = line.substr(pos + 1, line.size() - (pos + 1));
		istringstream issRadius(line);
		issRadius >> appSettings.wireConnRadius;
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// Change the min value from 0.18 to 0.05, default 0.1
		if (appSettings.wireConnRadius < 0.025f || appSettings.wireConnRadius > 0.3f) appSettings.wireConnRadius = DEFAULT_WIRECONNRADIUS;

        // all done
        iniFile.close();
	}
}

// Pedro Casanova (casanova@ujaen.es) 2020/04-12
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

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
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
				if (!RegQueryValueEx(hKey, "MainGateLib", NULL, NULL, (BYTE*)Value_C, (LPDWORD)&Length) == ERROR_SUCCESS)
					strcpy(Value_C, DEFAULT_MAINLIBFILE);
				if (!strcmp(Value_C, "res"))
					appSettings.mainGateLibFile = "res";
				else
					appSettings.mainGateLibFile = pathToExe + Value_C;

				Length = MAX_PATH;
				if (RegQueryValueEx(hKey, "UserGateLib", NULL, NULL, (BYTE*)Value_C, (LPDWORD)&Length) == ERROR_SUCCESS)
					appSettings.userGateLibFile = Value_C;
				else
					appSettings.userGateLibFile = pathToExe + DEFAULT_USERLIBFILE;
				Length = MAX_PATH;
				if (!RegQueryValueEx(hKey, "TextFont", NULL, NULL, (BYTE*)Value_C, (LPDWORD)&Length) == ERROR_SUCCESS)
					strcpy(Value_C, DEFAULT_TEXTFONTFILE);
				if (!strcmp(Value_C,"res"))
					appSettings.textFontFile = "res";
				else
					appSettings.textFontFile = pathToExe + Value_C;
				Length = MAX_PATH;
				// Pedro Casanova (casanova@ujaen.es) 2020/04-12
				// Help file is outdated
				if (!RegQueryValueEx(hKey, "HelpFile", NULL, NULL, (BYTE*)Value_C, (LPDWORD)&Length) == ERROR_SUCCESS)
					strcpy(Value_C, DEFAULT_HELPFILE);
				if (strlen(Value_C)!=0)
					appSettings.helpFile = pathToExe + Value_C;
				else
					appSettings.helpFile = "";
				Length = MAX_PATH;
				if (RegQueryValueEx(hKey, "LastDirectory", NULL, NULL, (BYTE*)Value_C, (LPDWORD)&Length) == ERROR_SUCCESS)
					appSettings.lastDir = Value_C;
				else
				{
					// Pedro Casanova (casanova@ujaen.es) 2020/04-12
					// Set default directory to "My Documents"
					char myDocs[MAX_PATH] = "";
					SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, myDocs);
					appSettings.lastDir = myDocs;
					//appSettings.lastDir = wxGetHomeDir();
				}
				Length = 4;
				if (RegQueryValueEx(hKey, "FrameMaximized", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = (DEFAULT_MAINFRAMEMAXIMIZE == true) ? 1 : 0;
				appSettings.mainFrameMaximized = (Value == 0) ? false : true;
				Length = 4;
				if (RegQueryValueEx(hKey, "FrameWidth", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = DEFAULT_MAINFRAMEWIDTH;
				appSettings.mainFrameWidth = (unsigned int)Value;
				Length = 4;
				if (RegQueryValueEx(hKey, "FrameHeight", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = DEFAULT_MAINFRAMEHEIGHT;
				appSettings.mainFrameHeight = (unsigned int)Value;
				Length = 4;
				if (RegQueryValueEx(hKey, "FrameLeft", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = DEFAULT_MAINFRAMELEFT;
				appSettings.mainFrameLeft = Value;
				Length = 4;
				if (RegQueryValueEx(hKey, "FrameTop", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = DEFAULT_MAINFRAMETOP;
				appSettings.mainFrameTop = Value;
				Length = 4;
				if (RegQueryValueEx(hKey, "TimeStep", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = DEFAULT_TIMEPERSTEP;
				appSettings.timePerStep = timeStepMod = (unsigned int)Value;
				Length = 4;
				if (RegQueryValueEx(hKey, "RefreshRate", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = DEFAULT_REFRESHRATE;
				appSettings.refreshRate = Value;
				if (appSettings.refreshRate <= 0) appSettings.refreshRate = 60;
				Length = 4;
				if (RegQueryValueEx(hKey, "WireConnRadius", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = ceil(100* DEFAULT_WIRECONNRADIUS);
				appSettings.wireConnRadius = ((float)Value) / 1000.0f;
				if (appSettings.wireConnRadius < 0.025f || appSettings.wireConnRadius > 0.3f) appSettings.wireConnRadius = DEFAULT_WIRECONNRADIUS;
				Length = 4;
				if (RegQueryValueEx(hKey, "GridLineVisible", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = (DEFAULT_GRIDLINEVISIBLE == true) ? 1 : 0;
				appSettings.gridlineVisible = (Value == 0) ? false : true;
				Length = 4;
				if (RegQueryValueEx(hKey, "WireConnVisible", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = (DEFAULT_WIRECONNVISIBLE == true) ? 1 : 0;
				appSettings.wireConnVisible = (Value == 0) ? false : true;
				Length = 4;
				if (RegQueryValueEx(hKey, "WideOutline", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = (DEFAULT_WIDEOUTLINE == true) ? 1 : 0;
				appSettings.wideOutline = (Value == 0) ? false : true;
				Length = 4;
				if (RegQueryValueEx(hKey, "ComponentCollVisible", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = (DEFAULT_COMPONENTCOLLVISIBLE == true) ? 1 : 0;
				appSettings.componentCollVisible = (Value == 0) ? false : true;
				if (RegQueryValueEx(hKey, "AdjustBitmap", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = (DEFAULT_ADJUSTBITMAP == true) ? 1 : 0;
				appSettings.adjustBitmap = (Value == 0) ? false : true;
				if (RegQueryValueEx(hKey, "MarkDeprecated", NULL, NULL, (BYTE*)&Value, (LPDWORD)&Length) != ERROR_SUCCESS)
					Value = (DEFAULT_MARKDEPRECATED == true) ? 1 : 0;
				appSettings.markDeprecated = (Value == 0) ? false : true;
			}
		RegCloseKey(hKey);
		if (newVersion)
			RegDeleteKey(HKEY_CURRENT_USER, "Software\\CedarLogic");
	}
	if (newVersion) {
		// set defaults
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// cl_gatedefs.xm in Resources, added extern UserLib.xml		
		if (DEFAULT_MAINLIBFILE != "res")
			appSettings.mainGateLibFile = pathToExe + DEFAULT_MAINLIBFILE;
		else
			appSettings.mainGateLibFile = "res";
		appSettings.userGateLibFile = pathToExe + DEFAULT_USERLIBFILE;
		// Pedro Casanova (casanova@ujaen.es) 2020/04-12
		// Help file is outdated
		if (DEFAULT_HELPFILE != "")
			appSettings.helpFile = pathToExe + DEFAULT_HELPFILE;
		else
			appSettings.helpFile = "";
		// Now in resources - Pedro Casanova (casanova@ujaen.es) 2020/04-12
		if (DEFAULT_TEXTFONTFILE != "res")
			appSettings.textFontFile = pathToExe + DEFAULT_TEXTFONTFILE;
		else
			appSettings.textFontFile = "res";
		appSettings.mainFrameWidth = DEFAULT_MAINFRAMEWIDTH;
		appSettings.mainFrameHeight = DEFAULT_MAINFRAMEHEIGHT;
		appSettings.mainFrameLeft = DEFAULT_MAINFRAMELEFT;
		appSettings.mainFrameTop = DEFAULT_MAINFRAMETOP;		
		appSettings.timePerStep = timeStepMod = DEFAULT_TIMEPERSTEP;		// ms
		appSettings.refreshRate = DEFAULT_REFRESHRATE;						// ms
		appSettings.wireConnRadius = DEFAULT_WIRECONNRADIUS;				// Pedro Casanova (casanova@ujaen.es) 2020/04-12	(was 0.18)
		appSettings.gridlineVisible = DEFAULT_GRIDLINEVISIBLE;
		appSettings.wireConnVisible = DEFAULT_WIRECONNVISIBLE;				// Pedro Casanova (casanova@ujaen.es) 2020/04-12	(Changed to false)
		appSettings.wideOutline = DEFAULT_WIDEOUTLINE;						// Pedro Casanova (casanova@ujaen.es) 2020/04-12	(Addded)
		appSettings.componentCollVisible = DEFAULT_COMPONENTCOLLVISIBLE;	// Pedro Casanova (casanova@ujaen.es) 2020/04-12	(Addded)
		appSettings.adjustBitmap = DEFAULT_ADJUSTBITMAP;					// Pedro Casanova (casanova@ujaen.es) 2020/04-12	(Addded)
		appSettings.markDeprecated = DEFAULT_MARKDEPRECATED;				// Pedro Casanova (casanova@ujaen.es) 2020/04-12	(Addded)
		appSettings.mainFrameMaximized = DEFAULT_MAINFRAMEMAXIMIZE;			// Pedro Casanova (casanova@ujaen.es) 2021/01-03	(Added)
	}


	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// .CDL extension file association
	// Not necesary, made by the installer in HKCR
/*	if (RegOpenKey(HKEY_CURRENT_USER, "Software\\Classes\\.cdl", &hKey) != ERROR_SUCCESS)
		if (RegCreateKey(HKEY_CURRENT_USER, "Software\\Classes\\.cdl", &hKey) == ERROR_SUCCESS)
		{
			RegSetValue(hKey, "", REG_SZ, "CedarLogic", 0);
			RegCloseKey(hKey);
			if (RegCreateKey(HKEY_CURRENT_USER, "Software\\Classes\\.cdl_auto_file_CedarLogic\\shell\\open\\command", &hKey) == ERROR_SUCCESS)
			{
				strcat_s(AppFilePath, " \"%1\"");
				RegSetValue(hKey, "", REG_SZ, AppFilePath, 0);
				RegCloseKey(hKey);
			}
			else
				RegDeleteKey(HKEY_CURRENT_USER, "Software\\Classes\\.cdl");
		}*/
}

void MainApp::loadSettings(bool Reg) {

	// Get Settings
	if (Reg)
		loadSettingsReg();
	else
		loadSettingsFile();

	// check screen coords
	wxScreenDC sdc;

	if (appSettings.mainFrameLeft < 0 || appSettings.mainFrameTop <0)
		appSettings.mainFrameLeft = appSettings.mainFrameTop = 20;
	if (appSettings.mainFrameLeft + (signed int)(appSettings.mainFrameWidth) > (signed int)(sdc.GetSize().GetWidth()) ||
		appSettings.mainFrameTop + (signed int)(appSettings.mainFrameHeight) > (signed int)(sdc.GetSize().GetHeight())) {

		appSettings.mainFrameWidth = 1100;
		appSettings.mainFrameHeight = 800;
		appSettings.mainFrameLeft = appSettings.mainFrameTop = 20;
	}
}
