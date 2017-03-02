
#include "cmdDeleteWire.h"
#include "../GUICircuit.h"
#include "../wire/guiWire.h"
#include "../gate/guiGate.h"
#include "../widget/GUICanvas.h"
#include "../message/Message.h"
#include "cmdMoveWire.h"
#include "cmdDisconnectWire.h"

cmdDeleteWire::cmdDeleteWire(GUICircuit* gCircuit, GUICanvas* gCanvas,
		IDType wireId) :
			klsCommand(true, "Delete Wire") {

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

	std::vector < wireConnection > destroyList = wire->getConnections();
	cmdMoveWire* movewire = new cmdMoveWire(gCircuit, wireIds[0], wire->getSegmentMap(), Point(0, 0));
	cmdList.push(movewire);

	for (unsigned int j = 0; j < destroyList.size(); j++) {
		cmdDisconnectWire* disconn = new cmdDisconnectWire(gCircuit, wireIds[0], destroyList[j].cGate->getID(), destroyList[j].connection, true);
		cmdList.push(disconn);
		disconn->Do();
	}
	gCanvas->removeWire(wireIds[0]);
	gCircuit->deleteWire(wireIds[0]);

	for (IDType id : wireIds) {
		gCircuit->sendMessageToCore(new Message_DELETE_WIRE(id));
	}

	return true;
}

bool cmdDeleteWire::Undo() {

	guiWire* gWire = gCircuit->createWire(wireIds);

	for (IDType id : wireIds) {
		gCircuit->sendMessageToCore(new Message_CREATE_WIRE(id));
	}

	while (!(cmdList.empty())) {
		cmdList.top()->Undo();
		delete cmdList.top();
		cmdList.pop();
	}

	gCanvas->insertWire(gWire);

	return true;
}