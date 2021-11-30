/*****************************************************************************
   logic_junction: Implements junction objects for the logic core
*****************************************************************************/

#pragma once

#include "logic_defaults.h"

class Wire;

class Junction  
{
public:
	Junction( IDType newID );
	virtual ~Junction();

	// Set/get enable state of the junction:
	void setEnableState( bool newState );
	bool getEnableState( void ) { return isEnabled; };

	// Connect a wire to this junction:
	void connectWire( IDType wireID );

	// Disconnect a wire to this junction:
	// Return true if this wire needs disconnected from
	// the junction because there are no more connections of it
	// plugged to this junction.
	bool disconnectWire( IDType wireID );

	// Get the list of wires that are attached to this junction:
	// Return a blank list if there are none.
	ID_SET< IDType > getWires( void ) {
		ID_SET< IDType > retList;
		retList.insert( wireList.begin(), wireList.end() );
		return retList;
	};

protected:
	// The ID of this junction in the circuit:
	IDType myID;

	// The enable/disabled state of this junction:
	bool isEnabled;

	// The set of wires attached to this junction:
	// (Note that duplicates can exist in a multiset. This
	// will allow the connectWire() and disconnectWire() methods
	// to work in such a way that if you connect a wire twice,
	// you have to remove it twice to completely get rid of it.)
	multiset< IDType > wireList;
};
