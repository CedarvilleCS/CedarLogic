
#include "guiGateBlackBox.h"
#include "gui/commands.h"
#include "gui/GUICircuit.h"
#include "gui/parse/quoted.h"

guiGateBlackBox::guiGateBlackBox(GUICircuit *circuit, bool purelyVisual) :
	circuit(circuit), purelyVisual(purelyVisual) { }

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
		if (!purelyVisual) {
			createInternals(value);
		}
	}

	guiGate::setGUIParam(paramName, value);
}

void guiGateBlackBox::createInternals(const std::string &internals) {
	
	std::istringstream unquoter(internals);
	std::string internalsCopy;
	unquoter >> parse::quoted(internalsCopy);

	cmdPasteBlock paste(internalsCopy, false, circuit, nullptr);

	// Prefix junctions and connect them to the Gate_BLACK_BOX.
	for (auto *command : paste.getCommands()) {

		if (command->GetName() == "Set Parameter") {

			cmdSetParams *paramSetter = static_cast<cmdSetParams *>(command);
			
			for (auto &p : paramSetter->getLogicParameterMap()) {

				if (p.first == "JUNCTION_ID") {
					std::string newJunctionName = "bbox#" + std::to_string(bBoxCount) + "." + p.second;
					setLogicParam("pin:" + p.second, newJunctionName);
					p.second = newJunctionName;
				}
			}
		}
	}
	bBoxCount++;

	// Actually create internals.
	paste.Do();

	// Record id's of internal elements.
	for (const auto &p : paste.getGateIds()) {
		gateIds.push_back(p.second);
	}
	for (const auto &p : paste.getWireIds()) {
		wireIds.push_back(p.second);
	}
}

int guiGateBlackBox::bBoxCount = 0;