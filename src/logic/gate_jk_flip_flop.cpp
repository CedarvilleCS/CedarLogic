#include "gate_jk_flip_flop.h"


// ******************* JK Flip Flop Gate *********************

Gate_JKFF::Gate_JKFF() : Gate() {
	// Declare the inputs:
	declareInput("clock", true);
	declareInput("J");
	declareInput("K");

	declareInput("set");
	declareInput("clear");

	// The outputs:
	declareOutput("Q");
	declareOutput("nQ");

	// The default state:
	currentState = ZERO;
	setParameter("SYNC_SET", "false");
	setParameter("SYNC_CLEAR", "false");
}


// Handle gate events:
void Gate_JKFF::gateProcess( void ) {
	// Get the input values (Unknown types are assumed as ZERO!):
	bool J = (getInputState("J") == ONE);
	bool K = (getInputState("K") == ONE);
	bool set = (getInputState("set") == ONE);
	bool clear = (getInputState("clear") == ONE);

	if( clear ) {
		if( (syncClear && isRisingEdge("clock")) || !syncClear ) {
			currentState = ZERO;
		}
	} else if( set ) {
		if( (syncSet && isRisingEdge("clock")) || !syncSet ) {
			currentState = ONE;
		}
	} else if( isRisingEdge("clock") ) {
		if( !J && !K ) {
			currentState = currentState; // Hold
		} else if( !J && K ) {
			currentState = ZERO; // Reset
		} else if( J && !K ) {
			currentState = ONE; // Set
		} else if( J && K ) {
			currentState = (currentState == ONE) ? ZERO : ONE; // Toggle
		}
	}
	// Set the output values:
	setOutputState("Q", currentState);
	setOutputState("nQ", (currentState == ONE) ? ZERO : ONE);
}



// Set the parameters:
bool Gate_JKFF::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "SYNC_SET" ) {
		string setVal;
		iss >> setVal;

		syncSet = (setVal == "true");
		return false;
	} else if( paramName == "SYNC_CLEAR" ) {
		string setVal;
		iss >> setVal;

		syncClear = (setVal == "true");
		return false;
	} else {
		return Gate::setParameter( paramName, value );
	}
	return false;
}


// Get the parameters:
string Gate_JKFF::getParameter( string paramName ) {
	ostringstream oss;
	if( paramName == "SYNC_SET" ) {
		oss << (syncSet ? "true" : "false");
		return oss.str();
	} else if( paramName == "SYNC_CLEAR" ) {
		oss << (syncClear ? "true" : "false");
		return oss.str();
	} else {
		return Gate::getParameter( paramName );
	}
}


// **************************** END JK Flip Flop GATE ***********************************
