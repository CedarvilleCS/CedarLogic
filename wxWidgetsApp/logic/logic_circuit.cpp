/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   
*****************************************************************************/

// logic_circuit.cpp: implementation of the Circuit class.
//
//////////////////////////////////////////////////////////////////////

#include "logic_circuit.h"
#include <iostream>
#include <algorithm>
#include <stack>
#include <iterator>

#include "../Z80/Z_80LogicGate.h"

#ifndef _PRODUCTION_
ofstream* logiclog;
#endif

// If this is defined, use inertial delay, otherwise default to transport delay
//#define INERTIAL_DELAY


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Circuit::Circuit()
{
	// Set time to begin starting:
	systemTime = 0;
	
	// Set the initial ID values:
	gateIDCount = 0;
	wireIDCount = 0;
	juncIDCount = 0;
	
#ifndef _PRODUCTION_
	logiclog = new ofstream( "corelog.log");
#endif
}

Circuit::~Circuit()
{
	// Cause all of the gates to destroy themselves
	// before the Circuit gets destroyed, or any other
	// objects are removed:
	// (This is needed so that if their destructors call
	// any Circuit methods, they won't crash.)

	gateList.clear();
}

// **************** The visible interface of the circuit ********************

// ********** Circuit input methods *************

//***************************************
//Edit by Joshua Lansford 3/27/07
//purpose of edit: The ram gate needs to
//update its pop-up right after it loads
//even if it is paused.  Thus this method
//is created, so that the pop-ups
//can request that the core proces
//thegateUpdateList without advanceing
//the system time
void Circuit::stepOnlyGates(){
	// Update the gates that have been connected or disconnected or had a 
	// parameter change within the last call to step() so that they can
	// recalculate correctly:
	ID_SET< IDType >::iterator updateGate = gateUpdateList.begin();
	while( updateGate != gateUpdateList.end() ) {
		gateList[*updateGate]->updateGate( *updateGate, this );
		updateGate++;
	}
	gateUpdateList.clear();
}
//End of Edit*****************************


