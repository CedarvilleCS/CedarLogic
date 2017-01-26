
#include "cmdMoveGate.h"
#include "../circuit/GUICircuit.h"
#include "../circuit/gate/guiGate.h"

cmdMoveGate::cmdMoveGate(GUICircuit* gCircuit, unsigned long gid,
		float startX, float startY, float endX, float endY, bool uW) :
			klsCommand(true, "Move Gate") {

	this->gCircuit = gCircuit;
	this->gid = gid;
	this->startX = startX;
	this->startY = startY;
	this->endX = endX;
	this->endY = endY;
	this->noUpdateWires = uW;
}

bool cmdMoveGate::Do() {

	if ((gCircuit->getGates())->find(gid) == (gCircuit->getGates())->end()) return false; // error, gate not found

	(*(gCircuit->getGates()))[gid]->setGLcoords(endX, endY, noUpdateWires);
	return true;
}

bool cmdMoveGate::Undo() {

	if ((gCircuit->getGates())->find(gid) == (gCircuit->getGates())->end()) return false; // error, gate not found

	(*(gCircuit->getGates()))[gid]->setGLcoords(startX, startY, noUpdateWires);
	return true;
}

std::string cmdMoveGate::toString() const {

	if ((gCircuit->getGates())->find(gid) == (gCircuit->getGates())->end()) return ""; // error, gate not found

	ostringstream oss;
	oss << "movegate " << gid << " " << startX << " " << startY << " " << endX << " " << endY;
	return oss.str();
}