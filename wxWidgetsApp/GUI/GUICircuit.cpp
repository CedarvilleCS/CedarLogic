/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   GUICircuit: Contains GUI circuit manipulation functions
*****************************************************************************/

#include "GUICircuit.h"
#include "MainApp.h"
#include "OscopeFrame.h"

DECLARE_APP(MainApp)
IMPLEMENT_DYNAMIC_CLASS(GUICircuit, wxDocument)

GUICircuit::GUICircuit() {
	nextGateID = nextWireID = 0;
	simulate = true;
	waitToSendMessage = true;
	panic = false;
	pausing = false;
	return;
}

GUICircuit::~GUICircuit() {

}

void GUICircuit::reInitializeLogicCircuit() {
	// do not wait to send messages to the core for reinit
	bool iswaiting = waitToSendMessage;
	waitToSendMessage = false;
	sendMessageToCore(klsMessage::Message(klsMessage::MT_REINITIALIZE));
	waitToSendMessage = iswaiting;
	hash_map< unsigned long, guiWire* >::iterator thisWire = wireList.begin();
	while( thisWire != wireList.end() ) {
		delete thisWire->second;
		thisWire++;
	}
	hash_map< unsigned long, guiGate* >::iterator thisGate = gateList.begin();
	while( thisGate != gateList.end() ) {
		delete thisGate->second;
		thisGate++;
	} 
	gateList.clear();
	wireList.clear();
	nextGateID = nextWireID = 0;
	waitToSendMessage = false;
	simulate = true;
}

guiGate* GUICircuit::createGate(string gateName, long id, bool noOscope) {
	string libName = wxGetApp().gateNameToLibrary[gateName];
	
	if (id == -1) id = getNextAvailableGateID();
	guiGate* newGate = NULL;
	LibraryGate gateDef = wxGetApp().libraries[libName][gateName];
	//wxGetApp().libraries[libName].getGate(gateName, gateDef);
	

	string ggt = gateDef.guiType;
	
	if (ggt == "REGISTER")
		newGate = (guiGate*)(new guiGateREGISTER());
	else if (ggt == "TO" || ggt == "FROM")
		newGate = (guiGate*)(new guiTO_FROM());
	else if (ggt == "LABEL")
		newGate = (guiGate*)(new guiLabel());
	else if (ggt == "LED")
		newGate = (guiGate*)(new guiGateLED());
	else if (ggt == "TOGGLE")
		newGate = (guiGate*)(new guiGateTOGGLE());
	else if (ggt == "KEYPAD")
		newGate = (guiGate*)(new guiGateKEYPAD());
	else if (ggt == "PULSE")
		newGate = (guiGate*)(new guiGatePULSE());
		
//*************************************************
//Edit by Joshua Lansford 12/25/2006
//I am creating a guiGate for the RAM so that
//the ram can have its own special pop-up window
	else if (ggt == "RAM"){
		newGate = (guiGate*)(new guiGateRAM());
	}

//End of edit
//*************************************************

//*************************************************
//Edit by Nathan Harro 01/13/2007
//This will recognize the Z80 type
//and call the correct guiGate method
	else if (ggt == "Z80"){
		newGate = (guiGate*)(new guiGateZ80());
	}

//End of edit
//*************************************************

//*******************************************
//Edit by Joshua Lansford 05/10/2007
//This is for the ADC as you might have guessed
	else if (ggt == "ADC"){
		newGate = (guiGate*)(new guiGateADC());
	}
//End of edit
//******************************************

	else
		newGate = new guiGate();

	newGate->setLibraryName( libName, gateName );

	for (unsigned int i = 0; i < gateDef.shape.size(); i++) {
		lgLine tempLine = gateDef.shape[i];
		newGate->insertLine(tempLine.x1, tempLine.y1, tempLine.x2, tempLine.y2);
	}
	for (unsigned int i = 0; i < gateDef.hotspots.size(); i++) {
		lgHotspot tempHS = gateDef.hotspots[i];
		newGate->insertHotspot(tempHS.x, tempHS.y, tempHS.name);
		if (tempHS.isInput) newGate->declareInput(tempHS.name);
		else newGate->declareOutput(tempHS.name);
	}
	map < string, string >::iterator paramWalk = gateDef.guiParams.begin();
	while (paramWalk != gateDef.guiParams.end()) {
		newGate->setGUIParam(paramWalk->first, paramWalk->second);
		paramWalk++;
	}
	paramWalk = gateDef.logicParams.begin();
	while (paramWalk != gateDef.logicParams.end()) {
		newGate->setLogicParam(paramWalk->first, paramWalk->second);
		paramWalk++;
	}
	newGate->calcBBox();
	gateList[id] = newGate;
	gateList[id]->setID(id);
	
	// Update the OScope with the new info:
	if(ggt == "TO" && !noOscope) {
		myOscope->UpdateMenu();
	}
	
	return newGate;
}

