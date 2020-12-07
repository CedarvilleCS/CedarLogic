
#include "klsCommand.h"

klsCommand::klsCommand(bool canUndo, const char *name) :
		wxCommand(canUndo, wxString(name)) {

	gCircuit = nullptr;
	gCanvas = nullptr;
	_MSG("KlsCommand: %s", name);	//##
}

std::string klsCommand::toString() const {
	return "";
}

void klsCommand::setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas,
		TranslationMap &gateids, TranslationMap &wireids) {

	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
};