// Step the simulation forward by one timestep:
// If a pointer to a set is passed, then it will
// return a set of all the changed wires to the calling function.
void Circuit::step( ID_SET< IDType > *changedWires )
{
	// NOTE: Should activate the polled gates here:
	// Basically just loop through the things in polledGates and call updateGate() on them.
	ID_SET< IDType >::iterator gateToPoll = polledGates.begin();
	while( gateToPoll != polledGates.end() ) {
		gateList[*gateToPoll]->updateGate( *gateToPoll, this );
		gateToPoll++;
	}

	// Update the gates that have been connected or disconnected or had a 
	// parameter change within the last call to step() so that they can
	// recalculate correctly:
	ID_SET< IDType >::iterator updateGate = gateUpdateList.begin();
	while( updateGate != gateUpdateList.end() ) {
		gateList[*updateGate]->updateGate( *updateGate, this );
		updateGate++;
	}
	gateUpdateList.clear();

	// Loop through all of the events with time == now, and 
	// activate them.
	bool makeYourOwnFreakinChangedWiresList = (changedWires == NULL);
	
	if( makeYourOwnFreakinChangedWiresList )
		changedWires = new ID_SET< IDType >;

	int processedEvents = 0;
	Event myEvent;
	if(!eventQueue.empty()) myEvent = eventQueue.top();
	while ( !eventQueue.empty() && (myEvent.eventTime <= systemTime) ) {
		// Pop the event off of the event queue:
		eventQueue.pop();
		
		// If the event is a junction event, handle it as a junction:
		if( myEvent.isJunctionEvent ) {
			// Handle the junction event:
			setJunctionState( myEvent.junctionID, myEvent.newJunctionState );

			// Also adds all of the wires hooked up to this junction to
			// the "wireUpdateList" list.
			// (Handled inside of a setJunctionState() method, to allow
			// it to be called from outside of an event handle - for zero delay.)
		} else {
			// Else, make the event happen to the wire:
			WIRE_PTR myWire = wireList[myEvent.wireID];
			myWire->setInputState(myEvent.gateID, myEvent.gateOutputID, myEvent.newState);

			// Insert all attached wires into the changed wires list:
			set< IDType > wireGroup = getJunctionGroupIDs( myEvent.wireID );
			changedWires->insert( wireGroup.begin(), wireGroup.end() );
		}
		
		// Look at the next thing in the list:
		if (!eventQueue.empty()) myEvent = eventQueue.top();
		
		processedEvents++;
	}
	
	// Insert the wires that have been disconnected (or were part of a junction that changed) within
	// the last call to step() so that they will be properly updated:
	changedWires->insert( wireUpdateList.begin(), wireUpdateList.end() );
	wireUpdateList.clear();	// Empty the wireUpdateList, since we are handling the updates.
	

	// Calculate the new wire states, and make a list of affected gates:
	ID_SET< IDType > changedGates;
	vector< IDType > affectedGates;

	// This keeps track of wires that share junctions that have already
	// calculated their state:
	ID_SET< IDType > doneWires;
	ID_SET< IDType >::iterator chgWireIterator = changedWires->begin();
	while ( chgWireIterator != changedWires->end() ) {
		WIRE_PTR myWire = wireList[*chgWireIterator];
		
		// Calculate the new state of a wire:
		// (Note: It sends the group of attached wires to the Wire::calculateState() method.
		if( doneWires.find( *chgWireIterator ) == doneWires.end() ) {
			set< IDType > wireGroupIDs = getJunctionGroupIDs( *chgWireIterator );
			set< WIRE_PTR > wireGroup = getJunctionGroup( &wireGroupIDs );
			StateType juncState = myWire->calculateState( wireGroup );
			
			set< WIRE_PTR >::iterator wgWire = wireGroup.begin();
			while( wgWire != wireGroup.end() ) {
				(*wgWire)->forceState( juncState );
				wgWire++;
			}
			
			doneWires.insert( wireGroupIDs.begin(), wireGroupIDs.end() );
		}
		
		// Add this wire's gates to the overall gate list:
		affectedGates = myWire->getOutputGates();
		changedGates.insert( affectedGates.begin(), affectedGates.end() );
		// NOTE: Aparently MSVCC doesn't like this last *STL standard and legal* line
		// of code.
		
		// Move on to the next wire in the list:
		chgWireIterator++;
	}
	
	// Update the gate's states and post the events from the gates:
	ID_SET< IDType >::iterator changedGatesIterator = changedGates.begin();
	

	// Update all of the gates and retrieve the events from them:
	while( changedGatesIterator != changedGates.end() ) {
		GATE_PTR myGate = gateList[*changedGatesIterator];
		
		myGate->updateGate( *changedGatesIterator, this );
		
		changedGatesIterator++;
	}

	// Increment the system timer, because this timestep is complete:
	systemTime++;
	
	// Remove the changedWiresList if we created it ourselves:
	if( makeYourOwnFreakinChangedWiresList )
		delete changedWires;
} // step()


