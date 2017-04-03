
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

	cmdPasteBlock paste(internalsCopy, false, circuit, nullptr);
	
	// Send pin:pinname's to logic gate.

	// Prefix junctions.
	for (auto *command : paste.getCommands()) {
		if (command->GetName() == "Set Parameter") {
			cmdSetParams *paramSetter = static_cast<cmdSetParams *>(command);
			
			for (auto &p : paramSetter->getLogicParameterMap()) {
				if (p.first == "JUNCTION_ID") {
					p.second = "bbox#" + std::to_string(bBoxCount) + "." + p.second;
				}
			}
		}
	}
	bBoxCount++;

	paste.Do();

	// 3. Refactor cmdPasteBlock, cmdCreateGate, cmdSetParams, cmdCreateWire, cmdConnectWire, cmdMoveWire.
	//     to allow access to created gate and wire ids.
}

int guiGateBlackBox::bBoxCount = 0;