/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   logic_junction: Implements junction objects for the logic core
*****************************************************************************/

#include "logic_junction.h"
#include "logic_circuit.h"
#include "logic_wire.h"

#include <iostream>
using namespace std;

Junction::Junction( IDType newID )
{
	// Junctions default to enabled state:
	isEnabled = true;
	myID = newID;
}

Junction::~Junction()
{
}

// Set enable state of the junction:
void Junction::setEnableState( bool newState ) {
	isEnabled = newState;
}

// Connect a wire to this junction:
void Junction::connectWire( IDType wireID ) {
	// Connect the junction to the wire:
	wireList.insert( wireID );
}

// Disconnect a wire to this junction:
// Return true if this wire needs disconnected from
// the junction because there are no more connections of it
// plugged to this junction.
bool Junction::disconnectWire( IDType wireID ) {
	if( wireList.find( wireID ) == wireList.end() ) {
		WARNING("Junction::disconnectWire() - wireID not found in junction.");
		return false;
	}

	// Take the first instance of this wire out of this junction:
	wireList.erase( wireList.find( wireID ) );

	return ( wireList.find( wireID ) == wireList.end() );
}
