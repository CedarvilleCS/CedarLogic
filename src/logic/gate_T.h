#pragma once

#include "logic_gate.h"

// ******************* T Gate *********************
// This class uses the circuit's Junctioning capabilities
// to enable and disable a junction and splice the inputs
// into the junction, to allow T-gates.
// Note: All of the connections are INPUTS! (That way, they default to HI_Z.)
// Input 0 = T-Gate input
// Input 1 = T-Gate input2/output
// Input 2 = Control input

class Gate_T : public Gate
{
public:
	// Initialize the gate:
	// Note: Because this gate does some really funky stuff,
	// it needs a pointer to the circuit to manipulate the Junction
	// objects.
	Gate_T( Circuit *newCircuit );
	
	// Destroy the gate, and remove the Junction object from the
	// Circuit:
	virtual ~Gate_T();

	// Handle gate events:
	void gateProcess( void );

	// Connect a wire to the input of this gate:
	void connectInput( string inputID, IDType wireID );

	// Disconnect a wire from the input of this gate:
	// (Returns the wireID of the wire that was connected.)
	IDType disconnectInput( string inputID );

private:
	Circuit * myCircuit;

	// The junctionID of the junction that this t-gate controls:
	IDType junctionID;

	// The last state of the junction:
	bool juncLastState;
};