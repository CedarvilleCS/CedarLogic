/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   GUICircuit: Contains GUI circuit manipulation functions
*****************************************************************************/

#include "GUICircuit.h"
#include "../MainApp.h"
#include "../widget/GUICanvas.h"
#include "../OscopeFrame.h"
#include "../thread/Message.h"
#include "guiGates.h"
#include "guiWire.h"

DECLARE_APP(MainApp)

GUICircuit::GUICircuit() {
	nextGateID = nextWireID = 0;
	simulate = true;
	waitToSendMessage = true;
	panic = false;
	pausing = false;
	commandProcessor = nullptr;
}

GUICircuit::~GUICircuit() {
	
	if (commandProcessor != nullptr) {
		delete commandProcessor;
	}
}

void GUICircuit::reInitializeLogicCircuit() {

	// do not wait to send messages to the core for reinit
	bool iswaiting = waitToSendMessage;
	waitToSendMessage = false;
	sendMessageToCore(new Message_REINITIALIZE());
	waitToSendMessage = iswaiting;

	guiWireMap::iterator thisWire = wireList.begin();
	while( thisWire != wireList.end() ) {
		if (thisWire->second != nullptr) {
			delete thisWire->second;
		}
		thisWire++;
	}

	guiGateMap::iterator thisGate = gateList.begin();
	while( thisGate != gateList.end() ) {
		delete thisGate->second;
		thisGate++;
	} 

	gateList.clear();
	wireList.clear();
	buslineToWire.clear();
	nextGateID = nextWireID = 0;
	waitToSendMessage = false;
	simulate = true;
}

