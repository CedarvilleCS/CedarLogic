/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   
*****************************************************************************/

// logic_wire.cpp: implementation of the Wire class.
//
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include "logic_wire.h"
#include "logic_defaults.h"
#include "logic_circuit.h"
#include "logic_junction.h"


// Definition of operator for WireInput (Allows it to be stored in maps).
bool operator < (const WireInput &left, const WireInput &right) {
	if( left.gateID < right.gateID ) {
		return true;
	} else if( left.gateID > right.gateID ) {
		return false;
	} else if( left.gateOutputID < right.gateOutputID ) {
		return true;
	} else return false;
}


// Definition of operator for WireOutput (Allows it to be stored in maps).
bool operator < (const WireOutput &left, const WireOutput &right) {
	if( left.gateID < right.gateID ) {
		return true;
	} else if( left.gateID > right.gateID ) {
		return false;
	} else if( left.gateInputID < right.gateInputID ) {
		return true;
	} else return false;
}


// Definition of operator for WIRE_PTR (Allows it to be stored in sets).
bool operator < (const WIRE_PTR &left, const WIRE_PTR &right) {
	return ( left.get() < right.get() );
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Wire::Wire()
{
	// Always initialize new wires to high-impedance since they are floating
	// until they are connected to a gate:
	wireState = HI_Z;
}


Wire::~Wire()
{

}


// Change the state of one of the wires' inputs. Don't update the internal state yet.
void Wire::setInputState(IDType gateID, string gateOutputID, StateType newState)
{
	WireInput theInput( gateID, gateOutputID, newState );
	if( inputList.find( theInput ) == inputList.end() ) {
		WARNING("Wire::setInputState() - Invalid input ID.");
	} else {
		// Set the new state of the WireInput:
		// (Erase the old one, and re-create the new one.)
		inputList.erase( inputList.find( theInput ) );
		inputList.insert( theInput );
	}
}


// Update the internal state of the wire.
// Return the new state.
StateType Wire::calculateState( set< WIRE_PTR > wireGroup )
{
	// The boolean map to tell which states are input into this wire:
	vector<bool> stateMap( NUM_STATES, false );

	// Loop through each wire in the list:
	set< WIRE_PTR >::iterator thisWire = wireGroup.begin();
	while( thisWire != wireGroup.end() ) {

		// Tally up all of the input states into this wire:
		if( (*thisWire)->inputList.empty() ) {
			// The wire isn't connected to any inputs, so it is in the HI-Z state by default!
			stateMap[HI_Z] = true;
		} else {
			ID_SET< WireInput >::iterator thisInput = (*thisWire)->inputList.begin();
			StateType thisState;
			while( thisInput != (*thisWire)->inputList.end() ) {
				thisState = thisInput->inputState;
				thisInput++;
	
				// Assign this state map position the value of "true" because
				// we found an input with that state type:
				stateMap[thisState] = true;
			}
		}

		thisWire++;
	}

	// Calculate the state based on the types of input states that exist:
	if( (stateMap[ZERO] && stateMap[ONE]) || stateMap[CONFLICT] ) {
		// There are conflicting inputs, so the output is "CONFLICT":
		wireState = CONFLICT;
	} else if( stateMap[ONE] ) {
		// No conflict or unknowns and ONE is true, so the output is ONE:
		wireState = ONE;
	} else if( stateMap[ZERO] ) {
		// No conflict or unknowns and ZERO is true, so the output is ZERO:
		wireState = ZERO;
	} else if( stateMap[UNKNOWN] ) {
		// There is an unknown input, but no conflict, so the output is unknown:
		wireState = UNKNOWN;
//TODO: Add an option to automatically set UNKNOWN to ONE or ZERO, to allow
// memory circuits to function correctly.
	} else if( stateMap[HI_Z] ) {
		// No conflict or unknowns or values except HI_Z, so the output is HI_Z:
		wireState = HI_Z;
	} else {
		wireState = UNKNOWN;
		WARNING("Wire::calculateState() - No valid input states!");
	}

	return wireState;
}

// Force the wire to change state (used when a wire is in a junction group):
void Wire::forceState( StateType newState ) {
	wireState = newState;
}


// Returns a list of output gates that this wire affects.
vector< IDType > Wire::getOutputGates()
{
	vector< IDType > outputGates;
	
	ID_SET< WireOutput >::iterator thisGate = outputList.begin();

	while( thisGate != outputList.end() ) {
		outputGates.push_back( thisGate->gateID );
		thisGate++;
	}

	return outputGates;
}


// Return the current state of the wire:
StateType Wire::getState( void )
{
	return wireState;
}

	
// Connect a gate output to this wire:
void Wire::connectInput( IDType gateID, string gateOutputID )
{
	// Only one wire input per gate & gateOutput is allowed.
	// (The input's state is initialized to "unknown".)
	WireInput newInput( gateID, gateOutputID, UNKNOWN );
	
	// Add it into the input list:
	inputList.insert( newInput );
}


// Disconnect a gate output from this wire's input.
void Wire::disconnectInput( IDType gateID, string gateOutputID ) {

	// Verify that the input exists:
	if( inputList.find( WireInput( gateID, gateOutputID ) ) == inputList.end() ) {
		WARNING("Wire::disconnectInput() - Input does not exist.");
		return;
	}

	// Remove that input from the wire's list:
	inputList.erase( WireInput( gateID, gateOutputID ) );
}


// Connect this wire to a gate input:
void Wire::connectOutput( IDType gateID, string gateInputID )
{
	// When adding multiple output gates, we should create a list of outputs with no duplicates.
	// A particular gate/inputID combination should only show up once in the outputGateList.
	
	// Store the gateIDs and the gateInputIDs in the gate lists:
	this->outputList.insert( WireOutput( gateID, gateInputID ) );
}


// Disconnect a gate input from this wire's output:
void Wire::disconnectOutput( IDType gateID, string gateInputID ) {
	// Verify that the output exists:
	if( outputList.find( WireOutput( gateID, gateInputID ) ) == outputList.end() ) {
		WARNING("Wire::disconnectOutput() - Output does not exist.");
		return;
	}

	// Remove that output from the wire's list:
	outputList.erase( WireOutput( gateID, gateInputID ) );
}


// Get the first non-external input of the wire:
// If there are no non-external inputs, then it returns a WireInput with gateID == ID_NONE;
WireInput Wire::getFirstInput( void ) {
	ID_SET< WireInput >::iterator inP = inputList.begin();
	
	while( inP != inputList.end() ) {
		WireInput temp = *(inputList.begin());
		if( temp.gateID != ID_NONE ) {
			return temp;
		}
		inP++;
	}

	WireInput junk( ID_NONE, "" );
	return junk;
}


// Get the first output of the wire:
// If there are no outputs, then it returns a WireOutput with gateID == ID_NONE;
WireOutput Wire::getFirstOutput( void ) {
	if( outputList.empty() ) {
		WireOutput junk( ID_NONE, "" );
		return junk;
	} else {
		WireOutput temp = *(outputList.begin());
		return temp;
	}
}


