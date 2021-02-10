/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   MainApp: Main application object
*****************************************************************************/

#ifndef MAINAPP_H_
#define MAINAPP_H_

#include "wx/wxprec.h"
#include "wx/wx.h"
#include "wx/thread.h"
#include "wx/image.h"
#include "wx/docview.h"
#include "wx/help.h"
#include "wx/fs_zip.h"
#include "threadLogic.h"
#include "autoSaveThread.h"
#include "../logic/logic_values.h"
#include "LibraryParse.h"
#include "gl_defs.h"
#include "klsMessage.h"
#include "settings_values.h"
#include <deque>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
// Pedro Casanova (casanova@ujaen.es) 2020/04-12
// To search "My Documents" directory
#include <shlobj.h>

// Pedro Casanova (casanova@ujaen.es) 2020/01-02
// Functions used in some modules
bool chkDigits(string number);
bool chkHexDigits(string* number, bool uppercase = false);
bool chkBits(string* bits, bool uppercase = false);
float Round_half(float v);
float Round(float v);

class MainFrame;

using namespace std;

struct ApplicationSettings {
	string gateLibFile;
	string textFontFile = "res";		// Pedro Casanova (casanova@ujaen.es) 2020/04-12	Font is now in resources 
	string helpFile;
	string lastDir;
	unsigned int mainFrameWidth;
	unsigned int mainFrameHeight;
	int mainFrameLeft;
	int mainFrameTop;
	unsigned int timePerStep;
	int refreshRate;    
	bool gridlineVisible;
    bool wireConnVisible; 
	bool wideOutline;
	bool componentCollVisible;
	bool adjustBitmap;
	bool markDeprecated;
	float wireConnRadius;	
	bool settingsInReg = true;			// Pedro Casanova (casanova@ujaen.es) 2020/04-12	Settings in register
};

class MainApp : public wxApp {
public:
	MainApp();
	virtual bool OnInit();

public:
    // crit section protects access to all of the arrays below
    wxCriticalSection m_critsect;

    // semaphore used to wait for the threads to exit, see MainFrame::OnQuit()
    wxSemaphore m_semAllDone;
	wxSemaphore simulate;
	wxSemaphore readyToSend;

	wxMutex mexMessages;
	deque< klsMessage::Message > dGUItoLOGIC;
	deque< klsMessage::Message > dLOGICtoGUI;
	// Use a stopwatch for timing between step calls
	wxStopWatch appSystemTime;
	unsigned long timeStepMod;
	
	// We need to have a map of libraries for palette organization, and knowledge
	//	of which one we are currently displaying.  We also have a map of which gate
	//	belongs to which library (child to parent)
	string currentLibrary;
	LibraryParse libParser;
	map < string, map < string, LibraryGate > > libraries;
	map < string, string > gateNameToLibrary;
		
    // the last exiting thread should post to m_semAllDone if this is true
    // (protected by the same m_critsect)
    bool m_waitingUntilAllDone;
    
    // Help system
    wxHelpController* helpController;
    
    bool showDragImage;
	string newGateToDrag;
	
	ApplicationSettings appSettings;
	
	threadLogic* logicThread;
	autoSaveThread* saveThread;
	
	ofstream logfile;
	
	//this pointer is added so that pop-ups can
	//resume simulation
	MainFrame* mainframe;
	
	//this string is necisary when the working directory
	//is not were the executeable is.
	string pathToExe;

	// OK, honestly, this shouldn't be here
	//	Basically exporting bitmaps doesn't like GL display
	//	lists, so we flag them
	bool doingBitmapExport;
	
private:

	// Pedro Casanova (casanova@ujaen.es) 2020/04-12
	// Settings now in windows register
	void loadSettingsFile(void);
	void loadSettingsReg(void);
	void loadSettings(bool Reg);	
};

#endif /*MAINAPP_H_*/
