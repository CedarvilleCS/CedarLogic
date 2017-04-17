
#pragma once

#include <string>
#include "wx/wx.h"
#include "wx/thread.h"
#include "wx/image.h"
#include "wx/docview.h"
#include "wx/help.h"
#include "wx/fs_zip.h"
#include "common.h"
#include "parse/GateLibrary.h"

class MainFrame;
class threadLogic;
class autoSaveThread;
class Message;

using namespace std;

struct ApplicationSettings {
	string gateLibFile;
	string textFontFile;
	string helpFile;
	string lastDir;
	unsigned int mainFrameWidth;
	unsigned int mainFrameHeight;
	int mainFrameLeft;
	int mainFrameTop;
	unsigned int timePerStep;
	int refreshRate;
    float wireConnRadius;
    bool wireConnVisible;
	bool gridlineVisible;
	bool autoIncrement;
};

class MainApp : public wxApp {
public:
	MainApp();
	virtual bool OnInit();

    // crit section protects access to all of the arrays below
    wxCriticalSection m_critsect;

    // semaphore used to wait for the threads to exit, see MainFrame::OnQuit()
    wxSemaphore m_semAllDone;
	wxSemaphore simulate;
	wxSemaphore readyToSend;

	

	// Use a stopwatch for timing between step calls
	wxStopWatch appSystemTime;
	unsigned long timeStepMod;
	
	// We need to have a map of libraries for palette organization, and knowledge
	//	of which one we are currently displaying.  We also have a map of which gate
	//	belongs to which library (child to parent)
	string currentLibrary;
	GateLibrary libParser;
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

	autoSaveThread* saveThread;
	
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
	void loadSettings();

	// Load colors if they exist, or write a default colors file for future use.
	void loadColors();
};