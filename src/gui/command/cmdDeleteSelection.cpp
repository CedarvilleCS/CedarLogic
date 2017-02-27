
#include "cmdDeleteSelection.h"
#include "../GUICircuit.h"
#include "../frame/OscopeFrame.h"
#include "cmdDeleteWire.h"
#include "cmdDeleteGate.h"

cmdDeleteSelection::cmdDeleteSelection(GUICircuit* gCircuit,
		GUICanvas* gCanvas, std::vector<unsigned long> &gates,
		std::vector<unsigned long> &wires) :
			klsCommand(true, "Delete Selection") {

	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
	for (unsigned int i = 0; i < gates.size(); i++) this->gates.push_back(gates[i]);
	for (unsigned int i = 0; i < wires.size(); i++) this->wires.push_back(wires[i]);
}

cmdDeleteSelection::~cmdDeleteSelection() {

	while (!(cmdList.empty())) {
		delete cmdList.top();
		cmdList.pop();
	}
}

bool cmdDeleteSelection::Do() {

	for (unsigned int i = 0; i < wires.size(); i++) {
		cmdList.push(new cmdDeleteWire(gCircuit, gCanvas, wires[i]));
		cmdList.top()->Do();
	}
	for (unsigned int i = 0; i < gates.size(); i++) {
		cmdList.push(new cmdDeleteGate(gCircuit, gCanvas, gates[i]));
		cmdList.top()->Do();
	}
	if (gCircuit->getOscope() != NULL) gCircuit->getOscope()->UpdateMenu();

	return true;
}

bool cmdDeleteSelection::Undo() {

	while (!(cmdList.empty())) {
		cmdList.top()->Undo();
		delete cmdList.top();
		cmdList.pop();
	}
	if (gCircuit->getOscope() != NULL) gCircuit->getOscope()->UpdateMenu();
	return true;
}