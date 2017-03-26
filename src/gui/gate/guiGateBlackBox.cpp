
#include "guiGateBlackBox.h"
#include "gui/command/cmdPasteBlock.h"


guiGateBlackBox::~guiGateBlackBox() {
	// destroy internal wires and gates.
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

	// set current canvas to null.
	// disable increment-copy.
	std::string internalsCopy = internals;
	cmdPasteBlock paste(internalsCopy);
	paste.Do();
	// reset current canvas.
	// reset increment-copy.

	// erase to-/from- junctions, replace with logic params.
	
	// hold all gate- and wire- ids.
	// destroy with self.
}