// Create a new gate, and return its ID:
IDType Circuit::newGate( string type, IDType gateID ) {
	IDType thisGateID;

	if( gateID == ID_NONE ) {
		// If no ID is specified for the new gate, then generate one:
		thisGateID = gateIDCount;
		gateIDCount++;
	} else {
		// Otherwise use the one given and make sure that the ID is not
		// ever re-used for a generated ID:
		thisGateID = gateID;
		gateIDCount = max(gateID + 1, gateIDCount);
	}

	// If the gate isn't already created, then make it:
	if( gateList.find(thisGateID)== gateList.end() ) {

		// Create a gate of the proper type:
		if( type == "AND" ) {
			gateList[thisGateID] = GATE_PTR( new Gate_AND );
		} else if( type == "OR" ) {
			gateList[thisGateID] = GATE_PTR( new Gate_OR );
		} else if( type == "XOR" ) {
			gateList[thisGateID] = GATE_PTR( new Gate_XOR );
		} else if( type == "BUFFER" ) {
			gateList[thisGateID] = GATE_PTR( new Gate_PASS );
		} else if( type == "MUX" ) {
			gateList[thisGateID] = GATE_PTR( new Gate_MUX );
		} else if( type == "DECODER" ) {
			gateList[thisGateID] = GATE_PTR( new Gate_DECODER );
		} else if (type == "PRI_ENCODER") {
			// Added 10/4/16 -- Colin Broberg
			gateList[thisGateID] = GATE_PTR( new Gate_PRI_ENCODER );
		} else if( type == "CLOCK" ) {
			gateList[thisGateID] = GATE_PTR( new Gate_CLOCK );
			// This is a polled gate, so insert it into the polled gates queue!
			polledGates.insert(thisGateID);
		} else if( type == "PULSE" ) {
			gateList[thisGateID] = GATE_PTR( new Gate_PULSE );
			// This is a polled gate, so insert it into the polled gates queue!
			polledGates.insert(thisGateID);
		} else if( type == "DRIVER" ) {
			gateList[thisGateID] = GATE_PTR( new Gate_DRIVER );
		} else if( type == "ADDER" ) {
			gateList[thisGateID] = GATE_PTR( new Gate_ADDER );
		} else if( type == "COMPARE" ) {
			gateList[thisGateID] = GATE_PTR( new Gate_COMPARE );
		} else if( type == "JKFF" ) {
			gateList[thisGateID] = GATE_PTR( new Gate_JKFF );
		} else if( type == "RAM" ) {
			gateList[thisGateID] = GATE_PTR( new Gate_RAM );
		} else if( type == "REGISTER" ) {
			gateList[thisGateID] = GATE_PTR( new Gate_REGISTER );
		} else if( ( type == "FROM" ) || ( type == "TO" ) ) {
			gateList[thisGateID] = GATE_PTR( new Gate_JUNCTION( this ) );
		} else if( type == "TGATE" ) {
			gateList[thisGateID] = GATE_PTR( new Gate_T( this ) );
		} else if( type == "NODE" ) {
			gateList[thisGateID] = GATE_PTR( new Gate_NODE( this ) );
		} else if( type == "EQUIVALENCE" ) {
			gateList[thisGateID] = GATE_PTR( new Gate_EQUIVALENCE );
//*******************************************************************
//  Edit by Joshua Lansford 1/22/06
//  This edit is added because Nathan Harro and I are adding a new
//  gate type!!
		} else if( type == "Z80" ){
			gateList[thisGateID] = GATE_PTR( new Z_80LogicGate() );
// End of edit*******************************************************

//********************************
//  Edit by Joshua Lansford 4/10/07
//  now adding the ADC
		} else if( type == "ADC" ){
			gateList[thisGateID] = GATE_PTR( new Gate_ADC() );
// End of edit********************

//********************************
//  Edit by Joshua Lansford 6/05/07
//  now adding the pauseulator
		} else if( type == "Pauseulator" ){
			gateList[thisGateID] = GATE_PTR( new Gate_pauseulator() );
// End of edit*******************

		} else {
			WARNING( "Circuit::newGate() - Invalid logic type!" );
		}

	} else {
		WARNING( "Circuit::newGate() - Re-used gate ID!" );
	}
	
	// Always initialize the non-polled gates, so that it drives some value:
//NOTE: Crashed the small computer file for some reason.
//	if( polledGates.find( thisGateID ) == polledGates.end() ) {
//		gateUpdateList.insert( gateID );
//	}
	
	return thisGateID;
}

	
// Create a new wire and return its ID:
IDType Circuit::newWire( IDType wireID ) {
	IDType thisWireID;
	WIRE_PTR myWire(new Wire);
	
	if( wireID == ID_NONE ) {
		// If no ID is specified for the new wire, then generate one:
		thisWireID = wireIDCount;
		wireIDCount++;
	} else {
		// Otherwise use the one given and make sure that the ID is not
		// ever re-used for a generated ID:
		thisWireID = wireID;
		wireIDCount = max(wireID + 1, wireIDCount);
	}
	
	// If the wire isn't already created, then make it:
	if( wireList.find(thisWireID) == wireList.end() ) {
		wireList[thisWireID] = myWire;
	} else {
		WARNING( "Circuit::newWire() - Re-used wire ID!" );
	}
	
	return thisWireID;
}


// Create a new junction and return its ID:
IDType Circuit::newJunction( IDType juncID  ) {
	IDType thisJuncID;
	
	if( juncID == ID_NONE ) {
		// If no ID is specified for the new junction, then generate one:
		thisJuncID = juncIDCount;
		juncIDCount++;
	} else {
		// Otherwise use the one given and make sure that the ID is not
		// ever re-used for a generated ID:
		thisJuncID = juncID;
		juncIDCount = max(juncID + 1, juncIDCount);
	}

	JUNC_PTR myJunc(new Junction(thisJuncID) );

	// If the junction isn't already created, then make it:
	if( juncList.find(thisJuncID) == juncList.end() ) {
		juncList[thisJuncID] = myJunc;
	} else {
		WARNING( "Circuit::newJunction() - Re-used junction ID!" );
	}
	
	return thisJuncID;
}



