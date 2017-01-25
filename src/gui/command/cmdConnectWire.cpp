
#include "cmdConnectWire.h"
#include <sstream>
#include "../circuit/GUICircuit.h"
#include "../circuit/guiWire.h"
#include "../circuit/guiGates.h"
#include "../thread/Message.h"

cmdConnectWire::cmdConnectWire(GUICircuit* gCircuit, IDType wid, IDType gid,
		const std::string &hotspot, bool noCalcShape) :
			klsCommand(true, "Connection") {

	this->gCircuit = gCircuit;
	this->wireId = wid;
	this->gateId = gid;
	this->hotspot = hotspot;
	this->noCalcShape = noCalcShape;
}

cmdConnectWire::cmdConnectWire(const std::string &def) :
		klsCommand(true, "Connection") {

	std::istringstream iss(def);
	std::string dump;
	iss >> dump >> wireId >> gateId >> hotspot;
	noCalcShape = true;
}

bool cmdConnectWire::Do() {

	if ((gCircuit->getWires())->find(wireId) == (gCircuit->getWires())->end()) return false; // error: wire not found
	if ((gCircuit->getGates())->find(gateId) == (gCircuit->getGates())->end()) return false; // error: gate not found

	guiGate* gate = gCircuit->getGates()->at(gateId);
	std::string hotspotPal = gate->getHotspotPal(hotspot);

	if (hotspotPal != "") {
		sendMessagesToConnect(gCircuit, wireId, gateId, hotspotPal, noCalcShape);
	}
	sendMessagesToConnect(gCircuit, wireId, gateId, hotspot, noCalcShape);

	return true;
}

bool cmdConnectWire::Undo() {

	if ((gCircuit->getWires())->find(wireId) == (gCircuit->getWires())->end()) return false; // error: wire not found
	if ((gCircuit->getGates())->find(gateId) == (gCircuit->getGates())->end()) return false; // error: gate not found

	guiGate* gate = gCircuit->getGates()->at(gateId);
	std::string hotspotPal = gate->getHotspotPal(hotspot);

	if (hotspotPal != "") {
		sendMessagesToDisconnect(gCircuit, wireId, gateId, hotspotPal);
	}
	sendMessagesToDisconnect(gCircuit, wireId, gateId, hotspot);

	return true;
}

bool cmdConnectWire::validateBusLines() const {

	guiWire *wire = (*gCircuit->getWires())[wireId];
	guiGate *gate = (*gCircuit->getGates())[gateId];

	int busLines = gate->getHotspot(hotspot)->getBusLines();

	// No connections on this wire? Cool. We'll make it a bus when we connect.
	if (wire->getConnections().empty()) {
		return true;
	}

	return wire->getIDs().size() == busLines;
}

std::string cmdConnectWire::toString() const {

	std::ostringstream oss;
	oss << "connectwire " << wireId << ' ' << gateId << ' ' << hotspot;
	return oss.str();
}

void cmdConnectWire::setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas,
		TranslationMap &gateids, TranslationMap &wireids) {

	gateId = gateids[gateId];
	wireId = wireids[wireId];
	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
}

IDType cmdConnectWire::getGateId() const {
	return gateId;
}

const std::string & cmdConnectWire::getHotspot() const {
	return hotspot;
}

void cmdConnectWire::sendMessagesToConnect(GUICircuit *gCircuit, IDType wireId,
		IDType gateId, const std::string &hotspot, bool noCalcShape) {

	guiGate *gate = gCircuit->getGates()->at(gateId);
	guiWire *wire = gCircuit->getWires()->at(wireId);

	// Grab the bus-lines from the wire.
	std::vector<IDType> wireIds = wire->getIDs();

	// Add the connection in the gui.
	gCircuit->setWireConnection(wireIds, gateId, hotspot, noCalcShape);

	// When connecting a gui hotspot to its underlying inputs/outputs,
	// single wires require no changes to their hotspot name, but
	// bus-lines need to have their bit appended.
	std::vector<std::string> internalHotspots;
	if (wireIds.size() == 1) {
		internalHotspots.push_back(hotspot);
	}
	else {
		for (int i = 0; i < wireIds.size(); i++) {
			internalHotspots.push_back(hotspot + "_" + std::to_string(i));
		}
	}

	bool isInput = gate->isConnectionInput(hotspot);

	// Connect each of wire's bus-lines to its corresponding gate hotspot.
	for (int i = 0; i < internalHotspots.size(); i++) {
		if (isInput) {
			gCircuit->sendMessageToCore(
				new Message_SET_GATE_INPUT(gateId, internalHotspots[i], wireIds[i]));
		}
		else {
			gCircuit->sendMessageToCore(
				new Message_SET_GATE_OUTPUT(gateId, internalHotspots[i], wireIds[i]));
		}
	}
}

void cmdConnectWire::sendMessagesToDisconnect(GUICircuit *gCircuit,
		IDType wireId, IDType gateId, const std::string &hotspot) {

	guiGate *gate = gCircuit->getGates()->at(gateId);
	guiWire *wire = gCircuit->getWires()->at(wireId);

	// Grab the bus-lines from the wire.
	std::vector<IDType> wireIds = wire->getIDs();

	// Remove the connection in the gui.
	int temp;
	gate->removeConnection(hotspot, temp);
	wire->removeConnection(gate, hotspot);

	// When disconnecting a gui hotspot from its underlying inputs/outputs,
	// single wires require no changes to their hotspot name, but
	// bus-lines need to have their bit appended.
	std::vector<std::string> internalHotspots;
	if (wireIds.size() == 1) {
		internalHotspots.push_back(hotspot);
	}
	else {
		for (int i = 0; i < wireIds.size(); i++) {
			internalHotspots.push_back(hotspot + "_" + to_string(i));
		}
	}

	bool isInput = gate->isConnectionInput(hotspot);

	// Disconnect each of wire's bus-lines from its corresponding gate hotspot.
	for (int i = 0; i < internalHotspots.size(); i++) {
		if (isInput) {
			gCircuit->sendMessageToCore(
				new Message_SET_GATE_INPUT(gateId, internalHotspots[i], 0, true));
		}
		else {
			gCircuit->sendMessageToCore(
				new Message_SET_GATE_OUTPUT(gateId, internalHotspots[i], 0, true));
		}
	}
}
