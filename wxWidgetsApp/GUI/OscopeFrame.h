/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   OscopeFrame: Window frame for the Oscope
*****************************************************************************/

#ifndef OSCOPEFRAME_H_
#define OSCOPEFRAME_H_

class GUICircuit;
class OscopeFrame;

#include "MainApp.h"
#include "PaletteCanvas.h"
#include "wx/wxprec.h"
#include "wx/thread.h"
#include "wx/tglbtn.h"
#include "threadLogic.h"
#include "OscopeCanvas.h"
#include "wx/combobox.h"
//Josh Edit 3/15/07
#include "wx/event.h"
#include <vector>


//Josh Edit 3/15/07
#define NONE_STR "[None]"
#define RMOVE_STR "[Remove]"

enum
{
	// Toggle button ID:
    ID_PAUSE_BUTTON
};

using namespace std;

class GUICircuit;

class OscopeFrame : public wxFrame {
public:
    // constructor(s)
    
    OscopeFrame(wxWindow *parent, const wxString& title, GUICircuit* gCircuit);
	
	void UpdateData(void);
	void UpdateMenu(void);
	
	void OnToggleButton( wxCommandEvent& event );
	void OnExport( wxCommandEvent& event );
	void OnLoad( wxCommandEvent& event );
	void OnSave( wxCommandEvent& event );
	
	// Hide, but don't close the frame:
	void OnClose( wxCloseEvent& event );
	
	//Function used to update menu when comboBox is changed
	void OnComboUpdate( wxCommandEvent& event );
	
	//*******************************
	//Edit by Joshua Lansford 3/11/06
	//In my effort to remove a bug that made
	//the o-scope insensative to case of
	//to pointers.  To help find the bug,
	//I did some cleaning up by making the
	//vector of the combo boxes private
	//and providing access methods for them
	//instead.
	
	//This makes sure that our vector of combo boxes
	//are sorted in the same order as the way the combo
	//boxes apear on the screen
	void verifyReferenceOrder();
	
	//creates a new feed by the specified name
	//and appends it to the end.
	void appendNewFeed( string newName );
	
	//this renames an existing feed
	void setFeedName( int i, string newName );
	
	//Returns how many active feeds there are in the
	//Oscope. i.e. how many combo boxes there are
	unsigned int numberOfFeeds();
	
	//takes a feed out with the combo box it was in
	void removeFeed( int i );
	
	//Returns the name of feed i.  i.e. the current
	//contents in the ith combo box
	string getFeedName( int i );
	
	//sets the name of a feed to none
	void cancelFeed( int i );
	
	//returns the y location of this feed in
	//the canves
	int getFeedYPos( int i );
	
	//This will cause the OscopeFrame
	//to update the list of
	void updatePossableFeeds( vector< string >* newPossabilities );
	

	
private:

	//This method takes a read pointer name
	//and returns a posably different name
	//(a name with more spaces on the end)
	//so that this name will be unique
	//from any other name despice case difference
	string encodeFeedName( string name );
	
	//This undoes the encoding done in encodeFeed
	string decodeFeedName( string name );
	
	//this is used by encodeFeedName to check
	//if two names are the same if you
	//disregard the case
	string toUpperCase( string name );

	//this allows a lookup of an encoded name
	//to determine the real name that it had
	map< string, string > encNameToRealName;
	
	//this does a lookup the otherway of
	//encNameToRealName
	map< string, string > realNameToEncName;
	
	//This holdes all the encoded names
	//in upper case.  This is used by
	//encodeFeedName to make sure the name
	//is unique
	map< string, bool > uppercasedEncNames;

	//End of edit***********************
	
	
	//Declared Combo Box
	 vector<wxComboBox*> comboBoxes;
	 
	GUICircuit* gCircuit;
	OscopeCanvas* theCanvas;
	wxBoxSizer* oSizer;
	wxGridSizer* vSizer;
	wxGridSizer* buttonSizer;
	wxToggleButton* pauseButton;
	wxButton* exportButton;
	wxButton* loadButton;
	wxButton* saveButton;
	
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

#endif /*OSCOPEFRAME_H_*/
