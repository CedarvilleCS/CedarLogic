
#include "cmdDisconnectWire.h"
#include <sstream>
#include "../GUICircuit.h"

cmdDisconnectWire::cmdDisconnectWire(GUICircuit* gCircuit, IDType wireId,
		IDType gateId, const std::string &hotspot, bool noCalcShape) :
			klsCommand(true, "Disconnection") {

	this->gCircuit = gCircuit;
	this->wireId = wireId;
	this->gateId = gateId;
	this->hotspot = hotspot;
	this->noCalcShape = noCalcShape;
}

bool cmdDisconnectWire::Do() {

	if ((gCircuit->getWires())->find(wireId) == (gCircuit->getWires())->end()) return false; // error: wire not found
	if ((gCircuit->getGates())->find(gateId) == (gCircuit->getGates())->end()) return false; // error: gate not found


	guiGate* gate = gCircuit->getGates()->at(gateId);
	std::string hotspotPal = gate->getHotspotPal(hotspot);

	if (hotspotPal != "") {
		cmdConnectWire::sendMessagesToDisconnect(gCircuit, wireId, gateId, hotspotPal);
	}
	cmdConnectWire::sendMessagesToDisconnect(gCircuit, wireId, gateId, hotspot);

	return true;
}

bool cmdDisconnectWire::Undo() {

	if ((gCircuit->getWires())->find(wireId) == (gCircuit->getWires())->end()) return false; // error: wire not found
	if ((gCircuit->getGates())->find(gateId) == (gCircuit->getGates())->end()) return false; // error: gate not found

	guiGate* gate = gCircuit->getGates()->at(gateId);
	std::string hotspotPal = gate->getHotspotPal(hotspot);

	if (hotspotPal != "") {
		cmdConnectWire::sendMessagesToConnect(gCircuit, wireId, gateId, hotspotPal, noCalcShape);
	}
	cmdConnectWire::sendMessagesToConnect(gCircuit, wireId, gateId, hotspot, noCalcShape);

	return true;
}

std::string cmdDisconnectWire::toString() const {

	std::ostringstream oss;
	oss << "disconnectwire " << wireId << " " << gateId << " " << hotspot;
	return oss.str();
}