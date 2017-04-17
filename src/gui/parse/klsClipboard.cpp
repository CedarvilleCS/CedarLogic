
#include "klsClipboard.h"
#include "../frame/OscopeFrame.h"
#include <fstream>
#include <map>
#include <unordered_map>

#include "../MainApp.h"
#include "../commands.h"
#include "../widget/GUICanvas.h"
#include "../GUICircuit.h"
#include "../gate/guiGate.h"
#include "../wire/guiWire.h"
#include "wx/clipbrd.h"
#include "wx/dataobj.h"

DECLARE_APP(MainApp)

klsCommand * klsClipboard::pasteBlock(GUICircuit *gCircuit, GUICanvas *gCanvas) {
	if (!wxTheClipboard->Open()) return NULL;
	if (!wxTheClipboard->IsSupported(wxDF_TEXT)) {
		wxTheClipboard->Close();
		return NULL;
	}

	wxTextDataObject wxText;
	if (wxTheClipboard->GetData(wxText)) {

		std::string copyText = wxText.GetText().ToStdString();
		klsCommand *result = new cmdPasteBlock(copyText, true, gCircuit, gCanvas);
		result->Do();

		// auto-incrementing can cause the copy text to change.
		wxTheClipboard->AddData(new wxTextDataObject(copyText));

		return result;
	}

	return nullptr;
}

void klsClipboard::copyBlock(GUICircuit* gCircuit, GUICanvas* gCanvas,
	const vector<IDType> &gates, const vector<IDType> &wires) {

	std::string copyText = getCopyText(gCircuit, gCanvas, gates, wires);

	if (wxTheClipboard->Open()) {
		wxTheClipboard->AddData(new wxTextDataObject(copyText));
		wxTheClipboard->Close();
	}
}

std::string klsClipboard::getCopyText(GUICircuit* gCircuit, GUICanvas* gCanvas,
	const vector<IDType> &gates, const vector<IDType> &wires) {

	if (gates.empty()) {
		return "";
	}

	ostringstream oss;
	klsCommand* cmdTemp;
	map<IDType, IDType> connectWireList;

	// Write strings to copy gates
	for (unsigned int i = 0; i < gates.size(); i++) {

		// generate list of wire connections
		map < string, Point > hotspotmap = (*(gCircuit->getGates()))[gates[i]]->getHotspotList();
		auto hsmapWalk = hotspotmap.begin();

		while (hsmapWalk != hotspotmap.end()) {
			if ((*(gCircuit->getGates()))[gates[i]]->isConnected(hsmapWalk->first))connectWireList[(*(gCircuit->getGates()))[gates[i]]->getConnection(hsmapWalk->first)->getID()]++;
			hsmapWalk++;
		}

		// Creation of a gate takes care of type, position, id; all other items are in params
		float x, y;
		(*(gCircuit->getGates()))[gates[i]]->getGLcoords(x, y);
		cmdTemp = new cmdCreateGate(gCanvas, gCircuit, gates[i], (*(gCircuit->getGates()))[gates[i]]->getLibraryGateName(), x, y);
		oss << cmdTemp->toString() << endl;
		delete cmdTemp;
		guiGate* gGate = (*(gCircuit->getGates()))[gates[i]];
		cmdTemp = new cmdSetParams(gCircuit, gates[i], paramSet(gGate->getAllGUIParams(), gGate->getAllLogicParams()));
		oss << cmdTemp->toString() << endl;
		delete cmdTemp;
	}

	// For wires, only copy if more than one active connection, and trim shape
	vector < guiWire* > copyWires;
	auto wireWalk = connectWireList.begin();
	while (wireWalk != connectWireList.end()) {

		if (wireWalk->second < 2) { wireWalk++; continue; }

		guiWire* wire = new guiWire();
		// Set the IDs
		wire->setIDs((*gCircuit->getWires())[wireWalk->first]->getIDs());
		// Shove all the connections
		vector < wireConnection > wireConns = (*(gCircuit->getWires()))[wireWalk->first]->getConnections();

		for (unsigned int i = 0; i < wireConns.size(); i++) wire->addConnection(wireConns[i].cGate, wireConns[i].connection, true);

		// Now get the segment map copy
		wire->setSegmentMap((*(gCircuit->getWires()))[wireWalk->first]->getSegmentMap());

		// Now that we have a good copy of the wire object, we can trim the connections that we don't want to carry over
		for (unsigned int i = 0; i < wireConns.size(); i++) {

			bool found = false;
			for (unsigned int j = 0; j < gates.size() && !found; j++) if (gates[j] == wireConns[i].gid) found = true;
			if (found) continue; // we found this connection; don't trim it
								 // get rid of it
			wire->removeConnection(wireConns[i].cGate, wireConns[i].connection);
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

	return oss.str();
}