// Delete a gate, removing its connections to wires first:
void Circuit::deleteGate( IDType theGate ) {
	if( gateList.find( theGate ) == gateList.end() ) {
		WARNING("Circuit::deleteGate() - Invalid gate ID.");
		return;
	}
	GATE_PTR myGate = gateList[ theGate ];
	
	// Delete the gate's inputs:
	while( myGate->getFirstConnectedInput() != "" ) {
		disconnectGateInput( theGate, myGate->getFirstConnectedInput() );
	}

	// Remove the gate from the update list, since disconnectGateInput()
	// will add it, and we don't want it updating after it's gone!
	gateUpdateList.erase( theGate );

	// Delete the gate's outputs:
	while( myGate->getFirstConnectedOutput() != "" ) {
		disconnectGateOutput( theGate, myGate->getFirstConnectedOutput() );
	}
	
	// Remove the gate from the circuit:
	gateList.erase( theGate );
	if ( polledGates.find( theGate ) != polledGates.end() ) polledGates.erase( theGate );
}


// Delete a wire, removing its connections from gates first:
// (The implementation of this may require that wires keep track
// of their input gates.)
void Circuit::deleteWire( IDType theWire ) {
	if( wireList.find( theWire ) == wireList.end() ) {
		WARNING("Circuit::deleteWire() - Invalid wire ID.");
		return;
	}
	WIRE_PTR myWire = wireList[theWire];
	
	// Delete the wire's inputs:
	WireInput tempI = myWire->getFirstInput();
	while( tempI.gateID != ID_NONE ) {
		disconnectGateOutput( tempI.gateID, tempI.gateOutputID );
		tempI = myWire->getFirstInput();
	}

	// Remove the wire from all connected junctions:
	// (This will put all of the connected wires into the update list to have their
	// state updated during the next step.)
	ID_SET< IDType > wireJuncs = myWire->getJunctions();
	ID_SET< IDType >::iterator theJunc = wireJuncs.begin();
	while( theJunc != wireJuncs.end() ) {
		disconnectJunction( *theJunc, theWire );
		theJunc++;
	}

	// Delete the wire's outputs:
	WireOutput tempO = myWire->getFirstOutput();
	while( tempO.gateID != ID_NONE ) {
		disconnectGateInput( tempO.gateID, tempO.gateInputID );
		tempO = myWire->getFirstOutput();
	}

	// Remove the wire from the update list, since disconnectGateOutput()
	// and disconnectJunction() will add it, and we don't want it
	// updating after it's gone!
	wireUpdateList.erase( theWire );

	// Remove the wire from the circuit:
	wireList.erase( theWire );
}


// Delete a junction, removing all its connections from wires first:
void Circuit::deleteJunction( IDType theJunc ) {
	if( juncList.find( theJunc ) == juncList.end() ) {
		WARNING("Circuit::deleteJunction() - Invalid junction ID.");
		return;
	}
	JUNC_PTR myJunc = juncList[theJunc];
	
	// Unhook all of the junction's connections:
	// (This will put all of the connected wires into the update list to have their
	// state updated during the next step.)
	ID_SET< IDType > juncWires = myJunc->getWires();
	ID_SET< IDType >::iterator theWire = juncWires.begin();
	while( theWire != juncWires.end() ) {
		disconnectJunction( theJunc, *theWire );
		theWire++;
	}

	// Take the junction out of the event list, to avoid calling events on it
	// after it has been removed.
	// Empty the priority queue into a temporary stack, filtering out outdated events:
	stack< Event > tempEventStack;
	while( !eventQueue.empty() ) {
		Event tempEvent = eventQueue.top();
		if( !((tempEvent.isJunctionEvent) && (tempEvent.junctionID == theJunc)) ) {
			tempEventStack.push( tempEvent );
		}
		eventQueue.pop();
	}
		
	// Push the stack back into the event queue:
	while( !tempEventStack.empty() ) {
		eventQueue.push( tempEventStack.top() );
		tempEventStack.pop();
	}

	// Remove the junction from the circuit:
	juncList.erase( theJunc );
}

/*OBSOLETE
// Connect an external event output to this wire, and return a new wire input ID
// which the output can connect to:
IDType extCount = 0;
IDType Circuit::connectExternalWireInput( IDType theWire ) {
	if( wireList.find( theWire ) != wireList.end() ) {
		extCount++; // Get a unique ID for the external input.
		(wireList[theWire])->connectInput(ID_NONE, extCount);
		return extCount;
	} else {
		WARNING("Circuit::connectExternalWireInput() - Invalid wire ID.");
		return ID_NONE;
	}
}
*/

