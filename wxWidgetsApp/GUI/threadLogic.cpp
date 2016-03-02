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

DECLARE_APP(MainApp)

/*string parseStringFromString(string& source) {
	unsigned int pointer = 0;
	while (pointer < source.size() && source[pointer] != ' ' && source[pointer] != '\n' && source[pointer] != '\t') pointer++;
	string result = source.substr(0,pointer);
	if (pointer != source.size()) source = source.substr(pointer+1,source.size()-pointer-1);
	return result;
}

int parseIntFromString(string& source) {
	unsigned int pointer = 0;
	int result = 0;
	while (pointer < source.size() && source[pointer] != ' ' && source[pointer] != '\n' && source[pointer] != '\t') pointer++;
	int factor = 0;
	for (int n = pointer-1; n >= 0; n--) {
		result += ((int)(source[n]-'0')) * ((int)pow((float)10,(int)factor++));
	}
	if (pointer != source.size()) source = source.substr(pointer+1,source.size()-pointer-1);
	return result;
}
 */
threadLogic::threadLogic() : wxThread() {
	return;
}

void *threadLogic::Entry() {
	// This is the main function of the thread, so now we can init
#ifndef _PRODUCTION_
	logfile.open("logiclog.log");
#endif
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
		parseMessage(wxGetApp().dGUItoLOGIC.front());
		wxGetApp().dGUItoLOGIC.pop_front();
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

bool threadLogic::parseMessage(klsMessage::Message input) {
	string temp, type, pinID;
	long id, wireID;
	switch (input.mType) {
	case klsMessage::MT_REINITIALIZE: {
		// REINITIALIZE LOGIC CIRCUIT
		delete cir;
		cir = new Circuit();
		logicIDs->clear();
		break;
	}
	case klsMessage::MT_CREATE_GATE: {
		// CREATE GATE TYPE type ID id
		klsMessage::Message_CREATE_GATE* msgCreateGate = (klsMessage::Message_CREATE_GATE*)(input.mStruct);

		// tell logic core to create a gate id of type OR
		cir->newGate( msgCreateGate->gateType, msgCreateGate->gateId );
		delete msgCreateGate;
		break;
	}
	case klsMessage::MT_CREATE_WIRE: {
		// CREATE WIRE ID id
		id = ((klsMessage::Message_CREATE_WIRE*)(input.mStruct))->wireId;
		// tell logic core to create wire id
		(*logicIDs)[id] = cir->newWire( id );
		(*logicIDs)[id] = id;
		delete ((klsMessage::Message_CREATE_WIRE*)(input.mStruct));
		break;
	}
	case klsMessage::MT_DELETE_GATE: {
		// DELETE GATE id
		id = ((klsMessage::Message_DELETE_GATE*)(input.mStruct))->gateId;
		cir->deleteGate(id);
		delete ((klsMessage::Message_DELETE_GATE*)(input.mStruct));
		break;
	}
	case klsMessage::MT_DELETE_WIRE: {
		// DELETE WIRE id
		id = ((klsMessage::Message_DELETE_WIRE*)(input.mStruct))->wireId;
		cir->deleteWire((*logicIDs)[id]);
		delete ((klsMessage::Message_DELETE_WIRE*)(input.mStruct));
		break;
	}
	case klsMessage::MT_SET_GATE_INPUT: {
		// SET GATE ID id INPUT ID id TO DISCONNECT/wid
		id = ((klsMessage::Message_SET_GATE_INPUT*)(input.mStruct))->gateId;
		pinID = ((klsMessage::Message_SET_GATE_INPUT*)(input.mStruct))->inputId;
		// tell logic core to set gate id's input id to connect with wireID
		if (((klsMessage::Message_SET_GATE_INPUT*)(input.mStruct))->disconnect) {
			cir->disconnectGateInput( id, pinID );
		} else {
			wireID = ((klsMessage::Message_SET_GATE_INPUT*)(input.mStruct))->wireId;
			if (logicIDs->find(wireID) == logicIDs->end()) {
				(*logicIDs)[wireID] = cir->connectGateInput( id, pinID, wireID );
			} else {
				cir->connectGateInput( id, pinID, (*logicIDs)[wireID] );
			}
		}
		delete ((klsMessage::Message_SET_GATE_INPUT*)(input.mStruct));
		break;
	}
	case klsMessage::MT_SET_GATE_INPUT_PARAM: {
		// SET GATE ID id INPUT ID id PARAM name value
		klsMessage::Message_SET_GATE_INPUT_PARAM* msgSetGateInputParam = (klsMessage::Message_SET_GATE_INPUT_PARAM*)(input.mStruct);
		// Now input holds the pValue
		// Send name "pName" and value "input" to gate for input pin settings
		cir->setGateInputParameter( msgSetGateInputParam->gateId, msgSetGateInputParam->inputId, msgSetGateInputParam->paramName, msgSetGateInputParam->paramValue );
		delete msgSetGateInputParam;
		break;
	}
	case klsMessage::MT_SET_GATE_OUTPUT: {
		// SET GATE ID id OUTPUT ID id TO DISCONNECT/wid
		id = ((klsMessage::Message_SET_GATE_OUTPUT*)(input.mStruct))->gateId;
		pinID = ((klsMessage::Message_SET_GATE_OUTPUT*)(input.mStruct))->outputId;
		// tell logic core to set gate id's output id to connect with wireID
		if (((klsMessage::Message_SET_GATE_OUTPUT*)(input.mStruct))->disconnect) {
			cir->disconnectGateOutput( id, pinID );
		} else {
			wireID = ((klsMessage::Message_SET_GATE_OUTPUT*)(input.mStruct))->wireId;
			if (logicIDs->find(wireID) == logicIDs->end()) {
				(*logicIDs)[wireID] = cir->connectGateOutput( id, pinID, wireID );
			} else {
				cir->connectGateOutput( id, pinID, (*logicIDs)[wireID] );
			}
		}
		delete ((klsMessage::Message_SET_GATE_OUTPUT*)(input.mStruct));
		break;
	}

	case klsMessage::MT_SET_GATE_OUTPUT_PARAM: {
		// SET GATE ID id OUTPUT ID id PARAM name value
		klsMessage::Message_SET_GATE_OUTPUT_PARAM* msgSetGateOutputParam = (klsMessage::Message_SET_GATE_OUTPUT_PARAM*)(input.mStruct);
		// Now input holds the pValue
		// Send name "pName" and value "input" to gate for input pin settings
		cir->setGateOutputParameter( msgSetGateOutputParam->gateId, msgSetGateOutputParam->outputId, msgSetGateOutputParam->paramName, msgSetGateOutputParam->paramValue );
		delete msgSetGateOutputParam;
		break;
	}
	case klsMessage::MT_SET_GATE_PARAM: {
		// SET GATE ID id PARAMETER paramname paramval
		klsMessage::Message_SET_GATE_PARAM* msgSetGateParam = (klsMessage::Message_SET_GATE_PARAM*)(input.mStruct);
		cir->setGateParameter(msgSetGateParam->gateId, msgSetGateParam->paramName, msgSetGateParam->paramValue);
		delete msgSetGateParam;
		break;
	}
	case klsMessage::MT_STEPSIM: {
		// STEPSIM numSteps
		wxStopWatch simTime;
		int numSteps = ((klsMessage::Message_STEPSIM*)(input.mStruct))->numSteps;
		bool pauseingSim = false;
		// Do that many steps and then notify GUI that we're done
		for (int i = 0; i < numSteps && !pauseingSim; i++) {
			ID_SET< IDType > changedWires;
			
			cir->step(&changedWires);
			ID_SET< IDType >::iterator cw = changedWires.begin();
			while (cw != changedWires.end()) {
				sendMessage(klsMessage::Message(klsMessage::MT_SET_WIRE_STATE, new klsMessage::Message_SET_WIRE_STATE(*cw, (int) cir->getWireState(*cw))));
				cw++;
			}
			
			// Update the possibly changed parameters:
			vector < changedParam > changedParams = cir->getParamUpdateList(); // Get the parameters that changed during this time step.
			cir->clearParamUpdateList(); // Let the circuit know that we are handling the updates!
			string paramVal;
			for( unsigned int i = 0; i < changedParams.size(); i++ ) {
				paramVal = cir->getGateParameter( changedParams[i].gateID, changedParams[i].paramName );
				if( paramVal.size() > 0 ) {
					sendMessage(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(changedParams[i].gateID, changedParams[i].paramName, paramVal)));
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
			sendMessage(klsMessage::Message(klsMessage::MT_COMPLETE_INTERIM_STEP));
		}
		sendMessage(klsMessage::Message(klsMessage::MT_DONESTEP, new klsMessage::Message_DONESTEP(simTime.Time())));
		delete ((klsMessage::Message_STEPSIM*)(input.mStruct));
		break;
	}
	case klsMessage::MT_UPDATE_GATES: {
		//*********************************************
		//Edit by Joshua Lansford 3/27/07
		//Purpose of edit:
		//  This is a new command that the gui can
		//  send the core. "UPDATE GATES".  It makes
		//  it so that gates can respond with paramiter
		//  changes without steping the simulation
		//  forward by a step
		
		// UPDATE GATE PARAMS		
		cir->stepOnlyGates();

		// Update the possibly changed parameters:
		vector < changedParam > changedParams = cir->getParamUpdateList(); // Get the parameters that changed
		cir->clearParamUpdateList(); // Let the circuit know that we are handling the updates!
		string paramVal;
		for( unsigned int i = 0; i < changedParams.size(); i++ ) {
			paramVal = cir->getGateParameter( changedParams[i].gateID, changedParams[i].paramName );
			if( paramVal.size() > 0 ) {
				sendMessage(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(changedParams[i].gateID, changedParams[i].paramName, paramVal)));
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

void threadLogic::sendMessage(klsMessage::Message message) {
	wxMutexLocker lock(wxGetApp().mexMessages);
	wxGetApp().dLOGICtoGUI.push_back(message);
}
