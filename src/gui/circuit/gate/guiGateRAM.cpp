
#include "guiGateRAM.h"
#include <sstream>
#include "../../parse/XMLParser.h"
#include "../../dialog/RamPopupDialog.h"

using namespace std;

//*************************************************
//Edit by Joshua Lansford 12/25/2006
//I am creating a guiGate for the RAM so that
//the ram can have its own special pop-up window
guiGateRAM::guiGateRAM() {
	guiGate();
	ramPopupDialog = NULL;
}

guiGateRAM::~guiGateRAM() {
	//Destroy is how you 'delete' wxwidget objects
	if (ramPopupDialog != NULL) {
		ramPopupDialog->Destroy();
		ramPopupDialog = NULL;
	}
}


void guiGateRAM::doParamsDialog(GUICircuit *gc, wxCommandProcessor *wxcmd) {
	if (ramPopupDialog == NULL) {
		ramPopupDialog = new RamPopupDialog(this, addressBits, (GUICircuit*)gc);
		ramPopupDialog->updateGridDisplay();
	}
	ramPopupDialog->Show(true);
}

//Because the ram gui will be passed lots of data
//from the ram logic, we don't want it all going
//into the default hash of changed paramiters.
//Thus we catch it here
void guiGateRAM::setLogicParam(const std::string &paramName, const std::string &value) {

	if (paramName.substr(0, 8) == "lastRead") {
		//this makes it so that the pop-up will green things
		//that have been just read
		istringstream addressiss(value);
		unsigned long address = 0;
		addressiss >> address;
		lastRead = address;
		if (ramPopupDialog != NULL)
			ramPopupDialog->updateGridDisplay();
	}
	else if (paramName.substr(0, 8) == "Address:") {
		istringstream addressiss(paramName.substr(8));
		unsigned long address = 0;
		addressiss >> address;
		istringstream dataiss(value);
		unsigned long data = 0;
		dataiss >> data;
		memory[address] = data;
		if (ramPopupDialog != NULL)
			ramPopupDialog->updateGridDisplay();
		lastWritten = address;
	}
	else if (paramName == "MemoryReset") {
		memory.clear();
		if (ramPopupDialog != NULL)
			ramPopupDialog->notifyAllChanged();
	}
	else if (paramName == "ADDRESS_BITS") {
		istringstream dataiss(value);
		dataiss >> addressBits;
		guiGate::setLogicParam(paramName, value);
		// Declare the address pins!		
	}
	else if (paramName == "DATA_BITS") {
		istringstream dataiss(value);
		dataiss >> dataBits;
		guiGate::setLogicParam(paramName, value);
	}
	else {
		guiGate::setLogicParam(paramName, value);
	}
}

//Saves the ram contents to the circuit file
//when the circuit saves
void guiGateRAM::saveGateTypeSpecifics(XMLParser* xparse) {
	for (map< unsigned long, unsigned long >::iterator I = memory.begin();
		I != memory.end();  ++I) {
		if (I->second != 0) {
			xparse->openTag("lparam");
			ostringstream memoryValue;
			memoryValue << "Address:" << I->first << " " << I->second;
			xparse->writeTag("lparam", memoryValue.str());
			xparse->closeTag("lparam");
		}
	}
}

//This method is used by the RamPopupDialog to
//learn what values are at different addresses
//in memory.
unsigned long guiGateRAM::getValueAt(unsigned long address) {
	//we want to refrain from creating an entry for an item
	//if it does not already exist.  Therefore we will not
	//use the [] operator
	map<unsigned long, unsigned long>::iterator finder = memory.find(address);
	if (finder != memory.end())
		return finder->second;
	return 0;
}

//These is used by the pop-up to determine
//what was the last value read and written
long guiGateRAM::getLastWritten() {
	return lastWritten;
}
long guiGateRAM::getLastRead() {
	return lastRead;
}
//End of edit
//*************************************************