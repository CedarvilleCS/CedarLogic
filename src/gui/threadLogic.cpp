/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   threadLogic: Main execution point of logic thread
*****************************************************************************/

#include "threadLogic.h"
#include "MainApp.h"
#include <sstream>
#include "wx/timer.h"

#include "../logic/logic_defaults.h"
#include "../logic/logic_wire.h"
#include "../logic/logic_gate.h"
#include "../logic/logic_circuit.h"
#include "../logic/logic_event.h"
#include <string>


DECLARE_APP(MainApp)

threadLogic::threadLogic() : wxThread() {
	return;
}

void *threadLogic::Entry() {
	// This is the main function of the thread, so now we can init
	logicIDs = new map < IDType, IDType >;
	
	cir = new Circuit();
	while (!TestDestroy()) {
		checkMessages();
		wxThread::Sleep(1);
	}
	
	return NULL;
}

void threadLogic::checkMessages() {
	wxCriticalSectionLocker locker(wxGetApp().m_critsect);
	while (wxGetApp().mexMessages.TryLock() == wxMUTEX_BUSY) wxYield();
	while (wxGetApp().dGUItoLOGIC.size() > 0) {
		Message *message = wxGetApp().dGUItoLOGIC.front();
		wxGetApp().dGUItoLOGIC.pop_front();
		parseMessage(message);
		delete message;
	}
	wxGetApp().mexMessages.Unlock();
}	

void threadLogic::OnExit() {
	wxCriticalSectionLocker locker(wxGetApp().m_critsect);
	delete cir;
	delete logicIDs;
	// Tell the main thread we can exit now
	wxGetApp().m_semAllDone.Post();
}