// Connect a gate input to the output of a wire:
IDType Circuit::connectGateInput( IDType gateID, string gateInputID, IDType wireID ) {
	IDType returnWireID = 0;
	
	// First of all, create the wire if it doesn't already exist:
	if( wireList.find(wireID) == wireList.end() ) {
		returnWireID = newWire(wireID);
	}

	// Hook the gate input to the wireID:
	(gateList[gateID])->connectInput( gateInputID, wireID );
	
	// Hook the wire output to the gateID:
	(wireList[wireID])->connectOutput( gateID, gateInputID );
	
	//TODO: Should trigger some kind of event since the wire now is connected to this here gate,
	// and therefore the gate's input has changed!
			// Gate needs to update its state and pass along events if outputs changed.
			// We basically just need to force it into the update list.
	gateUpdateList.insert( gateID );
	
	return returnWireID;
}

	
// Connect a gate output to the input of a wire:
// (The wire will create one unique input for each unique gateID/gateOutputID combination.)
IDType Circuit::connectGateOutput( IDType gateID, string gateOutputID, IDType wireID) {
	IDType returnWireID = 0;
	
	// First of all, create the wire if it doesn't already exist:
	if( wireList.find(wireID) == wireList.end() ) {
		returnWireID = newWire(wireID);
	}

	// Connect the wire input to the gate:
	(wireList[wireID])->connectInput( gateID, gateOutputID );

	// Connect the gate output to the wire:
	(gateList[gateID])->connectOutput( gateOutputID, wireID );
	
	
	//TODO: Should trigger some kind of event since the gate is now providing a new input
	// to this wire, and the wire's state has changed!
			// Should be: wireinput.state == gate.lastoutput, unless an event is already
			// scheduled for this gate output.
			// Could do weird things if we connect a wire between the time a gate issued
			// an event and the time that it is processed. ,'o)
			// We need to force the gate to update and re-issue the last event for just this wire.
			// But it needs to be an event so that the wire will update its output gates, too.
		// My decision:
			// I'm going to have the gate output keep track of its last event, and when the
			// wire is connected, we will simply tell the gate to resend its last event to
			// the newly connected wire.
	(gateList[gateID])->resendLastEvent( gateID, gateOutputID, this );
	
	return returnWireID;
}

	
// Disconnect a gate input from the output of a wire:
void Circuit::disconnectGateInput( IDType gateID, string gateInputID ) {
	if( gateList.find( gateID ) == gateList.end() ) {
		WARNING("Circuit::disconnectGateInput() - Invalid gate ID.");
		return;
	}
	GATE_PTR myGate = gateList[ gateID ];

	// Disconnect the gate from the wire:
	IDType theWire = myGate->disconnectInput( gateInputID );
	
	// Disconnect the wire from the gate:
	if( wireList.find( theWire ) != wireList.end() ) {
		WIRE_PTR myWire = wireList[ theWire ];
		myWire->disconnectOutput(gateID, gateInputID );
	} else if( theWire != ID_NONE ) {
		WARNING("Circuit::disconnectGateInput() - Wire not found.");
	}

	//TODO: Trigger event to update gate.
			// Gate needs to update its state and pass along events if outputs changed.
			// We basically just need to force it into the update list.
	gateUpdateList.insert( gateID );
}

	
// Disconnect a gate output from the input of a wire:
void Circuit::disconnectGateOutput( IDType gateID, string gateOutputID ) {
	if( gateList.find( gateID ) == gateList.end() ) {
		WARNING("Circuit::disconnectGateOutput() - Invalid gate ID.");
		return;
	}
	GATE_PTR myGate = gateList[ gateID ];

	// Wire needs to update based on its other inputs and
	// cause its output gates to update as well. Just force it onto the update list.
	// ADDITION: All wires connected by junctions also need to be updated, and
	// the list needs to be made *before* the wire is disconnected.
	IDType theWire = myGate->getOutputWire( gateOutputID );
	ID_SET< IDType > juncWires = getJunctionGroupIDs( theWire );
	wireUpdateList.insert( juncWires.begin(), juncWires.end() );

	// Disconnect the gate from the wire:
	myGate->disconnectOutput( gateOutputID );
	
	// Disconnect the wire from the gate:
	if( wireList.find( theWire ) != wireList.end() ) {
		WIRE_PTR myWire = wireList[ theWire ];
		myWire->disconnectInput(gateID, gateOutputID );
	} else if( theWire != ID_NONE ) {
		WARNING("Circuit::disconnectGateOutput() - Wire not found.");
		return;
	}

	// You also have to clear the event queue of any events scheduled for this
	// gate/gateOutput combination.
	
	// Empty the priority queue into a temporary stack, filtering out outdated events:
	stack< Event > tempEventStack;
	while( !eventQueue.empty() ) {
		Event tempEvent = eventQueue.top();
		if( !((!tempEvent.isJunctionEvent) && (tempEvent.gateID == gateID) && (tempEvent.gateOutputID == gateOutputID)) ) {
			tempEventStack.push( tempEvent );
		}
		eventQueue.pop();
	}
		
	// Push the stack back into the event queue:
	while( !tempEventStack.empty() ) {
		eventQueue.push( tempEventStack.top() );
		tempEventStack.pop();
	}

	return;
}