guiGate * GUICircuit::createGate(const std::string &gateName, IDType id, bool noOscope) {

	string libName = wxGetApp().gateNameToLibrary[gateName];
	
	if (id == ID_NONE) {
		id = getNextAvailableGateID();
	}
	
	LibraryGate gateDef = wxGetApp().libraries[libName][gateName];

	string ggt = gateDef.guiType;
	
	guiGate* newGate = NULL;

	if (ggt == "REGISTER") {
		newGate = new guiGateREGISTER();
	}
	else if (ggt == "TO" || ggt == "FROM") {
		newGate = new guiTO_FROM();
	}
	else if (ggt == "LABEL") {
		newGate = new guiLabel();
	}
	else if (ggt == "LED") {
		newGate = new guiGateLED();
	}
	else if (ggt == "TOGGLE") {
		newGate = new guiGateTOGGLE();
	}
	else if (ggt == "KEYPAD") {
		newGate = new guiGateKEYPAD();
	}
	else if (ggt == "PULSE") {
		newGate = new guiGatePULSE();
	}
	else if (ggt == "RAM") {
		newGate = new guiGateRAM();
	}
	else if (ggt == "Z80") {
		newGate = new guiGateZ80();
	}
	else if (ggt == "ADC") {
		newGate = new guiGateADC();
	}
	else {
		newGate = new guiGate();
	}

	newGate->setLibraryName( libName, gateName );

	for (unsigned int i = 0; i < gateDef.shape.size(); i++) {
		lgLine tempLine = gateDef.shape[i];
		newGate->insertLine(tempLine.x1, tempLine.y1, tempLine.x2, tempLine.y2);
	}
	for (unsigned int i = 0; i < gateDef.hotspots.size(); i++) {
		lgHotspot tempHS = gateDef.hotspots[i];
		newGate->insertHotspot(tempHS.x, tempHS.y, tempHS.name, tempHS.busLines);
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

guiWire* GUICircuit::createWire(const std::vector<IDType> &wireIds) {
	if (wireList.find(wireIds[0]) == wireList.end()) { // wire does not exist yet

		guiWire *wire = new guiWire();

		// Make sure that each used wireId has a spot in the wireList.
		// This lets getNextAvailableWireId() give an unused id.
		// Also add all buslines to the busline map.
		for (IDType id : wireIds) {
			wireList[id] = nullptr;
			buslineToWire[id] = wire;
		}

		wireList[wireIds[0]] = wire;
		wireList[wireIds[0]]->setIDs(wireIds);
	}
	return wireList[wireIds[0]];
}

void GUICircuit::deleteGate(IDType gid, bool waitToUpdate) {

	//Declaration Of Variables
	bool updateMenu = false;

	if (gateList.find(gid) == gateList.end()) return;

	//Update Oscope
	if (!waitToUpdate && gateList[gid]->getGUIType() == "TO") {
		updateMenu = true;
	}

	delete gateList[gid];
	gateList.erase(gid);

	//Call Update Oscope
	if (updateMenu) {
		myOscope->UpdateMenu();
	}
}

void GUICircuit::deleteWire(IDType wireId) {

	if (wireList.find(wireId) == wireList.end()) return;

	guiWire *wire = wireList.at(wireId);

	// Release ID's owned by the wire.
	for (int busLineId : wire->getIDs()) {
		wireList.erase(busLineId);
		buslineToWire.erase(busLineId);
	}

	delete wire;
}

void GUICircuit::connectWire(IDType wireId, IDType gateId, const std::string &hotspotName, bool hasShape) {
	
	guiWire *wire = wireList.at(wireId);
	guiGate *gate = gateList.at(gateId);

	wire->addConnection(gate, hotspotName, hasShape);
	gate->addConnection(hotspotName, wire);
}

void GUICircuit::setWireState(IDType wid, StateType state) {
	buslineToWire.at(wid)->setSubState(wid, state);
}

guiGateMap * GUICircuit::getGates() {
	return &gateList;
}

guiWireMap * GUICircuit::getWires() {
	return &wireList;
}

IDType GUICircuit::getNextAvailableGateID() {
	nextGateID++;

	while (gateList.find(nextGateID) != gateList.end())
		nextGateID++;
	
	return nextGateID;
}

IDType GUICircuit::getNextAvailableWireID() {
	nextWireID++;
	
	while (wireList.find(nextWireID) != wireList.end())
		nextWireID++;
	
	return nextWireID;
}

void GUICircuit::sendMessageToCore(Message *message) {

	wxMutexLocker lock(wxGetApp().mexMessages);

	if (waitToSendMessage) {

		if (simulate) {
			wxGetApp().dGUItoLOGIC.push_back(message);
		}
		else {
			messageQueue.push_back(message);
		}
	}
	else {
		wxGetApp().dGUItoLOGIC.push_back(message);
	}
}

void GUICircuit::parseMessage(Message *message) {

	string temp, type;
	static bool shouldRender = false;
	switch (message->type) {
		case MessageType::SET_WIRE_STATE: {

			shouldRender = true;

			auto msg = (Message_SET_WIRE_STATE*)message;

			setWireState(msg->wireId, msg->state);

			break;
		}
		case MessageType::SET_GATE_PARAM: {

			shouldRender = true;

			auto msg = (Message_SET_GATE_PARAM*)message;

			if (gateList.find(msg->gateId) != gateList.end()) {
				gateList[msg->gateId]->setLogicParam(msg->paramName, msg->paramValue);
			}

			
			if(msg->paramName == "PAUSE_SIM" ){
				pausing = true;
				panic = true;
			}
			break;
		}
		case MessageType::DONESTEP: { // DONESTEP

			auto msg = (Message_DONESTEP*)message;

			simulate = true;

			int logicTime = msg->logicTime;

			// Panic if core isn't keeping up, keep a 3ms buffer...
			panic = (logicTime > lastTime+3) || panic;

			// Now we can send the waiting messages
			for (unsigned int i = 0; i < messageQueue.size(); i++) {
				sendMessageToCore(messageQueue[i]);
			}

			messageQueue.clear();

			// Only render at the end of a step and only if necessary
			if (shouldRender) {
				gCanvas->Refresh();
				shouldRender = false;
			}
			break;
		}
		case MessageType::COMPLETE_INTERIM_STEP: {// COMPLETE INTERIM STEP - UPDATE OSCOPE
			myOscope->UpdateData();
			/* CB: Updated should render to fix chaotic display issues due to oscope*/
			shouldRender = true;
			break;
		}
		default:
			break;
	}
}

void GUICircuit::setSimulate(bool state) {
	simulate = state;
}

bool GUICircuit::getSimulate() {
	return simulate;
}

OscopeFrame* GUICircuit::getOscope() {
	return myOscope;
}

void GUICircuit::setOscope(OscopeFrame* of) {
	myOscope = of;
}

void GUICircuit::setCurrentCanvas(GUICanvas* gc) {
	gCanvas = gc;
}

void GUICircuit::setCommandProcessor(wxCommandProcessor *p) {
	commandProcessor = p;
}

wxCommandProcessor * GUICircuit::getCommandProcessor() const {
	return commandProcessor;
}