/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   klsClipboard: handles copy and paste of blocks
*****************************************************************************/

#include "klsClipboard.h"
#include "OscopeFrame.h"
#include <fstream>
#include <map>
#include <hash_map>   // removed .h  KAS

DECLARE_APP(MainApp)

cmdPasteBlock* klsClipboard::pasteBlock( GUICircuit* gCircuit, GUICanvas* gCanvas ) {
	if (!wxTheClipboard->Open()) return NULL;
	if ( !wxTheClipboard->IsSupported(wxDF_TEXT) ) {
		wxTheClipboard->Close();
		return NULL;
	}
    wxTextDataObject text;
    vector < klsCommand* > cmdList;
    if ( wxTheClipboard->GetData(text) ) {
    	string pasteText = (char*)(text.GetText().c_str());
    	if (pasteText.find('\n',0) == string::npos) return NULL;
    	istringstream iss(pasteText);
    	string temp;
		hash_map < unsigned long, unsigned long > gateids;
		hash_map < unsigned long, unsigned long > wireids;
    	while (getline( iss, temp, '\n' )) {
    		klsCommand* cg = NULL;
    		if (temp.substr(0,10) == "creategate") cg = new cmdCreateGate(temp);
    		else if (temp.substr(0,9) == "setparams") cg = new cmdSetParams(temp);
    		else if (temp.substr(0,10) == "createwire") cg = new cmdCreateWire(temp);
    		else if (temp.substr(0,11) == "connectwire") cg = new cmdConnectWire(temp);
    		else if (temp.substr(0,8) == "movewire") cg = new cmdMoveWire(temp);
    		else break;
    		cmdList.push_back( cg );
    		cg->setPointers( gCircuit, gCanvas, gateids, wireids );
    		cg->Do();
	    	if (iss.str().find('\n',0) == string::npos) return NULL;
    	}
		gCanvas->unselectAllGates();
		gCanvas->unselectAllWires();
		hash_map < unsigned long, unsigned long >::iterator gateWalk = gateids.begin();
		while (gateWalk != gateids.end()) {
			(*(gCircuit->getGates()))[gateWalk->second]->select();
			gateWalk++;
		}
		hash_map < unsigned long, unsigned long >::iterator wireWalk = wireids.begin();
		while (wireWalk != wireids.end()) {
			(*(gCircuit->getWires()))[wireWalk->second]->select();
			wireWalk++;
		}
		gCircuit->getOscope()->UpdateMenu();
    }
	wxTheClipboard->Close();
	if (cmdList.size() > 0) return new cmdPasteBlock ( cmdList );
	return NULL;
}

void klsClipboard::copyBlock( GUICircuit* gCircuit, GUICanvas* gCanvas, vector < unsigned long > gates, vector < unsigned long > wires ) {
	if (gates.size() == 0) return;
	ostringstream oss;
	klsCommand* cmdTemp;
	map < unsigned long, unsigned long > connectWireList;
	// Write strings to copy gates
	for (unsigned int i = 0; i < gates.size(); i++) {
		// generate list of wire connections
		map < string, GLPoint2f > hotspotmap = (*(gCircuit->getGates()))[gates[i]]->getHotspotList();
		map < string, GLPoint2f >::iterator hsmapWalk = hotspotmap.begin();
		while (hsmapWalk != hotspotmap.end()) {
			if ( (*(gCircuit->getGates()))[gates[i]]->isConnected(hsmapWalk->first) ) connectWireList[(*(gCircuit->getGates()))[gates[i]]->getConnection(hsmapWalk->first)->getID()]++;
			hsmapWalk++;
		}
		// Creation of a gate takes care of type, position, id; all other items are in params
		float x, y;
		(*(gCircuit->getGates()))[gates[i]]->getGLcoords(x,y);
		cmdTemp = new cmdCreateGate( gCanvas, gCircuit, gates[i], (*(gCircuit->getGates()))[gates[i]]->getLibraryGateName(), x, y);
		oss << cmdTemp->toString() << endl;
		delete cmdTemp;
		guiGate* gGate = (*(gCircuit->getGates()))[gates[i]];
		cmdTemp = new cmdSetParams( gCircuit, gates[i], paramSet(gGate->getAllGUIParams(), gGate->getAllLogicParams()) );
		oss << cmdTemp->toString() << endl;
		delete cmdTemp;
	}
	// For wires, only copy if more than one active connection, and trim shape
	vector < guiWire* > copyWires;
	map < unsigned long, unsigned long >::iterator wireWalk = connectWireList.begin();
	while (wireWalk != connectWireList.end()) {
		if ( wireWalk->second < 2 ) { wireWalk++; continue; }
		guiWire* wire = new guiWire();
		// Set the ID
		wire->setID( wireWalk->first );
		// Shove all the connections
		vector < wireConnection > wireConns = (*(gCircuit->getWires()))[wireWalk->first]->getConnections();
		for (unsigned int i = 0; i < wireConns.size(); i++) wire->addConnection( wireConns[i].cGate, wireConns[i].connection, true );
		// Now get the segment map copy
		wire->setSegmentMap( (*(gCircuit->getWires()))[wireWalk->first]->getSegmentMap() );
		// Now that we have a good copy of the wire object, we can trim the connections that we don't want to carry over
		for (unsigned int i = 0; i < wireConns.size(); i++) {
			bool found = false;
			for (unsigned int j = 0; j < gates.size() && !found; j++) if (gates[j] == wireConns[i].gid) found = true;
			if (found) continue; // we found this connection; don't trim it
			// get rid of it
			wire->removeConnection( wireConns[i].cGate, wireConns[i].connection );
		}
		wire->printme();
		// Wire should now have a completely valid shape to copy, shove it on the vector
		copyWires.push_back(wire);
		wireWalk++;
	}
	// Now actually generate copy of wire
	for (unsigned int i = 0; i < copyWires.size(); i++) {
		vector < wireConnection > wconns = copyWires[i]->getConnections();
		// now generate the connections - connections 1 and 2 must be passed to create the wire
		//	after which all connections may be done in succession.
		cmdConnectWire conn1( gCircuit, copyWires[i]->getID(), wconns[0].cGate->getID(), wconns[0].connection );
		cmdConnectWire conn2( gCircuit, copyWires[i]->getID(), wconns[1].cGate->getID(), wconns[1].connection );
		cmdTemp = new cmdCreateWire( gCanvas, gCircuit, copyWires[i]->getID(), &conn1, &conn2 );
		oss << cmdTemp->toString() << endl;
		delete cmdTemp;
		for (unsigned int j = 2; j < wconns.size(); j++) {
			cmdTemp = new cmdConnectWire( gCircuit, copyWires[i]->getID(), wconns[j].cGate->getID(), wconns[j].connection );
			oss << cmdTemp->toString() << endl;
			delete cmdTemp;
		}
		// now track the wire's shape:
		cmdTemp = new cmdMoveWire( gCircuit, copyWires[i]->getID(), copyWires[i]->getSegmentMap(), copyWires[i]->getSegmentMap());
		oss << cmdTemp->toString() << endl;
		delete cmdTemp;
		//delete copyWires[i];
	}
	if (!wxTheClipboard->Open()) return;
	wxTheClipboard->AddData(new wxTextDataObject((wxChar*)(oss.str().c_str())));
	wxTheClipboard->Close();
}