// Connect a junction to a wire:
void Circuit::connectJunction( IDType juncID, IDType wireID ) {
//TODO: Warn the user when a junction cannot happen!
	if( juncList.find( juncID ) == juncList.end() ) return;
	if( wireList.find( wireID ) == wireList.end() ) return;


	// Get the junction and wire:
	JUNC_PTR myJunc = juncList[juncID];
	WIRE_PTR myWire = wireList[wireID];
	
	// Link the wire to the junction.
	myJunc->connectWire( wireID );

	// Connect the wire to the junction:
	myWire->addJunction( juncID );

	// Put all the wires of the junction group into the update list to have its
	// state updated during the next step.
	// (Note: Do this before after hooking up the wire!)
	ID_SET< IDType > juncWires = getJunctionGroupIDs( wireID );
	wireUpdateList.insert( juncWires.begin(), juncWires.end() );
}

// Unhook a junction from a wire:
void Circuit::disconnectJunction( IDType juncID, IDType wireID ) {
//TODO: Warn the user when a junction cannot happen!
	if( juncList.find( juncID ) == juncList.end() ) return;
	if( wireList.find( wireID ) == wireList.end() ) return;

	// Put all the wires of the junction group into the update list to have its
	// state updated during the next step.
	// (Note: Do this before unhooking the wire!)
	ID_SET< IDType > juncWires = getJunctionGroupIDs( wireID );
	wireUpdateList.insert( juncWires.begin(), juncWires.end() );

	// Get the junction and wire:
	JUNC_PTR myJunc = juncList[juncID];
	WIRE_PTR myWire = wireList[wireID];
	
	// Unlink the wire from the junction.
	if( myJunc->disconnectWire( wireID ) ) {
		// If the junction has no more of this wire
		// connected to it, then unhook the wire from the junction:
		myWire->removeJunction( juncID );
	}
}



// Create an event and put it in the event queue:
void Circuit::createEvent( TimeType eventTime, IDType wireID, IDType gateID, string gateOutputID, StateType newState ) {
	Event myEvent;
	myEvent.eventTime = eventTime;
	myEvent.wireID = wireID;
	myEvent.gateID = gateID;
	myEvent.gateOutputID = gateOutputID;
	myEvent.newState = newState;

	ostringstream oss;
	oss << "Creating event for gate " << gateID << " output " << gateOutputID << " to state " << (int) newState << " at time = " << eventTime << "." << endl;
	WARNING(oss.str());

#ifdef INERTIAL_DELAY
	// Erase any other events in the queue with this gate output:
	// Clear the event queue of any events scheduled for this gate/gateOutput combination:
	
	// Empty the priority queue into a temporary stack, filtering out outdated events:
	stack< Event > tempEventStack;
	while( !eventQueue.empty() ) {
		Event tempEvent = eventQueue.top();
		if( !((!tempEvent.isJunctionEvent) && (tempEvent.gateID == gateID) && (tempEvent.gateOutputID == gateOutputID)) ) {
			tempEventStack.push( tempEvent );
		}
		eventQueue.pop();
	}
		
	// Push the stack back into the event queue:
	while( !tempEventStack.empty() ) {
		eventQueue.push( tempEventStack.top() );
		tempEventStack.pop();
	}
#endif

	// Push the event onto the event queue:
	eventQueue.push(myEvent);
}


