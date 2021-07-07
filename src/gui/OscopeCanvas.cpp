/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   OscopeCanvas: renders the waveform for the oscope
*****************************************************************************/

#include "OscopeCanvas.h"
#include "MainApp.h"
#include "GUICanvas.h"
#include "guiText.h"
#include "OscopeFrame.h"
#include "guiWire.h"

// Included to use the min() and max() templates:
#include <algorithm>
using namespace std;

DECLARE_APP(MainApp)

BEGIN_EVENT_TABLE(OscopeCanvas, wxGLCanvas)
    EVT_PAINT(OscopeCanvas::OnPaint)
    EVT_SIZE(OscopeCanvas::OnSize)
    EVT_ERASE_BACKGROUND(OscopeCanvas::OnEraseBackground)
END_EVENT_TABLE()

OscopeCanvas::OscopeCanvas(wxWindow *parent, GUICircuit* gCircuit, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
	: wxGLCanvas( parent, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE|wxSUNKEN_BORDER ) {

	this->gCircuit = gCircuit;
	m_init = false;
	parentFrame = (OscopeFrame*) parent;
}

OscopeCanvas::~OscopeCanvas(){ 
//	scrollTimer->Stop();
	return;
}

void OscopeCanvas::OnRender(){ 
	
	//Gets number of wires to render
	//unsigned int numberOfWires = stateValues.size();
	unsigned int numberOfWires = parentFrame->numberOfFeeds(); //<-Josh Edit using access method
	unsigned int wireNum = 0;
	
	//clear window 
	glClear(GL_COLOR_BUFFER_BIT); 

	// Set the projection matrix:	
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	wxSize sz = GetClientSize();
	// gluOrtho2D(left, right, bottom, top); (In world-space coords.)
	
	
	gluOrtho2D(0, OSCOPE_HORIZONTAL, numberOfWires * 1.5, -0.25);
	glViewport(0, 0, (GLint) sz.GetWidth(), (GLint) sz.GetHeight());

	// Set the model matrix:
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	
	glColor4f( 0.0, 0.0, 0.0, 1.0 );
	float intensity = (GLfloat) GRID_INTENSITY;
	glColor4f( 0.0, 0.0, intensity, intensity );

	
	//Drawing Vertical Lines
	glBegin(GL_LINES);
		glVertex2f( OSCOPE_HORIZONTAL/10, -0.5 );
		glVertex2f( OSCOPE_HORIZONTAL/10, numberOfWires * 1.5 );
		
		glVertex2f( (OSCOPE_HORIZONTAL/10)*2, -0.5 );
		glVertex2f( (OSCOPE_HORIZONTAL/10)*2, numberOfWires * 1.5 );
		
		glVertex2f( (OSCOPE_HORIZONTAL/10)*3, -0.5 );
		glVertex2f( (OSCOPE_HORIZONTAL/10)*3, numberOfWires * 1.5 );
		
		glVertex2f( (OSCOPE_HORIZONTAL/10)*4, -0.5 );
		glVertex2f( (OSCOPE_HORIZONTAL/10)*4, numberOfWires * 1.5 );
		
		glVertex2f( (OSCOPE_HORIZONTAL/10)*5, -0.5 );
		glVertex2f( (OSCOPE_HORIZONTAL/10)*5, numberOfWires * 1.5 );
		
		glVertex2f( (OSCOPE_HORIZONTAL/10)*6, -0.5 );
		glVertex2f( (OSCOPE_HORIZONTAL/10)*6, numberOfWires * 1.5 );
		
		glVertex2f( (OSCOPE_HORIZONTAL/10)*7, -0.5 );
		glVertex2f( (OSCOPE_HORIZONTAL/10)*7, numberOfWires * 1.5 );
		
		glVertex2f( (OSCOPE_HORIZONTAL/10)*8, -0.5 );
		glVertex2f( (OSCOPE_HORIZONTAL/10)*8, numberOfWires * 1.5 );
		
		glVertex2f( (OSCOPE_HORIZONTAL/10)*9, -0.5 );
		glVertex2f( (OSCOPE_HORIZONTAL/10)*9, numberOfWires * 1.5 );
		
		glVertex2f( 0, -0.5 );
		glVertex2f( 0, numberOfWires * 1.5 );
	glEnd();

	for (unsigned int i = 0; i < numberOfWires; i++) {
		if (parentFrame->getFeedName(i) == NONE_STR) { wireNum++; continue; }//<-Josh Edit using access method

		map< string, deque< StateType > >::iterator thisWire = stateValues.find(parentFrame->getFeedName(i).c_str()); //<-Josh Edit using access method
		if (thisWire == stateValues.end()) { wireNum++; continue; }
		deque< StateType >::reverse_iterator wireVal = (thisWire->second).rbegin();
		GLdouble horizLoc = OSCOPE_HORIZONTAL;
		GLdouble y = 0.0, lastY = 0.0;
		StateType theState;
		
		float intensity = (GLfloat) GRID_INTENSITY;
		glColor4f( 0.0, 0.0, intensity, intensity );

		//Draws Horizontal for wire
		glBegin(GL_LINES);
			glVertex2f( 0, (wireNum * 1.5) + 1);
			glVertex2f( OSCOPE_HORIZONTAL, (wireNum * 1.5) + 1);
		glEnd();
	
		bool firstTime = true;
		bool solid = false;
	
		while(wireVal != ((thisWire->second).rend())) {
			theState = *wireVal;
	
			solid = false;
			switch( theState ) {
			case ZERO:
				glColor4f( 0.0, 0.0, 0.0, 1.0 );
				y = 1.0 + wireNum * 1.5;
				break;
			case ONE:
				glColor4f( 1.0, 0.0, 0.0, 1.0 );
				y = 0.0 + wireNum * 1.5;
				break;
			case HI_Z:
				glColor4f( 0.0, 0.78f, 0.0, 1.0 );
				y = 0.5 + wireNum * 1.5;
				break;
			case UNKNOWN:
				glColor4f( 0.3f, 0.3f, 1.0, 1.0 );
				y = 0.75 + wireNum * 1.5;
				solid = true;
				break;
			case CONFLICT:
				glColor4f( 0.0, 1.0, 1.0, 1.0 );
				y = 0.75 + wireNum * 1.5;
				solid = true;
				break;
			}
			
			if( solid ) {
				glRectd( horizLoc, y, horizLoc - 1.0, 0 + wireNum * 1.5) ;
			} else {
				glBegin(GL_LINES);
				if(!firstTime && (lastY != y) ) {
					// Rise:
					glVertex2f( horizLoc, lastY );
					glVertex2f( horizLoc, y );
				}
				firstTime = false;
	
				// Run:
				glVertex2f( horizLoc, y );
				glVertex2f( horizLoc - 1.0, y );
				glEnd();
			}
			
			// Move on to the next data point:
			horizLoc -= 1.0;
			lastY = y;
			wireVal++;
		}
		wireNum++;
	} // for
}

void OscopeCanvas::OnPaint(wxPaintEvent& event){ 
	wxPaintDC dc(this);
#ifndef __WXMOTIF__
	if (!GetContext()) return;
#endif

	SetCurrent();
	// Init OpenGL once, but after SetCurrent
	if (!m_init)
	{
		//InitGL();
		m_init = true;
		glClearColor (1.0, 1.0, 1.0, 0.0);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		
		//*********************************
		//Edit by Joshua Lansford 4/09/07
		//anti-alis ing is nice
		glEnable( GL_LINE_SMOOTH );
		//End of edit

	}

	OnRender();

	// Show the new buffer:
	glFlush();
	SwapBuffers();
}

void OscopeCanvas::OnSize(wxSizeEvent& event)
{
    // this is also necessary to update the context on some platforms
    wxGLCanvas::OnSize(event);

    // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
#ifndef __WXMOTIF__
    if (GetContext())
#endif
    {
        Refresh();
        //Render();
    }
}


void OscopeCanvas::UpdateData(void){ 	
	
	//Declaration of variables
	deque<StateType> temp;

	// Log the values of all of the gates:
	unordered_map< unsigned long, guiGate* >* gateList = gCircuit->getGates();
	unordered_map< unsigned long, guiGate* >::iterator theGate;
	
	set< string > liveTOs;
	vector < guiGate* > toGates;
	if (parentFrame->numberOfFeeds() > 1) {
		// Set up a list of TO gates so I only search the whole gate list once.
		theGate = gateList->begin();
		while (theGate != gateList->end()) {
			// Pedro Casanova (casanova@ujaen.es 2020/04-12
			// TO, FROM and LINK are valid signal to Oscope
			if (((theGate->second)->getGUIType() == "TO") || ((theGate->second)->getGUIType() == "FROM") || ((theGate->second)->getGUIType() == "LINK"))
				toGates.push_back(theGate->second);
			theGate++;
		}
	}
	
	//Check to see if wire has already been added to OSCOPE
	map< string, bool > hasBeenAdded; 
	
	for (unsigned int i = 0; i < parentFrame->numberOfFeeds()-1; i++) {
		string junctionName = parentFrame->getFeedName(i).c_str();
		if (junctionName == NONE_STR || junctionName == RMOVE_STR || junctionName == "") continue;	
			
		if(hasBeenAdded.find(junctionName) == hasBeenAdded.end()) {
			hasBeenAdded[junctionName] = true;
			// Keep track of all junction names that are still valid.
			// If a gate disappears or changes junction names, then
			// we want to remove it from our data structure.
			liveTOs.insert(junctionName);

			// Create a new storage space for its data if we need it:
			if( stateValues.find(junctionName) == stateValues.end() ) {
				stateValues[junctionName] = temp;
			}
			
			// Search through our prebuilt TO gate list for this gate.
			//	From UpdateMenu, the gate should exist.
			guiGate* currentGate = NULL;
			for (unsigned int j = 0; j < toGates.size(); j++) {
				if (toGates[j]->getLogicParam("JUNCTION_ID") == junctionName) {
					currentGate = toGates[j];
					break;
				}
			}
			if (currentGate == NULL) { // Just in case of error
				stateValues.erase(junctionName);
				parentFrame->cancelFeed(i);
				//parentFrame->comboBoxVector[i]->SetValue("[None]");
				continue;
			}
			
			map<string, GLPoint2f> hsList = currentGate->getHotspotList();
			if( hsList.size() != 0 ) {
				string firstInput = (hsList.begin())->first;

				// Get the wire connected to the TO's input:
				if( currentGate->isConnected(firstInput) ) {
					guiWire* myWire = currentGate->getConnection( firstInput );
					
					// Push the current state onto this TO's data queue:
					stateValues[junctionName].push_back(myWire->getState()[0]);
				} else {
					// The TO is not connected, so the state is UNKNOWN:
					stateValues[junctionName].push_back(UNKNOWN); 
				}
			}
			
			// If the data queue is too big, then pop data off the other
			// end of the queue to make it the right size:
			if(stateValues[junctionName].size() > OSCOPE_HORIZONTAL) stateValues[junctionName].pop_front();
		}
	} // for ( not end of list )
	
	
	// Clear out data queues for TOs that don't exist anymore:
	map< string, deque< StateType > >::iterator checkData = stateValues.begin();
	while( checkData != stateValues.end() ) {
		if( liveTOs.find( checkData->first ) == liveTOs.end() ) {
			stateValues.erase( checkData );
			checkData = stateValues.begin();
		}
		else {
			checkData++;
		}
	}
	
	Refresh();
	//Render();
}



void OscopeCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
  // Do nothing, to avoid flashing.
}

