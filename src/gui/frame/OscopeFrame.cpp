/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   OscopeFrame: Window frame for the Oscope
*****************************************************************************/

#include <fstream>
#include <iomanip>
#include "../MainApp.h"
#include "OscopeFrame.h"
#include "wx/filedlg.h"
#include "wx/timer.h"
#include "wx/thread.h"
#include "wx/settings.h"
#include "../parse/CircuitParse.h"
#include "wx/combobox.h"
#include "wx/clipbrd.h"
#include "wx/filedlg.h"
#include "../GUICircuit.h"

#define ID_EXPORT 5950
#define ID_COMBOBOX 5951
#define ID_TEXTCTRL 5951
#define ID_LOAD 5953
#define ID_SAVE 5954


DECLARE_APP(MainApp)

BEGIN_EVENT_TABLE(OscopeFrame, wxFrame)
	EVT_TOGGLEBUTTON(ID_PAUSE_BUTTON, OscopeFrame::OnToggleButton)
	EVT_COMBOBOX(ID_COMBOBOX, OscopeFrame::OnComboUpdate)
	EVT_BUTTON(ID_EXPORT, OscopeFrame::OnExport)
	EVT_BUTTON(ID_LOAD, OscopeFrame::OnLoad)
	EVT_BUTTON(ID_SAVE, OscopeFrame::OnSave)
	
	// Hide, but don't close, the window:	
	EVT_CLOSE(OscopeFrame::OnClose)
END_EVENT_TABLE()

OscopeFrame::OscopeFrame(wxWindow *parent, const wxString& title, GUICircuit* gCircuit)
       : wxFrame(parent, wxID_ANY, title, wxDefaultPosition, wxSize(800,250))
{
	// Match the background color of buttons for the button area:
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );

	// Copy the circuit pointer to this frame:
	this->gCircuit = gCircuit;

	// set up the panel and make canvases
	oSizer = new wxBoxSizer( wxHORIZONTAL );
	
	//Sets vertical sizer
	vSizer = new wxGridSizer( 1 );

	//Combo box 
	//JoshEdit 3/15/07
	appendNewFeed( NONE_STR );
	
	//Adds vertical sizer to canvas
	oSizer->Add(vSizer, wxSizerFlags(0).Expand().Border(wxALL, 5) );

	wxSize sz = GetClientSize();
	theCanvas = new OscopeCanvas(this, gCircuit, wxID_ANY, wxDefaultPosition, wxSize(sz.GetWidth(), sz.GetHeight()), wxWANTS_CHARS|wxSUNKEN_BORDER);
	oSizer->Add( theCanvas, wxSizerFlags(1).Expand().Border(wxALL, 0) );
	oSizer->Show(theCanvas);

	buttonSizer = new wxGridSizer( 1 );

	pauseButton = new wxToggleButton(this, ID_PAUSE_BUTTON, "Pause", wxDefaultPosition, wxDefaultSize);
	pauseButton->SetValue(false);
	buttonSizer->Add(pauseButton, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 0) );

	exportButton = new wxButton(this, ID_EXPORT, "Export", wxDefaultPosition, wxDefaultSize);
	buttonSizer->Add(exportButton, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 0) );

	loadButton = new wxButton(this, ID_LOAD, "Load", wxDefaultPosition, wxDefaultSize);
	buttonSizer->Add(loadButton, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 0) );

	saveButton = new wxButton(this, ID_SAVE, "Save", wxDefaultPosition, wxDefaultSize);
	buttonSizer->Add(saveButton, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 0) );

	oSizer->Add(buttonSizer, wxSizerFlags(0).Expand().Border(wxALL, 5) );
	SetSizer( oSizer );
 }

// event handlers

void OscopeFrame::UpdateData(void){ 
	// If the button is not pressed, then update the data:
	if( !(pauseButton->GetValue()) ) {
		theCanvas->UpdateData();
	}
}

void OscopeFrame::UpdateMenu(void){ 
	theCanvas->UpdateMenu();
}

// Hide, but don't close the frame:
void OscopeFrame::OnClose( wxCloseEvent& event ){ 
	// Veto the close event:
	event.Veto();
	
	// Hide the window:
	this->Show(false);
}