// Create an event that occurs at systemTime + delay:
TimeType Circuit::createDelayedEvent( TimeType delay, IDType wireID, IDType gateID, string gateOutputID, StateType newState ) {
	if( (wireID != ID_NONE) && (gateOutputID != "") ) {
		createEvent( delay + getSystemTime(), wireID, gateID, gateOutputID, newState );
	}
	return delay + getSystemTime();
}


// Create Junction Event and put it in the event queue:
void Circuit::createJunctionEvent( TimeType eventTime, IDType juncID, bool newState ) {
	Event myEvent;
	myEvent.eventTime = eventTime;
	myEvent.isJunctionEvent = true;
	myEvent.newJunctionState = newState;
	myEvent.junctionID = juncID;

#ifdef INERTIAL_DELAY
	// Erase any other events in the queue with this gate output:
	// Clear the event queue of any events scheduled for this gate/gateOutput combination:
	
	// Empty the priority queue into a temporary stack, filtering out outdated events:
	stack< Event > tempEventStack;
	while( !eventQueue.empty() ) {
		Event tempEvent = eventQueue.top();
		if( !((tempEvent.isJunctionEvent) && (tempEvent.junctionID == juncID)) ) {
			tempEventStack.push( tempEvent );
		}
		eventQueue.pop();
	}
		
	// Push the stack back into the event queue:
	while( !tempEventStack.empty() ) {
		eventQueue.push( tempEventStack.top() );
		tempEventStack.pop();
	}
#endif


	// Push the event onto the event queue:
	eventQueue.push(myEvent);
}


// Clear out the event queue, destroying all events,
// and also erase all events in the gateUpdateList and wireUpdateList.
// This is used if we wanted a simulation where all of the wires
// start with "UNKNOWN" state and don't update until a signal
// from the outside world reaches them.
void Circuit::destroyAllEvents( void ) {

	while( !eventQueue.empty() ) {
		eventQueue.pop();
	}

	gateUpdateList.clear();
	wireUpdateList.clear();
}


// Set a gate parameter:
// (If the gate's parameter change requires the gate to be
// re-evaluated during the next cycle, then add it to the 
void Circuit::setGateParameter( IDType gateID, string paramName, string value ) {
	if( gateList.find( gateID ) != gateList.end() ) {
		WARNING("test");
		WARNING(paramName);
		if( gateList[gateID]->setParameter( paramName, value ) ) {
			// If the gate has changed parameters and needs updated, then
			// add it to the gateUpdateList:
			gateUpdateList.insert( gateID );
		}
	} else {
		WARNING("Circuit::setGateParameter() - Gate not found.");
	}
	return;
}

void Circuit::setGateInputParameter( IDType gateID, string inputID, string paramName, string value ) {
	if( gateList.find( gateID ) != gateList.end() ) {
		if( gateList[gateID]->setInputParameter( inputID, paramName, value ) ) {
			// If the gate has changed parameters and needs updated, then
			// add it to the gateUpdateList:
			gateUpdateList.insert( gateID );
		}
	} else {
		WARNING("Circuit::setGateParameter() - Gate not found.");
	}
	return;
}

void Circuit::setGateOutputParameter( IDType gateID, string outputID, string paramName, string value ) {
	if( gateList.find( gateID ) != gateList.end() ) {
		if( gateList[gateID]->setOutputParameter( outputID, paramName, value ) ) {
			// If the gate has changed parameters and needs updated, then
			// add it to the gateUpdateList:
			gateUpdateList.insert( gateID );
		}
	} else {
		WARNING("Circuit::setGateParameter() - Gate not found.");
	}
	return;
}


// ************ Circuit inspection methods **************

// Get the value of a gate parameter:
string Circuit::getGateParameter( IDType gateID, string paramName ) {

	if( gateList.find( gateID ) != gateList.end() ) {
		return gateList[ gateID ]->getParameter( paramName );
	} else {
		WARNING("Circuit::setGateParameter() - Gate not found.");
	}
	return "";
}


// Get a wire state by ID:
StateType Circuit::getWireState( IDType wireID ) {
	if( wireList.find( wireID ) != wireList.end() ) {
		return wireList[wireID]->getState();
	} else {
		WARNING("Circuit::getWireState() - Wire does not exist.");
		return UNKNOWN;
	}
}

