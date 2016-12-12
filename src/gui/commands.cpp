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












// JV

cmdDeleteTab::cmdDeleteTab(GUICircuit* gCircuit, GUICanvas* gCanvas, wxAuiNotebook* book, vector< GUICanvas* >* canvases, unsigned long ID) : klsCommand(true, (const wxChar *)"Delete Tab") {
	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
	this->canvasBook = book;
	this->canvases = canvases;
	this->canvasID = ID;


	hash_map< unsigned long, guiGate* >* gateList = gCanvas->getGateList();
	hash_map< unsigned long, guiGate* >::iterator thisGate = gateList->begin();
	while (thisGate != gateList->end()) {
		this->gates.push_back(thisGate->first);
		thisGate++;
	}
	hash_map< unsigned long, guiWire* >* wireList = gCanvas->getWireList();
	hash_map< unsigned long, guiWire* >::iterator thisWire = wireList->begin();
	while (thisWire != wireList->end()) {
		this->wires.push_back(thisWire->first);
		thisWire++;
	}
}

cmdDeleteTab::~cmdDeleteTab(void) {
	while (!(cmdList.empty())) {
		cmdList.pop();
	}
}

bool cmdDeleteTab::Do() {
	cmdList.push(new cmdDeleteSelection(gCircuit, gCanvas, gates, wires));
	cmdList.top()->Do();

	unsigned int canSize = canvases->size();
	//canvases->erase(canvases->begin() + canvasID);
	remove(canvases->begin(), canvases->end(), gCanvas);
	canvases->pop_back();
	if (canvasID < (canSize - 1)) {
		for (unsigned int i = canvasID; i < canSize; i++) {
			string text = "Page " + to_string(i);
			canvasBook->SetPageText(i, text);
		}
	}
	canvasBook->RemovePage(canvasID);
	//TODO fix canvases not refreshing
	gCanvas->Hide();
	return true;
}
bool cmdDeleteTab::Undo() {
	unsigned int canSize = canvases->size();
	canvases->insert(canvases->begin() + canvasID, gCanvas);
	ostringstream oss;
	oss << "Page " << canvasID+1;
	canvasBook->InsertPage(canvasID, gCanvas, (const wxChar *)oss.str().c_str(), (false));
	if (canvasID < (canSize)) {
		for (unsigned int i = canvasID+1; i < canSize+1; i++) {
			string text = "Page " + to_string(i+1);
			canvasBook->SetPageText(i, text);
		}
	}
	while (!(cmdList.empty())) {
		cmdList.top()->Undo();
		cmdList.pop();
	}
	return true;
}












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