void OscopeFrame::OnToggleButton( wxCommandEvent& event ){ 
	if( !(pauseButton->GetValue()) ) {
		theCanvas->clearData();
		pauseButton->SetLabel("Pause");
	} else {
		pauseButton->SetLabel("Reset");
	}
}

void OscopeFrame::OnComboUpdate( wxCommandEvent& event ){ 
	//***************************************
	//Edit by Joshua Lansford 3/11/07
	//In an effort to find the case insensitive bug,
	//I am rewriting this code.  The edit ends with the
	//end of this function.

	vector< int > idsToRemove;
	
	//make sure we don't delete the last one
	if( numberOfFeeds() > 1 ){
		for( unsigned int i = 0; i < numberOfFeeds(); ++i ){
			if( getFeedName( i ) == RMOVE_STR ){
				idsToRemove.push_back( i );
			}		
		}
	}
	
	//now we need to pull all the ones that we are going to toss
	//out of the sizer
	for( vector< int >::reverse_iterator I = idsToRemove.rbegin(); I != idsToRemove.rend(); ++I ){
		removeFeed( *I );
	}
	
	//now we need to make sure that there is not two [None]s at the end
	int feedNum = numberOfFeeds();
	while( feedNum >= 2 &&
			getFeedName( feedNum - 2 ) == NONE_STR &&
			getFeedName( feedNum - 1 ) == NONE_STR ){
		
		removeFeed( feedNum - 1 );	
		feedNum = numberOfFeeds();
	}
	
	//now make sure there is an empty one on the end for the use of adding
	if( feedNum == 0 || getFeedName( feedNum - 1 ) != NONE_STR ){
		appendNewFeed( NONE_STR );
	}
	
	
	//update layout
	Layout();
	
	//just in case we added someone, make sure
	//that they get updated with a list of what feeds are possable
	theCanvas->UpdateMenu();
	
	verifyReferenceOrder();

	/*
	if (comboBoxVector[comboBoxVector.size()-1]->GetValue() != "[None]") {		
		//starts new array of strings
		wxArrayString strings;
		strings.Add("[None]");
	
		comboBoxVector.push_back(new wxComboBox(this, ID_COMBOBOX, "[None]", wxDefaultPosition, wxDefaultSize, strings, 
	      wxCB_READONLY | wxCB_DROPDOWN	| wxCB_SORT));
	      
		//Adds selection box to vSizer
		vSizer->Add(comboBoxVector[comboBoxVector.size()-1], wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 0) );
	
		//Call layout function
		vSizer->Layout();
	}

	vector < unsigned int > deleteComboIDs;
	// Are last two set to [None] - if so then remove last one
	if (comboBoxVector.size() > 1 && 
		comboBoxVector[comboBoxVector.size()-1]->GetValue() == "[None]" &&
		comboBoxVector[comboBoxVector.size()-2]->GetValue() == "[None]") {
		
		deleteComboIDs.push_back(comboBoxVector.size()-1);
	}

	//For loop to test if a [Remove] is selected (except on last)
	for(unsigned int x = 0; x < comboBoxVector.size()-1; x++) {
		if(comboBoxVector[x]->GetValue() == "[Remove]"){
			deleteComboIDs.push_back(x);
		}
	}
	if (deleteComboIDs.size() > 0) {
		for (unsigned int i = deleteComboIDs.size(); i > 0; i--) {
			vSizer->Hide(comboBoxVector[deleteComboIDs[i-1]]);
			vSizer->Detach(comboBoxVector[deleteComboIDs[i-1]]);
			comboBoxVector.erase(comboBoxVector.begin()+deleteComboIDs[i-1]);
		}
		vSizer->Layout();	
	}
	//Calls updatemenu
	theCanvas->UpdateMenu();
	
	// Sort pointer vector by y-coord position
	// 	Cheap bubble sort hack
	for (unsigned int i = 0; i < comboBoxVector.size(); i++) {
		for (unsigned int j = i; j < comboBoxVector.size()-1; j++) {
			if (comboBoxVector[j+1]->GetPosition().y < comboBoxVector[j]->GetPosition().y) {
				wxComboBox* tmp = comboBoxVector[j];
				comboBoxVector[j] = comboBoxVector[j+1];
				comboBoxVector[j+1] = tmp;
			}
		}
	}
	*/
 }