// Get and set a junction's on/off toggle state:
void Circuit::setJunctionState( IDType juncID, bool newState ) {
//TODO: Warn the user when a junction doesn't exist!
	if( juncList.find( juncID ) == juncList.end() ) return;

	// Get the junction:
	JUNC_PTR myJunc = juncList[juncID];
	myJunc->setEnableState( newState );

	// Put all of the connected wires into the "wireUpdateList" list to have their
	// state updated during the next (or current) step() call.
	//NOTE: MUST add ALL wires in ALL junction nodes that are attached to this junction!
	ID_SET< IDType > juncWires = myJunc->getWires();
	ID_SET< IDType >::iterator juncWire = juncWires.begin();
	while( juncWire != juncWires.end() ) {
		ID_SET< IDType > juncGroup = getJunctionGroupIDs( *juncWire );
		wireUpdateList.insert( juncGroup.begin(), juncGroup.end() );
		juncWire++;
	}
}

bool Circuit::getJunctionState( IDType juncID ) {
//TODO: Warn the user when a junction doesn't exist!
	if( juncList.find( juncID ) == juncList.end() ) return false;

	// Get the junction:
	JUNC_PTR myJunc = juncList[juncID];
	return myJunc->getEnableState();
}


// Return the current simulation time:
TimeType Circuit::getSystemTime( void ) {
	return systemTime;
}


// Returns a list of IDs of wires that are connected to this
// wire via junctions:
set< IDType > Circuit::getJunctionGroupIDs( IDType wireID ) {
	// This is the wire group IDs that will be returned:
	set< IDType > wireGroupIDs;

//TODO: Warn the user when a wire does not exist!
	if( wireList.find( wireID ) == wireList.end() ) return wireGroupIDs;

	// This wire is automatically included in the group:
	WIRE_PTR thisWire = wireList[wireID];
	wireGroupIDs.insert( wireID );

	// This is the set of wires left to be searched:
	set< IDType > searchList = wireGroupIDs;
	

	// Do a breadth-first search to add other wires into the list:

	// Loop through any wires that are left to be checked:
	while( !searchList.empty() ) {

		// Pop the first item off of the searching list:
		IDType thisWireID = *(searchList.begin());
		searchList.erase( thisWireID );

		// Loop through all the junctions in this wire.
		thisWire = wireList[thisWireID];
		set< IDType > wireJuncs = thisWire->getJunctions();
		set< IDType >::iterator thisJunc = wireJuncs.begin();
		while( thisJunc != wireJuncs.end() ) {
	
			// If the junction is marked as "enabled", then
			// add all of its wires into the group:
			if( getJunctionState( *thisJunc ) ) {
				set< IDType > juncWires = juncList[*thisJunc]->getWires();
				
				// Add all of the new wires into the "yet to check" list:
				set_difference( juncWires.begin(), juncWires.end(),
								wireGroupIDs.begin(), wireGroupIDs.end(),
								inserter( searchList, searchList.begin() ) );
	
				// Add the new wires to the "found attached" list:			
				wireGroupIDs.insert( juncWires.begin(), juncWires.end() );
			}
	
			thisJunc++;
		}

	} // while( !searchList.empty() )
	
	return wireGroupIDs;
}


// Returns a list of all wires that are connected to this
// wire via junctions:
set< WIRE_PTR > Circuit::getJunctionGroup( IDType wireID ) {
	// This is the wire group that will be returned:
	set< WIRE_PTR > wireGroup;

	// Get the IDs of connected wires:
	set< IDType > wireGroupIDs = getJunctionGroupIDs( wireID );

	// Convert all of the wire IDs into wire pointers:
	set< IDType >::iterator wireIDs = wireGroupIDs.begin();
	while( wireIDs != wireGroupIDs.end() ) {
		IDType theWireID = *wireIDs;
		WIRE_PTR theWirePtr = wireList[ theWireID ];
		wireGroup.insert( theWirePtr );
		wireIDs++;
	}

	return wireGroup;
}


// Convert the wire IDs to wire pointers:
set< WIRE_PTR > Circuit::getJunctionGroup( set< IDType >* wireGroupIDs ) {
	// This is the wire group that will be returned:
	set< WIRE_PTR > wireGroup;

	// Convert all of the wire IDs into wire pointers:
	set< IDType >::iterator wireIDs = wireGroupIDs->begin();
	while( wireIDs != wireGroupIDs->end() ) {
		IDType theWireID = *wireIDs;
		WIRE_PTR theWirePtr = wireList[ theWireID ];
		wireGroup.insert( theWirePtr );
		wireIDs++;
	}

	return wireGroup;
}






// ************* End of the visible interface of the circuit ****************


