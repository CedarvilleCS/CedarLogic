/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   GUICircuit: Contains GUI circuit manipulation functions
*****************************************************************************/

#ifndef GUICIRCUIT_H_
#define GUICIRCUIT_H_

class GUICanvas;
class GUICircuit;

class OscopeFrame;
class guiGate;

#include "MainApp.h"
#include "guiGate.h"
#include "GUICanvas.h"
//#include "OscopeFrame.h"
class OscopeFrame;

#include "gl_wrapper.h"

#include <map>
#include <stack>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

class GUICircuit : public wxDocument
{
    DECLARE_DYNAMIC_CLASS(GUICircuit)
	
public:
    GUICircuit();

    virtual ~GUICircuit();

	// Reinit circuit
	void reInitializeLogicCircuit();
	// Renders the complete circuit on the current buffer
    void Render();
	// Creates a new gate with type, position, and id; returns used id
	guiGate* createGate(string gt, long id = -1, bool noOscope = false);

	// Creates a new wire/bus. 'wireIds' are each of the bus-line's ids.
	// Returns pointer to existing wire if called for ids that already exist.
	guiWire* createWire(const std::vector<IDType> &wireIds);

	// Sets a named input/output of a gate to be connected; returns pointer to wire
	guiWire* setWireConnection(const std::vector<IDType> &wireIds, long gid, string connection, bool openMode = false);

	// Sets a wire's state
	void setWireState(long wid, long state);
	// Delete components and sync the core
	void deleteWire(unsigned long wid);
	void deleteGate(unsigned long gid, bool waitToUpdate = false);
	
	// Maps of gates and wires to their IDs
	hash_map< unsigned long, guiGate* >* getGates() { return &gateList; };
	hash_map< unsigned long, guiWire* >* getWires() { return &wireList; };
	
	unsigned long getNextAvailableGateID() { nextGateID++; while (gateList.find(nextGateID) != gateList.end()) nextGateID++; return nextGateID; };
	unsigned long getNextAvailableWireID() { nextWireID++; while (wireList.find(nextWireID) != wireList.end()) nextWireID++; return nextWireID; };

	void sendMessageToCore(klsMessage::Message message);
	void parseMessage(klsMessage::Message message);
	
	void setSimulate(bool state) { simulate = state; };
	bool getSimulate() { return simulate; };
	
	void printState();
	
	OscopeFrame* getOscope() { return myOscope; };
	void setOscope(OscopeFrame* of) { myOscope = of; };
	
	void setCurrentCanvas(GUICanvas* gc) { gCanvas = gc; };
	
	bool panic;
	bool pausing;
	int lastTimeMod;
	int lastNumSteps;
	int lastTime;
	
private:
	hash_map< unsigned long, guiGate* > gateList;
	hash_map< unsigned long, guiWire* > wireList;

	hash_map<IDType, guiWire *> buslineToWire;

	unsigned long nextGateID;
	unsigned long nextWireID;
	
	OscopeFrame* myOscope;
	GUICanvas* gCanvas;

	bool   m_init;
    GLuint m_gllist;
	double lastDragX;
	double lastDragY;
    static unsigned long  m_secbase;
    static int            m_TimeInitialized;
    static unsigned long  m_xsynct;
    static unsigned long  m_gsynct;

	bool movingGate;
	bool drawingWire;
	bool simulate;			// Simulation state
	bool waitToSendMessage; // If false, then message is sent immediately
		
    long           m_Key;
    unsigned long  m_StartTime;
    unsigned long  m_LastTime;
    unsigned long  m_LastRedraw;
 
    vector < klsMessage::Message > messageQueue;
};

#endif /*GUICIRCUIT_H*/