void OscopeCanvas::UpdateMenu()
{
	//*******************************
	//Edit by Joshua Lansford 3/11/07
	//This edit is to retrofit this
	//method so that all it does is genrate
	//a list of possable feeds and then
	//passes it back up to its parent.
	//This way it does not mess directly
	//with the values in the combo boxes
	//which are some reason not remembering
	//what value they should be currently holding
	//The edit ends with the end of this function
	
	unordered_map< unsigned long, guiGate* >* gateList = gCircuit->getGates();
	
	vector< string > namesOfPossableFeeds;
	
	map< string, bool > alreadyAdded;
	
	//iterate over all gates
	for( unordered_map< unsigned long, guiGate* >::iterator 
		gateIterator = gateList->begin(); 
	    gateIterator != gateList->end(); 
	    gateIterator++ ){
		guiGate* aGate = gateIterator->second;
		// Pedro Casanova (casanova@ujaen.es 2020/04-12
		// TO, FROM and LINK are valid signal to Oscope
		//select out the gates which are TOs
		if ((aGate->getGUIType() == "TO") || (aGate->getGUIType() == "FROM") || (aGate->getGUIType() == "LINK"))
		{
	   		string feedName;
	   		feedName = aGate->getLogicParam("JUNCTION_ID");
	   		if (feedName!="") {
	   			//check if it has already been added
				if (alreadyAdded.find(feedName) == alreadyAdded.end()) {
					//add name to list
					namesOfPossableFeeds.push_back(feedName);
					alreadyAdded[feedName] = true;
				}
	   		}
		}
	}
	
	parentFrame->updatePossableFeeds( &namesOfPossableFeeds );

}

