
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

	// 1. Refactor all commands to not necessarily need a gui canvas.
	
	std::string internalsCopy = internals;

	// 1.5. Refactor cmdPasteBlock to not run immediately.

	// 2. Refactor cmdSetParams to allow prefixing of the 'JUNCTION_ID' attribute.

	cmdPasteBlock paste(internalsCopy, false);
	paste.Do();

	// 3. Refactor cmdPasteBlock, cmdCreateGate, cmdSetParams, cmdCreateWire, cmdConnectWire, cmdMoveWire.
	//     to allow access to created gate and wire ids.
}