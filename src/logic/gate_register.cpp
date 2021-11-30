#include "gate_register.h"

// ******************************** Register GATE ***********************************

Gate_REGISTER::Gate_REGISTER() : Gate_PASS() {
	// Declare the inputs:
	declareInput("clock", true);
	declareInput("clock_enable");
	declareInput("clear");
	declareInput("set");
	declareInput("load");

	declareInput("count_enable");
	declareInput("count_up"); // Favors "up" if not connected!

// For shift reg:
	declareInput("shift_enable");
	declareInput("shift_left"); // Favors "left" if not connected!
	declareInput("carry_in");

	// (Load input bus and the output bus are declared by Gate_PASS):
	declareOutput("carry_out");

// The input state priority goes like this:
// clear, set, load, count_enable, shift_enable, hold
// (So if all of these are set to ONE, the gate will CLEAR and ignore the others. So,
// the input states for the LOAD operation is (load && !clear && !set), even if none of them
// are syncronous. )
// Also, when doing a load operation, un-resolvable inputs are defaulted to 0's.

	// Set the default settings:
	syncSet = true;
	syncClear = true;
	syncLoad = true;
	disableHold = false;
    unknownOutputs = false;
	currentValue = 0;
	maxCount = 0;

	// An initialization value, to make REGISTERs initialize more
	// nicely when loading them or making new ones:
	firstGateProcess = true;
}


// Handle gate events:
void Gate_REGISTER::gateProcess( void ) {
	vector< StateType > outBus;
	StateType carryOut = ZERO; // Assume that carry out is reset.

	// If this is the first time this gate has been simulated,
	// then output the currentValue to the pins:
	if( firstGateProcess ) {
		firstGateProcess = false;
		outBus = ulong_to_bus( currentValue, inBits );
	}

	// Track to see if the current value changes, to know if to send
	// an update message to the GUI:
	unsigned long oldCurrentValue = currentValue;

	// Update outBus and currentValue based on the input states.
	if( getInputState("clear") == ONE ) {
		if(hasClockEdge()) {
			// Clear.
			currentValue = 0;
			outBus = ulong_to_bus( currentValue, inBits );
		}
	} else if( getInputState("set") == ONE ) {
		if(hasClockEdge()) {
			// Set.
			vector< StateType > allOnes( inBits, ONE );
			outBus = allOnes;
			currentValue = bus_to_ulong( outBus );
		}
	} else if( getInputState("load") == ONE ) {
		if(hasClockEdge()){
			// Load.
			vector< StateType > inputBus = getInputBusState("IN");
			for( unsigned long i = 0; i < inputBus.size(); i++ ) {
				if( (inputBus[i] == CONFLICT) || (inputBus[i] == HI_Z) ) {
					inputBus[i] = UNKNOWN;
				}
			}
			currentValue = bus_to_ulong( inputBus );
			outBus = inputBus;
		}
	} else if( getInputState("count_enable") == ONE ) {
		// Count.
		if( isRisingEdge("clock") ) {
			// Only count down if count_up is ZERO. This allows
			// HI_Z, CONFLICT, and UNKNOWN to favor counting upwards.
			if( getInputState("count_up") == ZERO ) {
				// Decrement the counter:
				if( (currentValue == 0) || (currentValue > maxCount) ) {
					currentValue = maxCount;
				} else {
					// (currentValue > 0)
					currentValue--;
				}
				outBus = ulong_to_bus( currentValue, inBits );
			} else {
				// Increment the counter:
				currentValue = (currentValue + 1) % (maxCount + 1);
				outBus = ulong_to_bus( currentValue, inBits );
			}
		}

		// Set the carry out bit, regardless of the clock edge:		
		if( getInputState("count_up") == ZERO ) {
			if( currentValue == 0 ) carryOut = ONE; // Carry out on ZERO count when downcounting.
		} else {
			if( currentValue == maxCount ) carryOut = ONE; // Carry out on MAX count when upcounting.
		}

	} else if( getInputState("shift_enable") == ONE ) {
		// Shift.
		if( isRisingEdge("clock") ) {
			if( getInputState("shift_left") == ZERO ) { // Favors "left" if not connected!
				// Shift right.
				currentValue >>= 1;
	
				outBus = ulong_to_bus( currentValue, inBits );
				
				// Add the input carry if needed:
				if( getInputState("carry_in") == ONE ) {
					outBus[inBits - 1] = ONE;
					currentValue = bus_to_ulong( outBus );
				}
			} else {
				// Shift left.
				currentValue <<= 1;

				// Add the input carry if needed:
				if( getInputState("carry_in") == ONE ) {
					currentValue++;
				}

				// Throw away the extra bits that aren't part of the register,
				// so that when you switch to "right-shift", it doesn't remember
				// more than it should!
				unsigned long mask = 0;
				for( unsigned long i = 0; i < inBits; i++ ) {
					mask <<= 1;
					mask += 1;
				}
				currentValue = currentValue & mask;

				// Send the value to the bus:
				outBus = ulong_to_bus( currentValue, inBits );
			}
		}

		// Set the carry out bit, regardless of the clock edge:		
		vector< StateType > tempBus = ulong_to_bus( currentValue, inBits );
		if( getInputState("shift_left") == ZERO ) { // Favors "left" if not connected!
			// Shift right.
			carryOut = tempBus[0];
		} else {
			// Shift left.
			carryOut = tempBus[tempBus.size() - 1];
		}
	} else {
		// If hold is allowed, then keep the current value.

		if( disableHold ) {
		// Otherwise, load in what is on the input pins:
			if(hasClockEdge()){
				// Load.
				vector< StateType > inputBus = getInputBusState("IN");
				for( unsigned long i = 0; i < inputBus.size(); i++ ) {
					if( (inputBus[i] == CONFLICT) || (inputBus[i] == HI_Z) ) {
						inputBus[i] = UNKNOWN;
					}
				}
				currentValue = bus_to_ulong( inputBus );
				outBus = inputBus;
			}
		}

	}

	// Set the output values:
	setOutputState("carry_out", carryOut);
	
	//********************************
	//Edit by Joshua Lansford 3/15/07
	//While it makes the most sence
	//to let the output of a latch or
	//register to be unknown if
	//it has latched an unknown input,
	//in practicality it is a real pain.
	//When implementing a finite state
	//machine, it is a nusence if
	//the whole thing is in an infinite
	//state of unknowingness
	for( vector< StateType >::iterator I = outBus.begin(); I != outBus.end(); ++I ){
		if( *I != ONE ){
			*I = ZERO;
		}
	}		
	//End of edit**********************
	
	if( outBus.size() != 0 ) {
		setOutputBusState("OUT", outBus);
		setOutputBusState("OUTINV", outBus);
		
		// Check if any of the outputs are "unknown" state, and send that info on
		// to the GUI:
		bool oldUO = unknownOutputs;
		unknownOutputs = false;
		for( unsigned int i = 0; i < outBus.size(); i++ ) {
			if( outBus[i] == UNKNOWN ) {
				unknownOutputs = true;
			}
		}
		// Update the GUI's knowledge of our unknown outputs state, if it has changed:
		if( oldUO != unknownOutputs ) {
			listChangedParam("UNKNOWN_OUTPUTS");
		}
	}
	
	// Update the GUI's knowledge of our current value, if it has changed:
	if( currentValue != oldCurrentValue ) {
		listChangedParam("CURRENT_VALUE");
	}
}


