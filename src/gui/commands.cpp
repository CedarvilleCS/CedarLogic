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













// CMDSETPARAMS

cmdSetParams::cmdSetParams(GUICircuit* gCircuit, unsigned long gid, paramSet pSet, bool setMode) : klsCommand(true, "Set Parameter") {
	if ((gCircuit->getGates())->find(gid) == (gCircuit->getGates())->end()) return; // error: gate not found
	this->gCircuit = gCircuit;
	this->gid = gid;
	this->fromString = setMode;
	// Save the original set of parameters
	map < string, string >::iterator paramWalk = (*(gCircuit->getGates()))[gid]->getAllGUIParams()->begin();
	while (paramWalk != (*(gCircuit->getGates()))[gid]->getAllGUIParams()->end()) {
		oldGUIParamList[paramWalk->first] = paramWalk->second;
		paramWalk++;
	}
	paramWalk = (*(gCircuit->getGates()))[gid]->getAllLogicParams()->begin();
	while (paramWalk != (*(gCircuit->getGates()))[gid]->getAllLogicParams()->end()) {
		oldLogicParamList[paramWalk->first] = paramWalk->second;
		paramWalk++;
	}
	// Now grab the new ones...
	if (pSet.gParams != NULL) {
		paramWalk = pSet.gParams->begin();
		while (paramWalk != pSet.gParams->end()) {
			newGUIParamList[paramWalk->first] = paramWalk->second;
			paramWalk++;
		}
	}
	if (pSet.lParams != NULL) {
		paramWalk = pSet.lParams->begin();
		while (paramWalk != pSet.lParams->end()) {
			newLogicParamList[paramWalk->first] = paramWalk->second;
			paramWalk++;
		}
	}
}

cmdSetParams::cmdSetParams(string def) : klsCommand(true, "Set Parameter") {
	this->fromString = true;
	istringstream iss(def);
	string dump; char comma;
	unsigned long numgParams, numlParams;
	iss >> dump >> gid >> numgParams >> comma >> numlParams;
	for (unsigned int i = 0; i < numgParams; i++) {
		string paramName, paramVal;
		iss >> paramName;
		getline(iss, paramVal, '\t');
		newGUIParamList[paramName] = paramVal.substr(1, paramVal.size() - 1);
		oldGUIParamList[paramName] = newGUIParamList[paramName];
	}
	for (unsigned int i = 0; i < numlParams; i++) {
		string paramName, paramVal;
		iss >> paramName;
		getline(iss, paramVal, '\t');
		newLogicParamList[paramName] = paramVal.substr(1, paramVal.size() - 1);
		oldLogicParamList[paramName] = newLogicParamList[paramName];
	}
}

bool cmdSetParams::Do() {
	if ((gCircuit->getGates())->find(gid) == (gCircuit->getGates())->end()) return false; // error: gate not found
	map < string, string >::iterator paramWalk = newLogicParamList.begin();
	vector < string > dontSendMessages;
	LibraryGate lg = wxGetApp().libraries[(*(gCircuit->getGates()))[gid]->getLibraryName()][(*(gCircuit->getGates()))[gid]->getLibraryGateName()];
	for (unsigned int i = 0; i < lg.dlgParams.size(); i++) {
		if (lg.dlgParams[i].isGui) continue;
		if (lg.dlgParams[i].type == "FILE_IN" || lg.dlgParams[i].type == "FILE_OUT") dontSendMessages.push_back(lg.dlgParams[i].name);
	}
	while (paramWalk != newLogicParamList.end()) {
		(*(gCircuit->getGates()))[gid]->setLogicParam(paramWalk->first, paramWalk->second);
		bool found = false;
		for (unsigned int i = 0; i < dontSendMessages.size() && !found; i++) {
			if (dontSendMessages[i] == paramWalk->first) found = true;
		}
		if (!found) gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(gid, paramWalk->first, paramWalk->second)));
		paramWalk++;
	}
	paramWalk = newGUIParamList.begin();
	while (paramWalk != newGUIParamList.end()) {
		(*(gCircuit->getGates()))[gid]->setGUIParam(paramWalk->first, paramWalk->second);
		paramWalk++;
	}
	if (!fromString && (*(gCircuit->getGates()))[gid]->getGUIType() == "TO" && gCircuit->getOscope() != NULL) gCircuit->getOscope()->UpdateMenu();
	return true;
}

bool cmdSetParams::Undo() {
	if ((gCircuit->getGates())->find(gid) == (gCircuit->getGates())->end()) return false; // error: gate not found
	map < string, string >::iterator paramWalk = oldLogicParamList.begin();
	vector < string > dontSendMessages;
	LibraryGate lg = wxGetApp().libraries[(*(gCircuit->getGates()))[gid]->getLibraryName()][(*(gCircuit->getGates()))[gid]->getLibraryGateName()];
	for (unsigned int i = 0; i < lg.dlgParams.size(); i++) {
		if (lg.dlgParams[i].isGui) continue;
		if (lg.dlgParams[i].type == "FILE_IN" || lg.dlgParams[i].type == "FILE_OUT") dontSendMessages.push_back(lg.dlgParams[i].name);
	}
	while (paramWalk != oldLogicParamList.end()) {
		(*(gCircuit->getGates()))[gid]->setLogicParam(paramWalk->first, paramWalk->second);
		bool found = false;
		for (unsigned int i = 0; i < dontSendMessages.size() && !found; i++) {
			if (dontSendMessages[i] == paramWalk->first) found = true;
		}
		if (!found) gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(gid, paramWalk->first, paramWalk->second)));
		paramWalk++;
	}
	paramWalk = oldGUIParamList.begin();
	while (paramWalk != oldGUIParamList.end()) {
		(*(gCircuit->getGates()))[gid]->setGUIParam(paramWalk->first, paramWalk->second);
		paramWalk++;
	}
	if (!fromString && (*(gCircuit->getGates()))[gid]->getGUIType() == "TO") gCircuit->getOscope()->UpdateMenu();
	return true;
}

string cmdSetParams::toString() const {
	ostringstream oss;
	oss << "setparams " << gid << " " << newGUIParamList.size() << "," << newLogicParamList.size() << " ";
	map < string, string >::const_iterator paramWalk = newGUIParamList.cbegin();
	while (paramWalk != newGUIParamList.cend()) {
		oss << paramWalk->first << " " << paramWalk->second << "\t";
		paramWalk++;
	}
	paramWalk = newLogicParamList.begin();
	while (paramWalk != newLogicParamList.end()) {
		oss << paramWalk->first << " " << paramWalk->second << "\t";
		paramWalk++;
	}
	return oss.str();
}

void cmdSetParams::setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas, hash_map < unsigned long, unsigned long > &gateids, hash_map < unsigned long, unsigned long > &wireids) {
	gid = gateids[gid];
	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
}


















































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