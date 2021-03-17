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
#include <unordered_map>   // removed .h  KAS

#include "MainApp.h"
#include "commands.h"
#include "GUICanvas.h"
#include "GUICircuit.h"
#include "guiGate.h"
#include "guiWire.h"
#include "wx/clipbrd.h"
#include "wx/dataobj.h"

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
		
		TranslationMap gateids;
		TranslationMap wireids;
    	while (getline( iss, temp, '\n' )) {
			_MSG("%s", temp.c_str());
    		klsCommand* cg = NULL;
    		if (temp.substr(0,10) == "creategate") cg = new cmdCreateGate(temp);
			else if (temp.substr(0, 9) == "setparams") {

				/* EDIT by Colin Broberg, 10/6/16
				   logic to increment number on end of TO/FROM tag */

				string numEnd = "";	// String of numbers on end that we will build
				string temp2 = temp;

				// If we are copying more than one thing, don't increment them -- that would be annoying
				if (pasteText.find("creategate", pasteText.find("creategate") + 1) == std::string::npos) {

					// Loop from end of temp to beginning, gathering up numbers to build unto numEnd
					// Starts at temp.length() - 2 so that it starts at the end minus one because 
					// temp always ends with a /t
					for (int i = temp.length() - 2; i > 0; i--) {
						if (isdigit(temp[i])) {
							numEnd = temp[i] + numEnd;
						}
						else {
							break;
						}
					}

					// If we have numbers to add and we are naming a junction_id
					if (numEnd != "" && temp.find("JUNCTION_ID") != std::string::npos) {
						string *newPasteText = new string();
						*newPasteText = pasteText; // This string will be modified and rewritten to the clipboard so that subsequent pastes continue to increment

						temp.erase(temp.length() - 1 - numEnd.length(), numEnd.length() + 1); // Erase number at end of tag, add 1 to erase the \t also
						newPasteText->erase(newPasteText->length() - 2 - numEnd.length(), numEnd.length() + 2);  // Modify clipboard data similarly, but +2 so it erases the \n also

						int holder = stoi(numEnd);
						holder++; // The whole point of this -- increment number at end of tag by 1
						string s = to_string(holder) + "\t";

						temp += s; // Add it back to temp string
						*newPasteText += s + "\n";

						wxTheClipboard->AddData(new wxTextDataObject((wxChar*)newPasteText->c_str())); // Update clipboard data so subsequent pastes carry 
					/* END OF EDIT */
					}

				}
				cg = new cmdSetParams(temp);
			}
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
		TranslationMap::iterator gateWalk = gateids.begin();
		while (gateWalk != gateids.end()) {
			(*(gCircuit->getGates()))[gateWalk->second]->select();
			gateWalk++;
		}
		TranslationMap::iterator wireWalk = wireids.begin();
		while (wireWalk != wireids.end()) {
			guiWire *wire = (*(gCircuit->getWires()))[wireWalk->second];
			if (wire != nullptr) {
				wire->select();
			}
			wireWalk++;
		}
		gCircuit->getOscope()->UpdateMenu();
    }

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
			if ( (*(gCircuit->getGates()))[gates[i]]->isConnected(hsmapWalk->first) )connectWireList[(*(gCircuit->getGates()))[gates[i]]->getConnection(hsmapWalk->first)->getID()]++;
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
		// Set the IDs
		wire->setIDs( (*gCircuit->getWires())[wireWalk->first]->getIDs() );
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
		// Wire should now have a completely valid shape to copy, shove it on the vector
		copyWires.push_back(wire);
		wireWalk++;
	}
	// Now actually generate copy of wire
	for (unsigned int i = 0; i < copyWires.size(); i++) {
		vector < wireConnection > wconns = copyWires[i]->getConnections();
		// now generate the connections - connections 1 and 2 must be passed to create the wire
		//	after which all connections may be done in succession.
		cmdConnectWire *conn1 = new cmdConnectWire(gCircuit, copyWires[i]->getID(), wconns[0].cGate->getID(), wconns[0].connection);
		cmdConnectWire *conn2 = new cmdConnectWire(gCircuit, copyWires[i]->getID(), wconns[1].cGate->getID(), wconns[1].connection);
		cmdTemp = new cmdCreateWire(gCanvas, gCircuit, gCircuit->getWires()->at(copyWires[i]->getID())->getIDs(), conn1, conn2);
		oss << cmdTemp->toString() << endl;
		delete cmdTemp;
		for (unsigned int j = 2; j < wconns.size(); j++) {
			cmdTemp = new cmdConnectWire(gCircuit, copyWires[i]->getID(), wconns[j].cGate->getID(), wconns[j].connection);
			oss << cmdTemp->toString() << endl;
			delete cmdTemp;
		}
		// now track the wire's shape:
		cmdTemp = new cmdMoveWire(gCircuit, copyWires[i]->getID(), copyWires[i]->getSegmentMap(), copyWires[i]->getSegmentMap());
		oss << cmdTemp->toString() << endl;
		delete cmdTemp;
		delete copyWires[i];
	}
	if (!wxTheClipboard->Open()) return;
	wxTheClipboard->AddData(new wxTextDataObject((wxChar*)(oss.str().c_str())));
	wxTheClipboard->Close();
}
