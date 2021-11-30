#include "gate_pulse.h"

// ******************************** Pulse GATE ***********************************
// The PULSE gate simply creates a pulse of a specified duration
// in simulation steps. By setting the parameter PULSE, it sets the
// remaining duration of the pulse. Once the duration expires, the
// output will return to 0. If a pulse is still going when another
// PULSE parameter is sent, then the pulse is extended to the normal
// end time of the last pulse.
//NOTE: This is a "polled" gate, so it will always be checked, just
// like Gate_CLOCK.

Gate_PULSE::Gate_PULSE() : Gate() {
	pulseRemaining = 0;
	
	// Declare the output:
	declareOutput("OUT_0");
}


// Handle gate events:
void Gate_PULSE::gateProcess( void ) {
	// The output is ONE if there is pulse remaining, and ZERO otherwise:
	setOutputState( "OUT_0", (pulseRemaining > 0) ? ONE : ZERO, 0 );

	// Decrement the remaining number of steps that the pulse is high.
	if( pulseRemaining != 0 ) pulseRemaining--;
}


// Set the pulses:
bool Gate_PULSE::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "PULSE" ) {
		iss >> pulseRemaining;
		return false; // It's a polled gate, so don't update it otherwise or the pulse count will be wrong.
	} else {
		return Gate::setParameter( paramName, value );
	}
}

// **************************** END Pulse GATE ***********************************