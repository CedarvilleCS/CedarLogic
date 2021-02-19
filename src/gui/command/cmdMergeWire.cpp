#include "cmdMergeWire.h"
#include <sstream>
#include "../GUICanvas.h"
#include "../guiWire.h"

// Pedro Casanova (casanova@ujaen.es) 2020/04-12

cmdMergeWire::cmdMergeWire(GUICanvas* gCanvas, GUICircuit* gCircuit,
	const std::vector<IDType> &wireIds, IDType wid1, IDType wid2) : 
		klsCommand(true, "Merge Wire") {

	this->gCanvas = gCanvas;
	this->gCircuit = gCircuit;
	this->wireIds = wireIds;
	this->wid1 = wid1;
	this->wid2 = wid2;
}

cmdMergeWire::~cmdMergeWire() {

}

bool cmdMergeWire::Do() {

	guiWire *wire2 = gCircuit->getWires()->at(wid2);
	vector<wireConnection> conn2(wire2->getConnections());	
	cmdDeleteWire* deleteWire = new cmdDeleteWire(gCircuit, gCanvas, wid2);

	cmdList.push_back(deleteWire);
	deleteWire->Do();

	cmdConnectWire *connectWire;

	for (int i = 0; i < (int)conn2.size(); i++) {
		connectWire = new cmdConnectWire(gCircuit, wid1, conn2[i].gid, conn2[i].connection);
		cmdList.push_back(connectWire);
		connectWire->Do();
	}

	return true;
}

bool cmdMergeWire::Undo() {

	for (int i = cmdList.size() - 1; i >= 0; i--) {
		cmdList[i]->Undo();
		cmdList.pop_back();
	}
	return true;
}

bool cmdMergeWire::validateBusLines() const {

	int busLines1 = gCircuit->getWires()->at(wid1)->getIDs().size();
	int busLines2 = gCircuit->getWires()->at(wid2)->getIDs().size();

	return busLines1 == busLines2 && busLines1 == wireIds.size();
}

const vector<IDType> & cmdMergeWire::getWireIds() const {
	return wireIds;
}

std::string cmdMergeWire::toString() const {
	std::ostringstream oss;
	oss << "mergewire ";

	// No need to put a count because thing after ids is a non-int.
/*	for (IDType id : wireIds) {
		oss << id << ' ';
	}

	oss << conn1->toString() << ' ' << conn2->toString();*/
	return oss.str();
}

void cmdMergeWire::setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas,
	TranslationMap &gateids, TranslationMap &wireids) {

	// remap ids.
	// notice the difference between wireIds and wireids.
	for (IDType &id : wireIds) {
		if (wireids.find(id) != wireids.end()) {
			id = wireids[id];
		}
		else {
			wireids[id] = gCircuit->getNextAvailableWireID();
			id = wireids[id];
		}
	}

	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
}

