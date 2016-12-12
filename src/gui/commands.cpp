/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   commands: Implements a klsCommand object for each user interface command
*****************************************************************************/

#include "commands.h"
#include "OscopeFrame.h"
#include <sstream>

#include "MainApp.h"
#include "GUICircuit.h"
#include "GUICanvas.h"
#include "guiGate.h"
#include "guiWire.h"
#include <string>
#include <stack>
#include <sstream>

//JV
#include <wx/aui/auibook.h>

DECLARE_APP(MainApp)
























//JV
cmdAddTab::cmdAddTab(GUICircuit* gCircuit, wxAuiNotebook* book, vector< GUICanvas* >* canvases) : klsCommand(true, (const wxChar *)"Add Tab") {
	this->gCircuit = gCircuit;
	this->canvasBook = book;
	this->canvases = canvases;
}

cmdAddTab::~cmdAddTab() {

}

bool cmdAddTab::Do() {
	canvases->push_back(new GUICanvas(canvasBook, gCircuit, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS));
	ostringstream oss;
	oss << "Page " << canvases->size();
	canvasBook->AddPage(canvases->at(canvases->size() - 1), (const wxChar *)oss.str().c_str(), (false));
	return true;
}

bool cmdAddTab::Undo() {
	canvases->erase(canvases->end()-1);
	canvasBook->DeletePage(canvasBook->GetPageCount()-1);
	return true;
}