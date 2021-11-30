#pragma once

#include "logic_gate.h"

// ******************* BUS_END Gate *********************
// This gate is basically a bunch of junctions.
// It's used as a transition between gui buses and logic wires.
class Gate_BUS_END : public Gate
{
public:
	// Initialize the gate:
	// Note: Because this gate does some really funky stuff,
	// it needs a pointer to the circuit to manipulate the Junction
	// objects.
	Gate_BUS_END(Circuit *newCircuit);

	// Destroy the gate, and remove the Junction objects from the
	// Circuit:
	virtual ~Gate_BUS_END();

	// Handle gate events:
	void gateProcess();

	// Connect a wire to the input of this gate:
	void connectInput(string inputID, IDType wireID);

	// Disconnect a wire from the input of this gate:
	// (Returns the wireID of the wire that was connected.)
	IDType disconnectInput(string inputID);

	// Set the parameters:
	bool setParameter(string paramName, string value);

	// Get the parameters:
	string getParameter(string paramName);

private:
	Circuit * myCircuit;

	// The bus end is just a set of junctions.
	std::vector<IDType> junctionIDs;

	int busWidth;
};