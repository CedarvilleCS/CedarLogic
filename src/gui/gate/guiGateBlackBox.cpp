
#include "guiGateBlackBox.h"
#include "gui/commands.h"

guiGateBlackBox::guiGateBlackBox(GUICircuit *circuit) : circuit(circuit) { }

guiGateBlackBox::~guiGateBlackBox() {
	
	for (IDType id : gateIds) {
		cmdDeleteGate deleter(circuit, nullptr, id);
		deleter.Do();
	}
	for (IDType id : wireIds) {
		cmdDeleteWire deleter(circuit, nullptr, id);
		deleter.Do();
	}
}

void guiGateBlackBox::setGUIParam(const std::string &paramName,
	const std::string &value) {

	if (paramName == "internals") {
		createInternals(value);
	}

	guiGate::setGUIParam(paramName, value);
}

void guiGateBlackBox::createInternals(const std::string &internals) {

	// TODO:

	// Refactor cmdPasteBlock, cmdCreateGate, cmdSetParams, cmdCreateWire, cmdConnectWire, cmdMoveWire.
	//     to allow access to necessary parameters, to take as parameters the current canvas and whether increment is enabled.
	// 
	// set current canvas to null.
	// disable increment-copy.
	std::string internalsCopy = internals;
	cmdPasteBlock paste(internalsCopy);
	paste.Do();
	// reset current canvas.
	// reset increment-copy.

	// prefix junction ids with an otherwise illegal character and a counter to ensure uniqueness.
	
	// hold all gate- and wire- ids.
}