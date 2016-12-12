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


// CMDDELETEGATE

cmdDeleteGate::cmdDeleteGate(GUICircuit* gCircuit, GUICanvas* gCanvas, unsigned long gid) : klsCommand(true, "Delete Gate") {
	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
	this->gid = gid;
}

cmdDeleteGate::~cmdDeleteGate ( void ) {
	while (!(cmdList.empty())) {
//		delete cmdList.top();
		cmdList.pop();
	}	
}

bool cmdDeleteGate::Do() {
	//make sure the gate exists
	if ( (gCircuit->getGates())->find(gid) == (gCircuit->getGates())->end() ) return false; //error: gate not found
	map< string, GLPoint2f > gateConns = (*(gCircuit->getGates()))[gid]->getHotspotList();
	map< string, GLPoint2f >::iterator connWalk = gateConns.begin();
	vector < int > deleteWires;
	//we will need to disconect all wires that connect to that gate from that gate
	//we iterate over the connections
	while (connWalk != gateConns.end()) {
		//if the connection is actually connected...
		if ((*(gCircuit->getGates()))[gid]->isConnected(connWalk->first)) {
			//grab the wire on that connection
			guiWire* gWire = (*(gCircuit->getGates()))[gid]->getConnection(connWalk->first);
			//create a disconnect command and do it
			cmdDisconnectWire* disconn = new cmdDisconnectWire( gCircuit, gWire->getID(), gid, connWalk->first );
			cmdList.push(disconn);
			disconn->Do();
			
			//----------------------------------------------------------------------------------------
			//Joshua Lansford edit 11/02/06--Added so "buffer" ports on a gate don't contain
			//wire artifacts after the rest of the wire has been deleted.  A buffer is created
			//by haveing a input and output hotspot in the same location. 
			//if the number of things the wire has left to connect is only two, then delete the wire.
			
			//first thing we verify is that we only have two connections left.
			if((*(gCircuit->getWires()))[gWire->getID()]->numConnections() == 2){
				//now we get the gid from both those connections.
				//I copied the test above from the test above from below.
				//I don't know why they are getting another reference to the wire when
				//they have gWire.  I suppose gWire doesn't get updated or something.
				//So I will do my work off of a freshly fetched wire and call it gWire2
				guiWire* gWire2 = (*(gCircuit->getWires()))[gWire->getID()];
				
				vector < wireConnection > connections = gWire2->getConnections();
				if( connections[0].gid == connections[1].gid ){
					
					//now we have to make sure that the connections are the same pin by comparing their positions
					guiGate* possibleBuffGate = (*(gCircuit->getGates()))[connections[0].gid];
					string* hotspot1Name = &connections[0].connection;
					string* hotspot2Name = &connections[1].connection;
					
					float x1 = 0, y1 = 0, x2 = 0, y2 = 0;
					possibleBuffGate->getHotspotCoords( *hotspot1Name, x1, y1 );
					possibleBuffGate->getHotspotCoords( *hotspot2Name, x2, y2 );
					
					if( x1 == x2 && y1 == y2 ){
						//this wire has met the requierments for being cooked.
						//so we will scedual it for being delted.
						deleteWires.push_back(gWire->getID());
					}
				}
			}
			//coment on edit. I compiled and tested this edit.
			//It doesn't delete wires that connect two different pins
			//on one chip when a gate is deleted.  It does delete a wire
			//connecting and input and a output that are in the same location
			//when you delete another gate
			//end of edit---------------the else on the following if was added as well------------------
			else if ((*(gCircuit->getWires()))[gWire->getID()]->numConnections() < 2) deleteWires.push_back(gWire->getID());
		}
		connWalk++;
	}
	
	for (unsigned int i = 0; i < deleteWires.size(); i++) {
		cmdDeleteWire* delwire = new cmdDeleteWire( gCircuit, gCanvas, deleteWires[i] );
		cmdList.push(delwire);
		delwire->Do();
	}

	float x, y;
	(*(gCircuit->getGates()))[gid]->getGLcoords(x, y);
	guiGate* gGate = (*(gCircuit->getGates()))[gid];
	cmdList.push ( new cmdMoveGate( gCircuit, gid, x, y, x, y ) );
	cmdList.push ( new cmdSetParams( gCircuit, gid, paramSet(gGate->getAllGUIParams(), gGate->getAllLogicParams()), true ) );
	
	gateType = (*(gCircuit->getGates()))[gid]->getLibraryGateName();
	
	gCanvas->removeGate(gid);
	gCircuit->deleteGate(gid, true);
	string logicType = wxGetApp().libParser.getGateLogicType( gateType );
	if( logicType.size() > 0 ) {
		gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_DELETE_GATE, new klsMessage::Message_DELETE_GATE(gid)));
	}
	return true;
}

bool cmdDeleteGate::Undo() {
	gCircuit->createGate(gateType, gid, true);

	string logicType = wxGetApp().libParser.getGateLogicType( gateType );
	if( logicType.size() > 0 ) {
		gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_CREATE_GATE, new klsMessage::Message_CREATE_GATE(logicType, gid)));
	}
	gCanvas->insertGate(gid, (*(gCircuit->getGates()))[gid], 0, 0);

	while (!(cmdList.empty())) {
		cmdList.top()->Undo();
		cmdList.pop();
	}
	return true;
}

// CMDDELETESELECTION

