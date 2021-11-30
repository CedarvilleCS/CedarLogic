#include "gate_pauselator.h"

//***************************************************************
//Edit by Joshua Lansford 6/5/2007
//This edit is to create a new gate called the pauseulator.
//This gate has one input and no outputs.  When the input of this
//gate goes high, then it will pause the simulation.  This takes
//avantage of the pauseing hooks that I had to create for the Z80.
Gate_pauseulator::Gate_pauseulator() : Gate(){
	declareInput( "signal", true );	
}

void Gate_pauseulator::gateProcess( void ) {
	if( isRisingEdge( "signal" ) ){
		listChangedParam( "PAUSE_SIM" );
	}
}

bool Gate_pauseulator::setParameter( string paramName, string value ) {
	//this is here to catch PAUSE_SIM so that when we load
	//and PAUSE_SIM gets thrown at us from the file,
	//we will pretend to do something with it.
	return false;
}

string Gate_pauseulator::getParameter( string paramName ) {
	//the only param that the system might we wanting is
	//PAUSE_SIM, so we will return "TRUE" because we only
	//flag it when it is true.
	return "TRUE";
}


//End of edit****************************************************

