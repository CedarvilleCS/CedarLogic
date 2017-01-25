
#pragma once
#include "guiGate.h"

class ADCPopupDialog;

class guiGateADC : public guiGate {
public:
	guiGateADC();

	//Destructor for cleaning up private vars
	virtual ~guiGateADC();

	// Function to show the gate's parameters dialog, takes the command
	//	processor object to assign the setparameters command to.  gc is
	//	a GUICircuit pointer
	virtual void doParamsDialog(void* gc, wxCommandProcessor* wxcmd);

	//this is so we can update the pop-up about the current value
	virtual void setLogicParam(std::string paramName, std::string value);

private:
	//The pop-up dialog
	ADCPopupDialog* aDCPopupDialog;
};