void GUICircuit::deleteGate(unsigned long gid, bool waitToUpdate) {
	
	//Declaration Of Variables
	bool updateMenu = false;
	
	if (gateList.find(gid) == gateList.end()) return;

	//Update Oscope
	if(!waitToUpdate && gateList[gid]->getGUIType() == "TO") {
		updateMenu = true;
	}
	
	delete gateList[gid];
	gateList.erase(gid);

	//Call Update Oscope
	if(updateMenu)
	{
		myOscope->UpdateMenu();
	}		
}

guiWire* GUICircuit::createWire(long wid) {
	if (wireList.find(wid) == wireList.end()) { // wire does not exist yet
		wireList[wid] = new guiWire();
	}
	return wireList[wid];
}

void GUICircuit::deleteWire(unsigned long wid) {
	hash_map < unsigned long, guiWire* >::iterator thisWire = wireList.find(wid);
	if (thisWire != wireList.end()) {
		delete thisWire->second;
		wireList.erase(thisWire);
	}
}

guiWire* GUICircuit::setWireConnection(long wid, long gid, string connection, bool openMode) {
	if (gateList.find(gid) == gateList.end()) return NULL; // error: gate not found
	createWire(wid); // do we need to init the wire first? if not then no effect.
	wireList[wid]->setID(wid);
	wireList[wid]->addConnection(gateList[gid], connection, openMode);
	gateList[gid]->addConnection(connection, wireList[wid]);
	return wireList[wid];
}

void GUICircuit::Render() {
	return;
}

void GUICircuit::parseMessage(klsMessage::Message message) {
	string temp, type;
	static bool shouldRender = false;
	switch (message.mType) {
		case klsMessage::MT_SET_WIRE_STATE: {
			// SET WIRE id STATE TO state
			shouldRender = true;
			klsMessage::Message_SET_WIRE_STATE* msgSetWireState = (klsMessage::Message_SET_WIRE_STATE*)(message.mStruct);
			setWireState(msgSetWireState->wireId, msgSetWireState->state);
			delete msgSetWireState;
			break;
		}
		case klsMessage::MT_SET_GATE_PARAM: {
			// SET GATE id PARAMETER name val
			shouldRender = true;
			klsMessage::Message_SET_GATE_PARAM* msgSetGateParam = (klsMessage::Message_SET_GATE_PARAM*)(message.mStruct);
			if (gateList.find(msgSetGateParam->gateId) != gateList.end()) gateList[msgSetGateParam->gateId]->setLogicParam(msgSetGateParam->paramName, msgSetGateParam->paramValue);
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
			//This spacific edit is so that the GUI thread will
			//hit the pause button
			if( msgSetGateParam->paramName == "PAUSE_SIM" ){
				pausing = true;
				panic = true;
			}
			//End of edit*************************************************
			delete msgSetGateParam;
			break;
		}
		case klsMessage::MT_DONESTEP: { // DONESTEP
			simulate = true;
			int logicTime = ((klsMessage::Message_DONESTEP*)(message.mStruct))->logicTime;
			// Panic if core isn't keeping up, keep a 3ms buffer...
			panic = (logicTime > lastTime+3) || panic;
			// Now we can send the waiting messages
			for (unsigned int i = 0; i < messageQueue.size(); i++) sendMessageToCore(messageQueue[i]);
			messageQueue.clear();
			// Only render at the end of a step and only if necessary
			if (shouldRender) gCanvas->Refresh();
			shouldRender = false;
			delete ((klsMessage::Message_DONESTEP*)(message.mStruct));
			break;
		}
		case klsMessage::MT_COMPLETE_INTERIM_STEP: {// COMPLETE INTERIM STEP - UPDATE OSCOPE
			myOscope->UpdateData();
			break;
		}
		default:
			break;
	}
}

void GUICircuit::sendMessageToCore(klsMessage::Message message) {
	wxMutexLocker lock(wxGetApp().mexMessages);

	if (waitToSendMessage) {
		
		if (simulate) {
			wxGetApp().dGUItoLOGIC.push_back(message);
		} else{
			messageQueue.push_back(message);
		}
	} else{
		wxGetApp().dGUItoLOGIC.push_back(message);
	}	
}

void GUICircuit::setWireState( long wid, long state ) {
	// If the wire doesn't exist, then don't set it's state!
	if( wireList.find(wid) == wireList.end() ) return;
	
	wireList[wid]->setState(state);
	gCanvas->Refresh();
	return;
}

void GUICircuit::printState() {
	wxGetApp().logfile << "print state" << endl << flush;
	hash_map < unsigned long, guiWire* >::iterator thisWire = wireList.begin();
	while (thisWire != wireList.end()) {
		wxGetApp().logfile << "wire " << thisWire->first << endl << flush;
		thisWire++;
	}
	hash_map < unsigned long, guiGate* >::iterator thisGate = gateList.begin();
	while (thisGate != gateList.end()) {
		wxGetApp().logfile << "gate " << thisGate->first << endl << flush;
		thisGate++;
	}
	
}
