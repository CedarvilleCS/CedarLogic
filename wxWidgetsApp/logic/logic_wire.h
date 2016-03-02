/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   
*****************************************************************************/

// logic_wire.h: interface for the Wire class.
// Ben Sprague, 10-15-2005
//////////////////////////////////////////////////////////////////////

#ifndef LOGIC_WIRE_H
#define LOGIC_WIRE_H

#include "logic_defaults.h"	// Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Wire;
class Junction;
class Circuit;


// An input to a wire:
// This allows the wires to never have duplicate inputs from a gate, as well as
// enabling disconnecting wires to work correctly.
class WireInput {
public:
	WireInput( IDType gateID, string gateOutputID, StateType inputState = UNKNOWN ) {
		this->gateID = gateID;
		this->gateOutputID = gateOutputID;
		this->inputState = inputState;
	}
	
	IDType gateID;
	string gateOutputID;

	StateType inputState;
};

// Operator for WireInput (Allows it to be stored in maps).
bool operator < (const WireInput &left, const WireInput &right);

// Wire outputs, which connect a gate and a gateInput together:
class WireOutput {
public:
	WireOutput( IDType gateID, string gateInputID ) {
		this->gateID = gateID;
		this->gateInputID = gateInputID;
	}

	IDType gateID;
	string gateInputID;
};

// Operator for WireOutput (Allows it to be stored in maps).
bool operator < (const WireOutput &left, const WireOutput &right);


// Operator for WIRE_PTRs, to allow it to be stored in sets and maps.
bool operator < (const WIRE_PTR &left, const WIRE_PTR &right);


class Wire  
{
friend class Junction;
friend class Circuit;
public:
	// Change the state of one of the wires' inputs. Don't update the internal state yet.
	void setInputState(IDType gateID, string gateOutputID, StateType newState);

	// Update the internal state of the wire.
	// Return the new state.
	StateType calculateState( set< WIRE_PTR > wireGroup );
	
	// Force the wire to change state (used when a wire is in a junction group):
	void forceState( StateType newState );

	// Returns a list of output gates that this wire affects.
	vector< IDType > getOutputGates( void );

	// Return the current state of the wire:
	StateType getState( void );

	// Connect a gate output to this wire:
	void connectInput( IDType gateID, string gateOutputID );

	// Connect this wire to a gate input:
	void connectOutput( IDType gateID, string gateInputID );

	// Disconnect a gate output from this wire's input.
	void disconnectInput( IDType gateID, string gateOutputID );

	// Disconnect a gate input from this wire's output:
	void disconnectOutput( IDType gateID, string gateInputID );

	// Get the first non-external input of the wire:
	// If there are no non-external inputs, then it returns a WireInput with gateID == ID_NONE;
	WireInput getFirstInput( void );

	// Get the first output of the wire:
	// If there are no outputs, then it returns a WireOutput with gateID == ID_NONE;
	WireOutput getFirstOutput( void );

	Wire();
	virtual ~Wire();

protected:
// To be used only by the Junction and Circuit classes:

	// Return the junctions connected to this wire:
	ID_SET< IDType > getJunctions( void ) {
		return junctionList;
	};

	// Connect this wire to a new junction:
	// (Don't do anything fancy - just add the ID to this wire's list.)
	void addJunction( IDType theJunction ) {
		junctionList.insert( theJunction );
	};

	// Disconnect this wire from a junction:
	// (Just remove the ID from this wire's list.)
	void removeJunction( IDType theJunction ) {
		junctionList.erase( theJunction );
	};

protected:
	StateType wireState; // The current state of this wire.

	// A list of the wire input states, associated by wire input ID:
	// (It's a "set" so that there are no duplicates).
	ID_SET< WireInput > inputList;

	// A set containing all of the output gates that this wire affects:
	// (It's a "set" so that there are no duplicates).
	ID_SET< WireOutput > outputList;
	
	// A list of junctions that this wire connects to.
	ID_SET< IDType > junctionList;
};

#endif // LOGIC_WIRE_H