void OscopeFrame::OnExport( wxCommandEvent& event ){ 
	wxSize imageSize = theCanvas->GetClientSize();
	wxImage circuitImage = theCanvas->generateImage();
	wxBitmap circuitBitmap(circuitImage);
	
	wxMemoryDC memDC;
	wxBitmap labelBitmap(theCanvas->GetPosition().x+theCanvas->GetSize().GetWidth(), getFeedYPos( numberOfFeeds() - 1 ));
	memDC.SelectObject(labelBitmap);
	memDC.SetBackground(*wxWHITE_BRUSH);
	memDC.Clear();
	wxFont font(10, wxFONTFAMILY_DEFAULT, wxNORMAL, wxFONTWEIGHT_NORMAL);
	memDC.SetFont( font );
	memDC.SetTextForeground(*wxBLACK);
	memDC.SetTextBackground(*wxWHITE);
	//JoshEdit 3/15/07
	for( unsigned int i = 0; i < numberOfFeeds()-1; ++i ){
		memDC.DrawText(getFeedName(i), wxPoint(5, getFeedYPos(i)));
	}
	memDC.DrawBitmap(circuitBitmap, theCanvas->GetPosition().x, 0, false);
	
	if (wxTheClipboard->Open()) {
		wxTheClipboard->SetData(new wxBitmapDataObject(labelBitmap));
		wxTheClipboard->Close();
	}
 }

void OscopeFrame::OnLoad( wxCommandEvent& event ){ 
	wxString caption = "Open an O-scope Layout";
	wxString wildcard = "CEDAR O-scope Layout files (*.cdo)|*.cdo";
	wxString defaultFilename = "";
	wxFileDialog dialog(this, caption, wxEmptyString, defaultFilename, wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	
	if (dialog.ShowModal() == wxID_OK) {
		string path = dialog.GetPath();
		ifstream inFile(path);
		string lineFile;
		getline(inFile, lineFile, '\n');
		if (lineFile != "OSCOPE LAYOUT FILE") return;
		unsigned int numLines = 0;
		inFile >> numLines;
		getline(inFile, lineFile, '\n');

		// Remove the old boxes
		for (unsigned int i = numberOfFeeds(); i > 0; i--) {
			removeFeed( i-1 );
		}

		for (unsigned int i = 0; i < numLines; i++) {
			getline(inFile, lineFile, '\n');
			appendNewFeed( lineFile );
		}		
		//Call layout function
		Layout();
		theCanvas->UpdateMenu();
		theCanvas->clearData();
	}
 }

void OscopeFrame::OnSave( wxCommandEvent& event ){ 
	wxString caption = "Save o-scope layout";
	wxString wildcard = "CEDAR O-scope Layout files (*.cdo)|*.cdo";
	wxString defaultFilename = "";
	wxFileDialog dialog(this, caption, wxEmptyString, defaultFilename, wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dialog.ShowModal() == wxID_OK) {
		wxString path = dialog.GetPath();
		string openedFilename = path.ToStdString();
		ofstream outFile(openedFilename);
		outFile << "OSCOPE LAYOUT FILE" << endl;
		outFile << numberOfFeeds() << " : following lines are order of inputs" << endl;
		for (unsigned int i = 0; i < numberOfFeeds(); i++) outFile << getFeedName(i) << endl;
		outFile.close();
	}
 }

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
void OscopeFrame::verifyReferenceOrder(){
	bool didSwap = true;
	while( didSwap ){
		didSwap = false;
		for (unsigned int j = 0; j < comboBoxes.size()-1; j++) {
			if (comboBoxes[j+1]->GetPosition().y < comboBoxes[j]->GetPosition().y) {
				wxComboBox* tmp = comboBoxes[j];
				comboBoxes[j] = comboBoxes[j+1];
				comboBoxes[j+1] = tmp;
				didSwap = true;
			}
		}
	}
}

//creates a new feed by the specified name
//and appends it to the end.
void OscopeFrame::appendNewFeed( string newName ){
	wxArrayString strings;
	strings.Add(encodeFeedName(NONE_STR));
	if( numberOfFeeds() > 0 ){
		strings.Add(encodeFeedName( RMOVE_STR ));
	}
	
	wxComboBox* newCombo = new wxComboBox(this, ID_COMBOBOX, encodeFeedName(newName), wxDefaultPosition, wxDefaultSize, strings,
	      wxCB_READONLY  | wxCB_DROPDOWN | wxCB_SORT );
	comboBoxes.push_back( newCombo );

	//Adds vertical box to canvas
	vSizer->Add( newCombo, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 0) );
}

