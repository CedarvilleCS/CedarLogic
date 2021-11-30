#pragma once

#include "logic_gate.h"

// ******************* Junction Gate *********************
// This class uses the circuit's Junctioning capabilities
// to enable and disable a junction and splice the inputs
// into the junction, to allow true to/from nodes.
class Gate_JUNCTION : public Gate
{
public:
	// Initialize the junction gate:
	// Note: Because this gate does some really funky stuff,
	// it needs a pointer to the circuit to manipulate the Junction
	// objects.
	Gate_JUNCTION( Circuit *newCircuit );

	// Remove this junction's claim on the junction ID:
	virtual ~Gate_JUNCTION();

	// Handle gate events:
	void gateProcess( void );

	// Set the junction's ID:
	bool setParameter( string paramName, string value );

	// Get the junction's ID:
	string getParameter( string paramName );

	// Connect a wire to the input of this gate:
	void connectInput( string inputID, IDType wireID );

	// Disconnect a wire from the input of this gate:
	// (Returns the wireID of the wire that was connected.)
	IDType disconnectInput( string inputID );

private:
	string myID;
	Circuit * myCircuit;

	// All the wires hooked up to this particular junction gate:
	// (Needs the counted wire deal in case you hook up a wire twice,
	// and then unhook it once. We don't want to remove it from the set yet then.)
	ID_SET< IDType > myWires;
	ID_MAP< IDType, unsigned long > myWireCounts;

	// The shared junction IDs and usage counters:
//	static ID_MAP< string, IDType > junctionIDs;
//	static ID_MAP< string, unsigned long > junctionUseCounter;
};