// Print the canvas contents to a bitmap:
wxImage OscopeCanvas::generateImage(){ 
//WARNING!!! Heavily platform-dependent code ahead! This only works in MS Windows because of the
// DIB Section OpenGL rendering.

	wxSize sz = GetClientSize();

	// Create a DIB section.
	// (The Windows wxBitmap implementation will create a DIB section for a bitmap if you set
	// a color depth of 24 or greater.)
	wxBitmap theBM( sz.GetWidth(), sz.GetHeight(), 32 );
	
	// Get a memory hardware device context for writing to the bitmap DIB Section:
	wxMemoryDC myDC;
	myDC.SelectObject(theBM);
	WXHDC theHDC = myDC.GetHDC();

	// The basics of setting up OpenGL to render to the bitmap are found at:
	// http://www.nullterminator.net/opengl32.html
	// http://www.codeguru.com/cpp/g-m/opengl/article.php/c5587/

    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    // set the pixel format for the DC
    ::ZeroMemory( &pfd, sizeof( pfd ) );
    pfd.nSize = sizeof( pfd );
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_SUPPORT_GDI;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ::ChoosePixelFormat( (HDC) theHDC, &pfd );
    ::SetPixelFormat( (HDC) theHDC, iFormat, &pfd );

    // create and enable the render context (RC)
    HGLRC hRC = ::wglCreateContext( (HDC) theHDC );
    HGLRC oldhRC = ::wglGetCurrentContext();
    HDC oldDC = ::wglGetCurrentDC();
    ::wglMakeCurrent( (HDC) theHDC, hRC );

	// Setup the viewport for rendering:
//	setViewport();
	// Reset the glViewport to the size of the bitmap:
//	glViewport(0, 0, (GLint) sz.GetWidth(), (GLint) sz.GetHeight());
	
	// Set the bitmap clear color:
	glClearColor (1.0, 1.0, 1.0, 0.0);
	glColor3b(0, 0, 0);
		
	//TODO: Check if alpha is hardware supported, and
	// don't enable it if not!
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	
	//*********************************
	//Edit by Joshua Lansford 4/09/07
	//anti-alis ing is nice
	glEnable( GL_LINE_SMOOTH );
	//End of edit

	// Do the rendering here.
	OnRender();

	// Flush the OpenGL buffer to make sure the rendering has happened:	
	glFlush();
	
	// Destroy the OpenGL rendering context, release the memDC, and
	// convert the DIB Section into a wxImage to return to the caller:
    ::wglMakeCurrent( oldDC, oldhRC );
    //::wglMakeCurrent( NULL, NULL );
    ::wglDeleteContext( hRC );
	myDC.SelectObject(wxNullBitmap);
	return theBM.ConvertToImage();
}
