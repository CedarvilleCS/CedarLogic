#include "gate_clock.h"


// ******************************** CLOCK GATE ***********************************


// Initialize the half cycle:
Gate_CLOCK::Gate_CLOCK( TimeType newHalfCycle ) : Gate(), halfCycle(newHalfCycle) {
	theState = ZERO;
	
	// Declare the output:
	declareOutput("CLK");
}


// Handle gate events:
void Gate_CLOCK::gateProcess( void ) {
	
	TimeType now = getSimTime();
	
	if( (halfCycle > 0) && ( now % halfCycle == 0 ) ) {
		if( theState == ZERO ) theState = ONE;
		else theState = ZERO;
	}

	setOutputState( "CLK", theState, 0 );
}


// Set the clock rate:
bool Gate_CLOCK::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "HALF_CYCLE" ) {
		iss >> halfCycle;
		return false;
	} else {
		return Gate::setParameter( paramName, value );
	}
}


// Get the clock rate:
string Gate_CLOCK::getParameter( string paramName ) {
	ostringstream oss;
	if( paramName == "HALF_CYCLE" ) {
		oss << halfCycle;
		return oss.str();
	} else {
		return Gate::getParameter( paramName );
	}
}


// **************************** END CLOCK GATE ***********************************