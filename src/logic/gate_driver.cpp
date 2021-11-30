#include "gate_driver.h"


// ******************************** Driver GATE ***********************************
// Can be used to drive a bus of n bits to a specific binary number.

// Initialize the starting state and the output:
Gate_DRIVER::Gate_DRIVER() : Gate() {
	// The default output number is 0:
	output_num = 0;

	// Default of 0 outputs (Must be specified in library file, or no inputs will be made!):
	setParameter("OUTPUT_BITS", "0");
}


// Handle gate events:
void Gate_DRIVER::gateProcess( void ) {
	// All the driver gate does is throw events IMMEDIATELY
	// whenever the gate has changed state:
	setOutputBusState( "OUT", ulong_to_bus(output_num, outBits), 0 );
}


// Set the toggle state variable:
bool Gate_DRIVER::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "OUTPUT_NUM" ) {
		iss >> output_num;
		return true; // Update the gate during the next step!
	} else if( paramName == "OUTPUT_BITS" ) {
		iss >> outBits;

		// Declare the output pins!		
		if( outBits > 0 ) {
			declareOutputBus( "OUT", outBits );
		}

		//NOTE: Don't return "true" from this, because
		// you shouldn't be setting this param during simulation while
		// anything is connected anyhow!
	} else {
		return Gate::setParameter( paramName, value );
	}
	return false;
}


// Get the toggle state variable:
string Gate_DRIVER::getParameter( string paramName ) {
	ostringstream oss;
	if( paramName == "OUTPUT_NUM" ) {
		oss << output_num;
		return oss.str();
	} else if( paramName == "OUTPUT_BITS" ) {
		oss << outBits;
		return oss.str();
	} else {
		return Gate::getParameter( paramName );
	}
}


// **************************** END Driver GATE ***********************************
