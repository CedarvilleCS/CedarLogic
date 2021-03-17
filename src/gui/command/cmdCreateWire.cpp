
#include "cmdCreateWire.h"
#include <sstream>
#include "../GUICanvas.h"
#include "../guiGate.h"

cmdCreateWire::cmdCreateWire(GUICanvas* gCanvas, GUICircuit* gCircuit,
		const std::vector<IDType> &wireIds, cmdConnectWire* conn1,
		cmdConnectWire* conn2) :
			klsCommand(true, "Create Wire") {

	this->gCanvas = gCanvas;
	this->gCircuit = gCircuit;
	this->wireIds = wireIds;
	this->conn1 = conn1;
	this->conn2 = conn2;
}

cmdCreateWire::cmdCreateWire(const std::string &def) :
		klsCommand(true, "Create Wire") {
	_MSGCOM("Command String: %s\n", def.c_str());	//@@@@
	std::istringstream iss(def);
	std::string dump;
	iss >> dump;

	// The list of id's is followed by a non-integer.
	// So we can just read until we can't get an integer.
	IDType tempId;
	while (iss >> tempId) {
		wireIds.push_back(tempId);
	}
	iss.clear();

	std::string wireid, gateid, hotspot;
	iss >> dump >> wireid >> gateid >> hotspot;
	conn1 = new cmdConnectWire(dump + " " + wireid + " " + gateid + " " + hotspot);
	iss >> dump >> wireid >> gateid >> hotspot;
	conn2 = new cmdConnectWire(dump + " " + wireid + " " + gateid + " " + hotspot);
}

cmdCreateWire::~cmdCreateWire() {
	delete conn1;
	delete conn2;
}

bool cmdCreateWire::Do() {

	guiWire *wire = gCircuit->createWire(wireIds);
	gCanvas->insertWire(wire);

	for (IDType wireId : wireIds) {
		gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_CREATE_WIRE,
			new klsMessage::Message_CREATE_WIRE(wireId)));
	}

	conn1->Do();
	conn2->Do();

	return true;
}

bool cmdCreateWire::Undo() {

	conn1->Undo();
	conn2->Undo();

	for (IDType wireId : wireIds) {
		gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_DELETE_WIRE,
			new klsMessage::Message_DELETE_WIRE(wireId)));
	}

	gCanvas->removeWire(wireIds[0]);
	gCircuit->deleteWire(wireIds[0]);

	return true;
}

bool cmdCreateWire::validateBusLines() const {

	IDType gate1Id = conn1->getGateId();
	IDType gate2Id = conn2->getGateId();

	guiGate *gate1 = (*gCircuit->getGates())[gate1Id];
	guiGate *gate2 = (*gCircuit->getGates())[gate2Id];

	std::string hotspot1 = conn1->getHotspot();
	std::string hotspot2 = conn2->getHotspot();

	int busLines1 = gate1->getHotspot(hotspot1)->getBusLines();
	int busLines2 = gate2->getHotspot(hotspot2)->getBusLines();

	return busLines1 == busLines2 && busLines1 == wireIds.size();
}

const vector<IDType> & cmdCreateWire::getWireIds() const {
	return wireIds;
}

std::string cmdCreateWire::toString() const {
	std::ostringstream oss;
	oss << "createwire ";

	// No need to put a count because thing after ids is a non-int.
	for (IDType id : wireIds) {
		oss << id << ' ';
	}

	oss << conn1->toString() << ' ' << conn2->toString();
	return oss.str();
}

void cmdCreateWire::setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas,
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

	conn1->setPointers(gCircuit, gCanvas, gateids, wireids);
	conn2->setPointers(gCircuit, gCanvas, gateids, wireids);
	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
}