// Set the parameters:
bool Gate_REGISTER::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "CURRENT_VALUE" ) {
		iss >> currentValue;
		return true;
	} else if( paramName == "UNKNOWN_OUTPUTS" ) {
		string setVal;
		iss >> setVal;

		unknownOutputs = (setVal == "true");
	} else if( paramName == "MAX_COUNT" ) {
		iss >> maxCount;
	} else if( paramName == "SYNC_SET" ) {
		string setVal;
		iss >> setVal;

		syncSet = (setVal == "true");
		return false;
	} else if( paramName == "SYNC_CLEAR" ) {
		string setVal;
		iss >> setVal;

		syncClear = (setVal == "true");
		return false;
	} else if( paramName == "SYNC_LOAD" ) {
		string setVal;
		iss >> setVal;

		syncLoad = (setVal == "true");
		return false;
	} else if( paramName == "NO_HOLD" ) {
		string setVal;
		iss >> setVal;

		disableHold = (setVal == "true");
		return false;
	} else if( paramName == "INPUT_BITS" ) {
		iss >> inBits;

		// Declare the inverted output pins:
		if( inBits > 0 ) {
			declareOutputBus( "OUTINV", inBits );

			// Make all of the OUTINV pins inverted:
			ostringstream oss;
			for( unsigned long i = 0; i < inBits; i++ ) {
				oss.str("");
				oss.clear();
				oss << "OUTINV_" << i;
				setOutputInverted( oss.str(), true );
			}
		}

		//NOTE: Don't return "true" from this, because
		// you shouldn't be setting this param during simulation while
		// anything is connected anyhow!
		// Also, allow the Gate_PASS class to change the number of inputs:
		return Gate_PASS::setParameter( paramName, value );
	} else {
		return Gate_PASS::setParameter( paramName, value );
	}
	return false;
}


// Set the parameters:
string Gate_REGISTER::getParameter( string paramName ) {
	ostringstream oss;
	if( paramName == "CURRENT_VALUE" ) {
		oss << currentValue;
		return oss.str();
	} else if( paramName == "UNKNOWN_OUTPUTS" ) {
		oss << (unknownOutputs ? "true" : "false");
		return oss.str();
	} else if( paramName == "MAX_COUNT" ) {
		oss << maxCount;
		return oss.str();
	} else if( paramName == "SYNC_SET" ) {
		oss << (syncSet ? "true" : "false");
		return oss.str();
	} else if( paramName == "SYNC_CLEAR" ) {
		oss << (syncClear ? "true" : "false");
		return oss.str();
	} else {
		return Gate_PASS::getParameter( paramName );
	}
}

bool Gate_REGISTER::hasClockEdge() {
	return isRisingEdge("clock") && getInputState("clock_enable") != ZERO || !syncLoad;
}

// **************************** END Register GATE ***********************************