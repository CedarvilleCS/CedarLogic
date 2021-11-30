#include "gate_junction.h"
#include "logic_circuit.h"

// ******************************** Junction GATE ***********************************
// This class uses the circuit's Junctioning capabilities
// to enable and disable a junction and splice the inputs
// into the junction, to allow true to/from nodes.


// This is the mapping of junction states, and how often each is used (# of gates):
//ID_MAP< string, IDType > Gate_JUNCTION::junctionIDs;
//ID_MAP< string, unsigned long > Gate_JUNCTION::junctionUseCounter;


// Initialize the starting state and the output:
Gate_JUNCTION::Gate_JUNCTION( Circuit *newCircuit ) : Gate() {

	// Keep the circuit pointer, to use to access the Junctions
	myCircuit = newCircuit;

	// The only attributes that this gate has is what wires
	// are hooked up, and what its name is. Set the name
	// using "setParameter" so that the new Junction will be
	// created if needed:
	this->setParameter( "JUNCTION_ID", "NONE" );

/*	declareInput( "IN1", 0 );
	declareOutput( "OUT1", 0 );
	
	this->isFrom = isFrom;
*/
}


// Remove this junction's claim on the junction ID:
Gate_JUNCTION::~Gate_JUNCTION() {
	if( !((*(myCircuit->getJunctionIDs())).empty() ) && ((*(myCircuit->getJunctionIDs())).find( myID ) != (*(myCircuit->getJunctionIDs())).end()) ) {
		(*(myCircuit->getJunctionUseCounter()))[myID] -= 1;
		// Unhook this gate's wires from the old junction:
		ID_SET< IDType >::iterator thisWire = myWires.begin();
		while( thisWire != myWires.end() ) {
			myCircuit->disconnectJunction( (*(myCircuit->getJunctionIDs()))[myID], *thisWire );
			thisWire++;
		}

		// If that junction is no longer used, then erase it:
		if( (*(myCircuit->getJunctionUseCounter()))[myID] == 0 ) {
			// Erase the junction from the Circuit:
			myCircuit->deleteJunction( (*(myCircuit->getJunctionIDs()))[myID] );

			// Erase the junction from the junction maps:				
			(*(myCircuit->getJunctionIDs())).erase( myID );
			(*(myCircuit->getJunctionUseCounter())).erase( myID );
		}

	}
}


// Handle gate events:
void Gate_JUNCTION::gateProcess( void ) {
	// Do nothing, 'cause the Junction object does all the work for us!
}


// Set the junction's ID:
bool Gate_JUNCTION::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "JUNCTION_ID" ) {
		string myOldID = myID;

		// Read in the new ID:
		myID = value; // (We want to include whitespace in them, too.)
//		iss >> myID;

		// (Note that the first time that this is called is from the
		// constructor, which calls it with the string "NONE" as the new
		// id, and myID is uninitialized, which means it contains "" already.)

		// If we didn't change the junction ID, then we're done:
		if( myOldID == myID ) return false;
		// Decrement the old junction id counter:
		if( !((*(myCircuit->getJunctionIDs())).empty() ) && ((*(myCircuit->getJunctionIDs())).find( myOldID ) != (*(myCircuit->getJunctionIDs())).end()) ) {
			(*(myCircuit->getJunctionUseCounter()))[myOldID] -= 1;
			// Unhook this gate's wires from the old junction:
			ID_SET< IDType >::iterator thisWire = myWires.begin();
			while( thisWire != myWires.end() ) {
				myCircuit->disconnectJunction( (*(myCircuit->getJunctionIDs()))[myOldID], *thisWire );
				thisWire++;
			}

			// If that junction is no longer used, then erase it:
			if( (*(myCircuit->getJunctionUseCounter()))[myOldID] == 0 ) {
				// Erase the junction from the Circuit:
				myCircuit->deleteJunction( (*(myCircuit->getJunctionIDs()))[myOldID] );

				// Erase the junction from the junction maps:				
				(*(myCircuit->getJunctionIDs())).erase( myOldID );
				(*(myCircuit->getJunctionUseCounter())).erase( myOldID );
			}

		}

		// If the junction does not already exist, then create it. Otherwise,
		// simply increment the "use counter":
		if( (*(myCircuit->getJunctionIDs())).find( myID ) == (*(myCircuit->getJunctionIDs())).end() ) {
			// Create the new junction in the circuit:
			(*(myCircuit->getJunctionIDs()))[myID] = myCircuit->newJunction();

			// Add the use counter:
			(*(myCircuit->getJunctionUseCounter()))[myID] = 1;
		} else {
			(*(myCircuit->getJunctionUseCounter()))[myID] += 1;
		}

		// Add this gate's wires to the newly assigned junction:
		ID_SET< IDType >::iterator thisWire = myWires.begin();
		while( thisWire != myWires.end() ) {
			myCircuit->connectJunction( (*(myCircuit->getJunctionIDs()))[myID], *thisWire );
			thisWire++;
		}

		return false; // gateProcess() doesn't do anything anyway!
	} else {
		return Gate::setParameter( paramName, value );
	}
}


// Set the junction's ID:
string Gate_JUNCTION::getParameter( string paramName ) {
	ostringstream oss;
	if( paramName == "JUNCTION_ID" ) {
		oss << myID;
		return oss.str();
	} else {
		return Gate::getParameter( paramName );
	}
}


// Connect a wire to the input of this gate:
void Gate_JUNCTION::connectInput( string inputID, IDType wireID ) {
	Gate::connectInput( inputID, wireID );

	// Connect the wire to the junction in the Circuit:	
	myCircuit->connectJunction( (*(myCircuit->getJunctionIDs()))[myID], wireID );

	// Track this wire, so that it can move to a new junction if
	// our name changes:
	myWires.insert( wireID );
	if( myWireCounts.find( wireID ) == myWireCounts.end() ) {
		myWireCounts[wireID] = 1;
	} else {
		myWireCounts[wireID] += 1;
	}

//TODO: Decide whether or not it is a good idea to allow inputs
// to be connected to the gate, or if they should all be outputs, to avoid
// the wire having a dangling wireInput object.
}


// Disconnect a wire from the input of this gate:
// (Returns the wireID of the wire that was connected.)
IDType Gate_JUNCTION::disconnectInput( string inputID ) {
	IDType wireID = ID_NONE;
	
	// Call the gate's method:
	wireID = Gate::disconnectInput( inputID );

	if( wireID != ID_NONE ) {
		// Unhook the wire from the Junction in the Circuit:
		myCircuit->disconnectJunction( (*(myCircuit->getJunctionIDs()))[myID], wireID );
	}

	// Erase the wire from our tracking list, so that we won't keep it anymore:
	if( myWireCounts[wireID] == 1 ) {
		myWires.erase( wireID );
		myWireCounts.erase( wireID );
	}

	return wireID;
}


// **************************** END Junction GATE ***********************************

