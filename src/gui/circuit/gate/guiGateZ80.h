
#pragma once
#include "guiGate.h"

class Z80PopupDialog;

class guiGateZ80 : public guiGate {
public:
	guiGateZ80();

	// Function to show the gate's parameters dialog, takes the command
	//	processor object to assign the setparameters command to.  gc is
	//	a GUICircuit pointer
	virtual void doParamsDialog(void* gc, wxCommandProcessor* wxcmd);

	//Destructor for cleaning up private vars
	virtual ~guiGateZ80();

	//Because the z80 gui will be passed lots of data
	//from the z80 logic, we don't want it all going
	//into the default hash of changed paramiters.
	//Thus we catch it here
	virtual void setLogicParam(std::string paramName, std::string value);

private:
	//The pop-up dialog
	Z80PopupDialog* z80PopupDialog;
};