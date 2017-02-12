/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   GUICircuit: Contains GUI circuit manipulation functions
*****************************************************************************/

#pragma once

#include <map>
#include <stack>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include "wx/docview.h"
#include "../gl_wrapper.h"
#include "../../logic/logic_values.h"
using namespace std;

class GUICanvas;
class GUICircuit;
class OscopeFrame;
class guiGate;
class guiWire;
class Message;

using guiGateMap = std::unordered_map<IDType, guiGate *>;
using guiWireMap = std::unordered_map<IDType, guiWire *>;


class GUICircuit : public wxDocument {
public:
    GUICircuit();


	void reInitializeLogicCircuit();

	// Create a gate and add it to the circuit.
	// An existing gateId can be used.
	// Existing gateId's are not checked for conflicts.
	// If no gateId is specified, a valid one will be generated.
	guiGate * createGate(const std::string &gateType, IDType gateId = ID_NONE, bool noOscope = false);

	// Create/retrieve a wire.
	// Every wire is a bus. Many just have 1 busline.
	// A wireId is specified for each busline in the wire.
	guiWire* createWire(const std::vector<IDType> &wireIds);

	// Destroy a gate, remove it from the GUICircuit.
	void deleteGate(IDType gid, bool waitToUpdate = false);

	// Destroy a wire, remove it from the GUICircuit.
	void deleteWire(IDType wid);

	// Connect all bus-lines of a wire to a gate's hotspot.
	// 'hasShape' indicates that the wire does not need to be routed to the gate.
	void connectWire(IDType wireId, IDType gateId, const std::string &hotspotName, bool hasShape);
	
	// Set a single busline's state.
	// wireId is not well named. It refers to a busline in a wire.
	void setWireState(IDType wireId, StateType state);
	
	guiGateMap * getGates();

	guiWireMap * getWires();
	
	IDType getNextAvailableGateID();

	IDType getNextAvailableWireID();

	void sendMessageToCore(Message *message);

	void parseMessage(Message *message);
	
	void setSimulate(bool state);

	bool getSimulate();
	
	OscopeFrame* getOscope();

	void setOscope(OscopeFrame* of);
	
	void setCurrentCanvas(GUICanvas* gc);
	
	bool panic;
	bool pausing;
	int lastTimeMod;
	int lastNumSteps;
	int lastTime;
	
private:
	guiGateMap gateList;
	guiWireMap wireList;

	guiWireMap buslineToWire;

	IDType nextGateID;
	IDType nextWireID;
	
	OscopeFrame* myOscope;
	GUICanvas* gCanvas;

	bool movingGate;
	bool drawingWire;
	bool simulate;			// Simulation state
	bool waitToSendMessage; // If false, then message is sent immediately
	
	
    vector < Message *> messageQueue;

	
};