//this renames an existing feed
void OscopeFrame::setFeedName( int i, string newName ){
	comboBoxes[i]->SetValue(encodeFeedName(newName));
	//feedNames[i] = newName;
}

//Returns how many active feeds there are in the
//Oscope. i.e. how many combo boxes there are
unsigned int OscopeFrame::numberOfFeeds(){
	return comboBoxes.size();
}

//takes a feed out with the combo box it was in
void OscopeFrame::removeFeed( int i ){
	vSizer->Hide( i );
	vSizer->Remove( i );
	comboBoxes.erase( comboBoxes.begin() + i );
}

//Returns the name of feed i.  i.e. the current
//contents in the ith combo box
string OscopeFrame::getFeedName( int i ){
	return decodeFeedName(comboBoxes[i]->GetValue().ToStdString());
}

//Removes the feed from the list.
//This was originally done by setting
//the name to '[None]'
void OscopeFrame::cancelFeed( int i ){
	setFeedName( i, NONE_STR );
}
	
//returns the y location of this feed in
//the canves
int OscopeFrame::getFeedYPos( int i ){
	return comboBoxes[i]->GetPosition().y;
}

	
//This will cause the OscopeFrame
//to update the list of
void OscopeFrame::updatePossableFeeds( vector< string >* newPossabilities ){ 

	//refresh the combo boxes and set their values to [None] if they
	//are not valid anymore.
	for( unsigned int i = 0; i < numberOfFeeds(); ++i ){
		//clear dumps the current value also
		string currentFeedName = getFeedName( i );
		
		comboBoxes[i]->Clear();
		
		bool valueValid = false;
		
		//iterate over the new possabilities adding them to the current combo
		//box and checking if any of them match the current name which is being
		//used as the feed.
		for( vector< string >::iterator I = newPossabilities->begin(); I != newPossabilities->end(); ++I ){
			comboBoxes[i]->Append(encodeFeedName(*I));
			if( (*I) == currentFeedName ){
				valueValid = true;
			}
		}
		
		comboBoxes[i]->Append(encodeFeedName(NONE_STR));
		if( numberOfFeeds() > 1 ){
			comboBoxes[i]->Append(encodeFeedName(RMOVE_STR));
		}
		
		//if the value wasn't valid anymore, then we set it to [None]
		if( valueValid ){
			setFeedName( i, currentFeedName );
		}else{
			setFeedName( i, NONE_STR );
		}
	}
	
	
	verifyReferenceOrder();
}

//This method takes a read pointer name
//and returns a posably different name
//(a name with more spaces on the end)
//so that this name will be unique
//from any other name despice case difference
string OscopeFrame::encodeFeedName( string name ){
	if( realNameToEncName.find( name ) == realNameToEncName.end() ){
		ostringstream encNameUpper;
		ostringstream encName;
		
		encNameUpper << toUpperCase( name );
		encName << name;
		
		while( uppercasedEncNames.find( encNameUpper.str() ) != uppercasedEncNames.end() ){
			encNameUpper << " ";
			encName << " ";
		}
		
		uppercasedEncNames[ encNameUpper.str() ] = true;
		
		encNameToRealName[ encName.str() ] = name;
		realNameToEncName[ name ] = encName.str();
		
		return encName.str();
	}else{
		return realNameToEncName[ name ];
	}
}

//This undoes the encoding done in encodeFeed
string OscopeFrame::decodeFeedName( string name ){
	if( encNameToRealName.find( name ) != encNameToRealName.end() ){
		return encNameToRealName[ name ];
	}
	return name;
}

//this is used by encodeFeedName to check
//if two names are the same if you
//disregard the case
string OscopeFrame::toUpperCase( string name ){
	for( unsigned int i = 0; i < name.size(); ++i ){
		if( name[i] >= 'a' && name[i] <= 'z' ){
			name[i] = name[i] - 'a' + 'A';
		}
	}
	return name;
}

//End of edit***********************