cmdDeleteSelection::cmdDeleteSelection(GUICircuit* gCircuit, GUICanvas* gCanvas, vector < unsigned long > &gates, vector < unsigned long > &wires) : klsCommand(true, "Delete Selection") {
	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
	for (unsigned int i = 0; i < gates.size(); i++) this->gates.push_back(gates[i]);
	for (unsigned int i = 0; i < wires.size(); i++) this->wires.push_back(wires[i]);
}

cmdDeleteSelection::~cmdDeleteSelection( void ) {
	while (!(cmdList.empty())) {
		delete cmdList.top();
		cmdList.pop();
	}	
}

bool cmdDeleteSelection::Do() {
	for (unsigned int i = 0; i < wires.size(); i++) {
		cmdList.push( new cmdDeleteWire( gCircuit, gCanvas, wires[i] ) );
		cmdList.top()->Do();
	}
	for (unsigned int i = 0; i < gates.size(); i++) {
		cmdList.push( new cmdDeleteGate( gCircuit, gCanvas, gates[i] ) );
		cmdList.top()->Do();
	}
	if (gCircuit->getOscope() != NULL) gCircuit->getOscope()->UpdateMenu();

	return true;
}

bool cmdDeleteSelection::Undo() {
	while (!(cmdList.empty())) {
		cmdList.top()->Undo();
		delete cmdList.top();
		cmdList.pop();
	}
	if (gCircuit->getOscope() != NULL) gCircuit->getOscope()->UpdateMenu();
	return true;
}


































































// CMDDELETEWIRE

cmdDeleteWire::cmdDeleteWire(GUICircuit* gCircuit, GUICanvas* gCanvas, IDType wireId) : klsCommand(true, "Delete Wire") {
	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
	this->wireIds = { wireId };
}

cmdDeleteWire::~cmdDeleteWire() {
	while (!(cmdList.empty())) {
		delete cmdList.top();
		cmdList.pop();
	}
}

bool cmdDeleteWire::Do() {

	if ((gCircuit->getWires())->find(wireIds[0]) == (gCircuit->getWires())->end()) return false; //error: wire not found

	guiWire *wire = gCircuit->getWires()->at(wireIds[0]);

	wireIds = wire->getIDs();

	vector < wireConnection > destroyList = wire->getConnections();
	cmdMoveWire* movewire = new cmdMoveWire(gCircuit, wireIds[0], wire->getSegmentMap(), GLPoint2f(0, 0));
	cmdList.push(movewire);

	for (unsigned int j = 0; j < destroyList.size(); j++) {
		cmdDisconnectWire* disconn = new cmdDisconnectWire(gCircuit, wireIds[0], destroyList[j].cGate->getID(), destroyList[j].connection, true);
		cmdList.push(disconn);
		disconn->Do();
	}
	gCanvas->removeWire(wireIds[0]);
	gCircuit->deleteWire(wireIds[0]);

	for (IDType id : wireIds) {
		gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_DELETE_WIRE,
			new klsMessage::Message_DELETE_WIRE(id)));
	}

	return true;
}

bool cmdDeleteWire::Undo() {

	guiWire* gWire = gCircuit->createWire(wireIds);

	for (IDType id : wireIds) {
		gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_CREATE_WIRE,
			new klsMessage::Message_CREATE_WIRE(id)));
	}

	while (!(cmdList.empty())) {
		cmdList.top()->Undo();
		delete cmdList.top();
		cmdList.pop();
	}

	gCanvas->insertWire(gWire);

	return true;
}
























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




















// CMDPASTEBLOCK

cmdPasteBlock::cmdPasteBlock(vector < klsCommand* > &cmdList) : klsCommand(true, "Paste") {
	for (unsigned int i = 0; i < cmdList.size(); i++) this->cmdList.push_back(cmdList[i]);
	m_init = false;
}

bool cmdPasteBlock::Do() {
	if (!m_init) {
		m_init = true;
		return true;
	}
	for (unsigned int i = 0; i < cmdList.size(); i++) cmdList[i]->Do();
	return true;
}

bool cmdPasteBlock::Undo() {
	for (int i = cmdList.size()-1; i >= 0; i--) {
		cmdList[i]->Undo();
	}
	return true;
}


















// CMDWIRESEGDRAG

cmdWireSegDrag::cmdWireSegDrag(GUICircuit* gCircuit, GUICanvas* gCanvas, unsigned long wireID) : klsCommand(true, "Wire Shape") {
	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
	this->wireID = wireID;
	if ( (gCircuit->getWires())->find(wireID) == (gCircuit->getWires())->end() ) return; // error: wire not found
	oldSegMap = (*(gCircuit->getWires()))[wireID]->getOldSegmentMap();
	newSegMap = (*(gCircuit->getWires()))[wireID]->getSegmentMap();
}

bool cmdWireSegDrag::Do() {
	if ( (gCircuit->getWires())->find(wireID) == (gCircuit->getWires())->end() ) return false; // error: wire not found
	(*(gCircuit->getWires()))[wireID]->setSegmentMap(newSegMap);
	return true;
}

bool cmdWireSegDrag::Undo() {
	if ( (gCircuit->getWires())->find(wireID) == (gCircuit->getWires())->end() ) return false; // error: wire not found
	(*(gCircuit->getWires()))[wireID]->setSegmentMap(oldSegMap);
	return true;
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