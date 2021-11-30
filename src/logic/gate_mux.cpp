#pragma once

#include "gate_mux.h"

// Initialize the gate's interface:
Gate_MUX::Gate_MUX() : Gate_N_INPUT() {

	// The control inputs data inputs are declared in setParameter().
	// (Must be set before using this method!)
	setParameter("INPUT_BITS", "0");

	// One output:
	declareOutput("OUT");
}




// Handle gate events:
void Gate_MUX::gateProcess( void ) {
	vector< StateType > selBus = getInputBusState("SEL");
	unsigned long sel = bus_to_ulong( selBus ); //NOTE: The MUX assumes 0 on non-specified input lines (Not UNKNOWN)!
	vector< StateType > inputs = getInputBusState("IN");

	StateType outState = UNKNOWN; // Assume UNKNOWN, in case we select an invalid number.
	if( sel < inputs.size() ) {
		outState = inputs[sel];
	}

	// Muxes can't output HI_Z or CONFLICT!
	if( (outState == HI_Z) || (outState == CONFLICT) ) {
		outState = UNKNOWN;
	}

	setOutputState("OUT", outState);
}


// Set the parameters:
bool Gate_MUX::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "INPUT_BITS" ) {
		iss >> inBits;

		// Declare the selection pins!		
		if( inBits > 0 ) {
			// The number of selection bits is the ceiling of
			// the log base 2 of the number of input bits.
			selBits = (unsigned long)ceil( log((double)inBits) / log(2.0) );
			declareInputBus( "SEL", selBits );
		} else {
			selBits = 0;
		}

		//NOTE: Don't return "true" from this, because
		// you shouldn't be setting this param during simulation while
		// anything is connected anyhow!
		// Also, allow the Gate_N_INPUT class to change the number of inputs:
		return Gate_N_INPUT::setParameter( paramName, value );
	} else {
		return Gate_N_INPUT::setParameter( paramName, value );
	}
	return false;
}