bool threadLogic::parseMessage(Message *input) {

	string temp, type, pinID;
	long id, wireID;
	
	switch (input->type) {
	case MessageType::REINITIALIZE: {
		// REINITIALIZE LOGIC CIRCUIT
		delete cir;
		cir = new Circuit();
		logicIDs->clear();
		break;
	}
	case MessageType::CREATE_GATE: {

		auto msg = (Message_CREATE_GATE*)input;

		// tell logic core to create a gate id of type OR
		cir->newGate(msg->gateType, msg->gateId );

		break;
	}
	case MessageType::CREATE_WIRE: {
		
		auto msg = (Message_CREATE_WIRE*)input;

		id = msg->wireId;

		// tell logic core to create wire id
		(*logicIDs)[id] = cir->newWire( id );

		break;
	}
	case MessageType::DELETE_GATE: {
		
		auto msg = (Message_DELETE_GATE*)input;

		id = msg->gateId;

		cir->deleteGate(id);

		break;
	}
	case MessageType::DELETE_WIRE: {
		
		auto msg = (Message_DELETE_WIRE*)input;

		id = msg->wireId;

		cir->deleteWire((*logicIDs)[id]);

		break;
	}
	case MessageType::SET_GATE_INPUT: {

		auto msg = (Message_SET_GATE_INPUT*)input;

		// SET GATE ID id INPUT ID id TO DISCONNECT/wid
		id = msg->gateId;

		pinID = msg->inputId;

		// tell logic core to set gate id's input id to connect with wireID
		if (msg->disconnect) {
			cir->disconnectGateInput( id, pinID );
		} else {
			wireID = msg->wireId;
			if (logicIDs->find(wireID) == logicIDs->end()) {
				(*logicIDs)[wireID] = cir->connectGateInput( id, pinID, wireID );
			} else {
				cir->connectGateInput( id, pinID, (*logicIDs)[wireID] );
			}
		}
		break;
	}
	case MessageType::SET_GATE_INPUT_PARAM: {
		
		auto msg = (Message_SET_GATE_INPUT_PARAM*)input;
		
		// Now input holds the pValue
		// Send name "pName" and value "input" to gate for input pin settings
		cir->setGateInputParameter( msg->gateId, msg->inputId, msg->paramName, msg->paramValue );

		break;
	}
	case MessageType::SET_GATE_OUTPUT: {

		auto msg = (Message_SET_GATE_OUTPUT*)input;

		// SET GATE ID id OUTPUT ID id TO DISCONNECT/wid
		id = msg->gateId;

		pinID = msg->outputId;
		// tell logic core to set gate id's output id to connect with wireID
		if (msg->disconnect) {
			cir->disconnectGateOutput( id, pinID );
		} else {
			wireID = msg->wireId;
			if (logicIDs->find(wireID) == logicIDs->end()) {
				(*logicIDs)[wireID] = cir->connectGateOutput( id, pinID, wireID );
			} else {
				cir->connectGateOutput( id, pinID, (*logicIDs)[wireID] );
			}
		}
		break;
	}

	case MessageType::SET_GATE_OUTPUT_PARAM: {

		auto msg = (Message_SET_GATE_OUTPUT_PARAM*)input;

		// Now input holds the pValue
		// Send name "pName" and value "input" to gate for input pin settings
		cir->setGateOutputParameter( msg->gateId, msg->outputId, msg->paramName, msg->paramValue );

		break;
	}
	case MessageType::SET_GATE_PARAM: {

		auto msg = (Message_SET_GATE_PARAM*)input;
		
		cir->setGateParameter(msg->gateId, msg->paramName, msg->paramValue);

		break;
	}
	case MessageType::STEPSIM: {

		auto msg = (Message_STEPSIM*)input;

		// STEPSIM numSteps
		wxStopWatch simTime;
		int numSteps = msg->numSteps;
		bool pauseingSim = false;
		// Do that many steps and then notify GUI that we're done
		for (int i = 0; i < numSteps && !pauseingSim; i++) {
			ID_SET< IDType > changedWires;
			
			cir->step(&changedWires);
			ID_SET< IDType >::iterator cw = changedWires.begin();
			while (cw != changedWires.end()) {
				sendMessage(new Message_SET_WIRE_STATE(*cw, (int) cir->getWireState(*cw)));
				cw++;
			}
			
			// Update the possibly changed parameters:
			vector < changedParam > changedParams = cir->getParamUpdateList(); // Get the parameters that changed during this time step.
			cir->clearParamUpdateList(); // Let the circuit know that we are handling the updates!
			string paramVal;
			for( unsigned int i = 0; i < changedParams.size(); i++ ) {
				paramVal = cir->getGateParameter( changedParams[i].gateID, changedParams[i].paramName );
				if( paramVal.size() > 0 ) {
					sendMessage(new Message_SET_GATE_PARAM(changedParams[i].gateID, changedParams[i].paramName, paramVal));
				}
				
				//************************************************************
				//Edit by Joshua Lansford 11/24/06
				//the perpose of this edit is to allow logic gates to be able
				//to pause the simulation.  This is so that the 
				//Z_80LogicGate can 'single step' through T states and
				//instruction states by pauseing the simulation when it
				//compleates eather.
				//
				//The way that this is acomplished is that when ever any gate
				//signals that a property has changed, and the name of that
				//property is "PAUSE_SIM", then the core should bail out
				//and not finnish the requested number of steps.
				//The GUI will also see this property fly by and will toggle
				//the pause button.
				//
				//This spacific edit is so that the core will see this property
				//and will bail out.
				if( changedParams[i].paramName == "PAUSE_SIM" ){
					pauseingSim = true;
				}
				//End of Edit************************************************
			}
			// send interim done step message
			sendMessage(new Message_COMPLETE_INTERIM_STEP());
		}
		sendMessage(new Message_DONESTEP(simTime.Time()));
		break;
	}
	case MessageType::UPDATE_GATES: {
		
		// UPDATE GATE PARAMS		
		cir->stepOnlyGates();

		// Update the possibly changed parameters:
		vector < changedParam > changedParams = cir->getParamUpdateList(); // Get the parameters that changed
		cir->clearParamUpdateList(); // Let the circuit know that we are handling the updates!
		string paramVal;
		for( unsigned int i = 0; i < changedParams.size(); i++ ) {
			paramVal = cir->getGateParameter( changedParams[i].gateID, changedParams[i].paramName );
			if( paramVal.size() > 0 ) {
				sendMessage(new Message_SET_GATE_PARAM(changedParams[i].gateID, changedParams[i].paramName, paramVal));
			}
	
		}
		break;
	}
	default:
		break;
	}
	//End of edit**********************************
	
	return false;
}

void threadLogic::sendMessage(Message *message) {
	wxMutexLocker lock(wxGetApp().mexMessages);
	wxGetApp().dLOGICtoGUI.push_back(message);
}
