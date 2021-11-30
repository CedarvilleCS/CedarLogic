#include "gate_bus_end.h"
#include "logic_circuit.h"

// **************************** BUS_END Gate ***********************************


// Initialize the starting state and the output:
Gate_BUS_END::Gate_BUS_END(Circuit *newCircuit) : Gate() {

	// Keep the circuit pointer, to use to access the Junctions
	myCircuit = newCircuit;
	busWidth = 0;
}

Gate_BUS_END::~Gate_BUS_END() {
	for (IDType id : junctionIDs) {
		myCircuit->deleteJunction(id);
	}
}

void Gate_BUS_END::gateProcess() { }

void Gate_BUS_END::connectInput(string inputID, IDType wireID) {
	Gate::connectInput(inputID, wireID);
	
	// Grab the bus line from the back of the inputID.
	int id = atoi(inputID.substr(inputID.find('_') + 1).c_str());

	// Connect the wire to the junction in the Circuit:
	myCircuit->connectJunction(junctionIDs[id], wireID);
}

IDType Gate_BUS_END::disconnectInput(string inputID) {
	IDType wireID = ID_NONE;

	// Call the gate's method:
	wireID = Gate::disconnectInput(inputID);

	// Grab the bus line from the back of the inputID.
	int id = atoi(inputID.substr(inputID.find('_') + 1).c_str());

	if (wireID != ID_NONE) {
		// Unhook the wire from the Junction in the Circuit:
		myCircuit->disconnectJunction(junctionIDs[id], wireID);
	}

	return wireID;
}

bool Gate_BUS_END::setParameter(string paramName, string value) {

	istringstream iss(value);
	if (paramName == "INPUT_BITS") {

		if (busWidth != 0) {
			return false;
		}

		iss >> busWidth;

		// Declare the address pins!		
		if (busWidth > 0) {

			// Declare inputs and outputs for connect/disconnect routines.
			declareInputBus("IN", busWidth);
			declareInputBus("OUT", busWidth);

			// Create internal junctions.
			for (int i = 0; i < busWidth; i++) {
				junctionIDs.push_back(myCircuit->newJunction());
				myCircuit->setJunctionState(junctionIDs[i], true);
			}
		}

		//NOTE: Don't return "true" from this, because
		// you shouldn't be setting this param during simulation while
		// anything is connected anyhow!
	}
	else {
		return Gate::setParameter(paramName, value);
	}
	return false;
}

string Gate_BUS_END::getParameter(string paramName) {

	ostringstream oss;
	if (paramName == "INPUT_BITS") {
		oss << busWidth;
		return oss.str();
	}
	else {
		return Gate::getParameter(paramName);
	}
}


// **************************** END BUS_END GATE ***********************************
