
#include "cmdMoveSelection.h"
#include "../GUICircuit.h"
#include "../guiWire.h"

cmdMoveSelection::cmdMoveSelection(GUICircuit* gCircuit,
		vector<GateState> &preMove, vector<WireState> &preMoveWire,
		float startX, float startY, float endX, float endY) :
			klsCommand(true, "Move Selection") {

	for (unsigned int i = 0; i < preMove.size(); i++) gateList.push_back(preMove[i].id);

	for (unsigned int i = 0; i < preMoveWire.size(); i++) {
		wireList.push_back(preMoveWire[i].id);
		if ((gCircuit->getWires())->find(preMoveWire[i].id) == (gCircuit->getWires())->end()) continue; // error, wire not found
		oldSegMaps[preMoveWire[i].id] = preMoveWire[i].oldWireTree;
		newSegMaps[preMoveWire[i].id] = (*(gCircuit->getWires()))[preMoveWire[i].id]->getSegmentMap();
	}

	this->gCircuit = gCircuit;
	this->startX = startX;
	this->startY = startY;
	this->endX = endX;
	this->endY = endY;
	wireMove = 1;
}

bool cmdMoveSelection::Do() {

	for (unsigned int i = 0; i < gateList.size(); i++) {
		if ((gCircuit->getGates())->find(gateList[i]) == (gCircuit->getGates())->end()) continue; // error, gate not found
		(*(gCircuit->getGates()))[gateList[i]]->translateGLcoords(endX - startX, endY - startY);
		(*(gCircuit->getGates()))[gateList[i]]->finalizeWirePlacements();
	}
	for (unsigned int i = 0; i < wireList.size(); i++) {
		if ((gCircuit->getWires())->find(wireList[i]) == (gCircuit->getWires())->end()) continue; // error, wire not found
		(*(gCircuit->getWires()))[wireList[i]]->setSegmentMap(newSegMaps[wireList[i]]);
	}
	for (unsigned int i = 0; i < proxconnects.size(); i++) {
		proxconnects[i]->Do();
	}
	return true;
}

bool cmdMoveSelection::Undo() {
	for (unsigned int i = 0; i < gateList.size(); i++) {
		if ((gCircuit->getGates())->find(gateList[i]) == (gCircuit->getGates())->end()) continue; // error, gate not found
		(*(gCircuit->getGates()))[gateList[i]]->translateGLcoords(startX - endX, startY - endY);
		(*(gCircuit->getGates()))[gateList[i]]->finalizeWirePlacements();
	}
	for (unsigned int i = 0; i < wireList.size() && wireMove < 0; i++) {
		if ((gCircuit->getWires())->find(wireList[i]) == (gCircuit->getWires())->end()) continue; // error, wire not found
		(*(gCircuit->getWires()))[wireList[i]]->setSegmentMap(oldSegMaps[wireList[i]]);
	}
	wireMove = -1;
	for (unsigned int i = 0; i < proxconnects.size(); i++) {
		proxconnects[i]->Undo();
	}
	return true;
}

vector<klsCommand *> * cmdMoveSelection::getConnections() {
	return &proxconnects;
}