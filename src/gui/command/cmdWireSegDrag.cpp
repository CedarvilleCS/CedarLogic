
#include "cmdWireSegDrag.h"
#include "../GUICircuit.h"
#include "../wire/guiWire.h"

cmdWireSegDrag::cmdWireSegDrag(GUICircuit* gCircuit, GUICanvas* gCanvas,
		IDType wireID) :
			klsCommand(true, "Wire Shape") {

	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
	this->wireID = wireID;

	if ((gCircuit->getWires())->find(wireID) == (gCircuit->getWires())->end()) return; // error: wire not found

	oldSegMap = (*(gCircuit->getWires()))[wireID]->getOldSegmentMap();
	newSegMap = (*(gCircuit->getWires()))[wireID]->getSegmentMap();
}

bool cmdWireSegDrag::Do() {

	if ((gCircuit->getWires())->find(wireID) == (gCircuit->getWires())->end()) return false; // error: wire not found

	(*(gCircuit->getWires()))[wireID]->setSegmentMap(newSegMap);

	return true;
}

bool cmdWireSegDrag::Undo() {

	if ((gCircuit->getWires())->find(wireID) == (gCircuit->getWires())->end()) return false; // error: wire not found

	(*(gCircuit->getWires()))[wireID]->setSegmentMap(oldSegMap);

	return true;
}