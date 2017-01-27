
#pragma once
#include "guiGate.h"

class RamPopupDialog;

class guiGateRAM : public guiGate {
public:
	guiGateRAM();

	//Destructor for cleaning up private vars
	virtual ~guiGateRAM();

	// Function to show the gate's parameters dialog, takes the command
	//	processor object to assign the setparameters command to.  gc is
	//	a GUICircuit pointer
	virtual void doParamsDialog(GUICircuit* gc, wxCommandProcessor* wxcmd) override;

	//Because the ram gui will be passed lots of data
	//from the ram logic, we don't want it all going
	//into the default hash of changed paramiters.
	//Thus we catch it here
	virtual void setLogicParam(const std::string &paramName, const std::string &value) override;

	//Saves the ram contents to the circuit file
	//when the circuit saves
	virtual void saveGateTypeSpecifics(XMLParser* xparse) override;

	//This method is used by the RamPopupDialog to
	//learn what values are at different addresses
	//in memory.
	unsigned long getValueAt(unsigned long address);

	//These is used by the pop-up to determine
	//what was the last value read and written
	long getLastWritten();
	long getLastRead();

private:
	//The pop-up dialog
	RamPopupDialog* ramPopupDialog;

	//This data is so we can fill the pop-up with relevent
	//information
	unsigned long dataBits;
	unsigned long addressBits;
	std::map< unsigned long, unsigned long > memory;

	//used for highlighting the last read and
	//written in the pop-up
	long lastWritten;
	long lastRead;
};