#include "gate_decoder.h"


// ******************************** DECODER GATE ***********************************


// Initialize the gate's interface:
Gate_DECODER::Gate_DECODER() : Gate_N_INPUT() {

	// The control inputs data inputs are declared in setParameter().
	// (Must be set before using this method!)
	setParameter("INPUT_BITS", "0");

	//Josh Edit 4/6/2007
	declareInput("ENABLE");
	
	//Josh Edit 10/3/2007
	declareInput("ENABLE_B");
	declareInput("ENABLE_C");

	// One output:
	declareOutput("OUT");
}


// Handle gate events:
void Gate_DECODER::gateProcess( void ) {
	vector< StateType > inBus = getInputBusState("IN");
	unsigned long inNum = bus_to_ulong( inBus ); //NOTE: The DECODER assumes 0 on non-specified input lines (Not UNKNOWN)!

	vector< StateType > outBus( outBits, ZERO ); // All bits are 0, except for the active

	//********************************
	//Edit by Joshua Lansford 6/4/2007
	//Reason: The docoder is used with
	//the Z80 to select ports in order
	//to enable them and disable them.
	//the ENABLE input connected to
	//the inverted /IORQ signal.
	//The problem is that when the chip
	//is disabled, the outputs were 
	//floating. This caused ports to
	//not know if they were selected
	//or not and then to fry the data 
	//lines with unknownness.
	//Change: To fix this I have
	//changed the enable input from
	//ENABLE_0 to just ENABLE. The
	//enable is then manually checked
	//to see if it is active before
	//activating an output.
	
	//*************************************
	//Reedit by Joshua Lansford 10/3/2007
	//Resion: I am creating another decoder
	//implementation that needs three enables.
	//for the same resion as states above,
	//(I can't have the outputs float on a
	//disable), I can't use the default enables.
	//Thus I am creating my own.  I am asumeing
	//that it is OK to declare inputs that
	//are not used on the gui side.
	
	bool enabled = true;
	
	//by testing for ZERO instead of one, we let a floating enable
	//be enabling.
	if( getInputState("ENABLE") == ZERO || getInputState("ENABLE_B") == ZERO ||
	    getInputState("ENABLE_C") == ZERO ){
	    	enabled = false;
	}
	
	if( enabled && inNum < outBus.size() ) {
	
	//End of edit *********************
	
		outBus[inNum] = ONE;
	}

	setOutputBusState("OUT", outBus);
}


// Set the parameters:
bool Gate_DECODER::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "INPUT_BITS" ) {
		iss >> inBits;

		// Declare the selection pins!		
		if( inBits > 0 ) {
			// The number of output bits is the power of 2 of the
			// number of input bits.
			outBits = (unsigned long)ceil( pow( (double) inBits, 2.0 ) );
			declareOutputBus( "OUT", outBits );
		} else {
			outBits = 0;
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


// **************************** END DECODER GATE ***********************************
