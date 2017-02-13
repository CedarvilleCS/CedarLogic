/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   GUICanvas: Contains rendering and input functions for a page
*****************************************************************************/

#ifndef GUICANVAS_H_
#define GUICANVAS_H_

#include <map>
#include <unordered_map>
#include <vector>
#include <string>
using namespace std;

#include "klsGLCanvas.h"
#include "../circuit/wireSegment.h"
#include "../klsCollisionChecker.h"
#include "../../gui/thread/threadLogic.h"

class klsCommand;
class guiWire;
class cmdPasteBlock;
class GUICircuit;

// Struct GateState
//		stores the position and id of a gate so we know where we moved from
struct GateState {
	GateState( unsigned int nID, float nX, float nY, bool nSel ) : id(nID), x(nX), y(nY), selected(nSel) {}
	unsigned int id;
	float x;
	float y;
	bool selected;
};

// Struct WireState
//		stores the relative position (to itself) of a wire so we know where we moved from
struct WireState {
	WireState( unsigned int nID, GLPoint2f nPoint, map < long, wireSegment > nTree ) : 
		id(nID), point(nPoint), oldWireTree(nTree) {}
	unsigned int id;
	GLPoint2f point;
	map < long, wireSegment > oldWireTree;
};

// Struct ConnectionSource
//		stores the source of a drag connect operation
struct ConnectionSource {
	bool isGate;
	unsigned long objectID;
	string connection;
	
	ConnectionSource( bool ig, unsigned long id, string conn ) : isGate(ig), objectID(id), connection(conn) {};
	ConnectionSource() {};
};

#define MAX_UNDO_STATES 256

#define MIN_ZOOM 1.0/120.0
#define MAX_ZOOM 1.0*1.0
#define DEFAULT_ZOOM 1.0/10.0
// The amount of zooming done per step (in %).
#define ZOOM_STEP 0.75

#define MIN_PAN -1.0e10
#define MAX_PAN 1.0e10

// The amount of panning done per step for keypress (in pixels).
#define PAN_STEP 30

// The amount of panning done per step for autoscroll (in pixels).
#define SCROLL_STEP 30
#define SCROLL_TIMER_RATE 30
#define SCROLL_TIMER_ID 1

// The amount of area that will react as a hotspot
#define HOTSPOT_SCREEN_RADIUS 3.0
#define HOTSPOT_SCREEN_DELTA  5.0
#define WIRE_HOVER_SCREEN_DELTA 5.0
#define MOUSE_HOVER_DELTA 4.5

#define GRID_INTENSITY 0.08
#define MIN_GRID_SCREEN_SPACING 13

#define ZOOM_ALL_MARGIN 0.25

// DragStates
enum DragState {
	DRAG_NONE = 0,
	DRAG_CONNECT,
	DRAG_SELECT,
	DRAG_SELECTION,
	DRAG_NEWGATE,
	DRAG_WIRESEG
};

// Class GUICanvas, inherits from klsGLCanvas for basic scroll/zoom/viewport functionality
//		all event handling is passed to this subclass in GL coordinates.
//		GUICanvas handles all gate and wire manipulation.
class GUICanvas: public klsGLCanvas
{
public:
    GUICanvas( wxWindow *parent, GUICircuit* gCircuit, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = "GUICanvas" );

	virtual ~GUICanvas();

	// Event handlers
	void OnMouseDown(wxMouseEvent& event);
    void mouseLeftDown(wxMouseEvent& event);
    void mouseRightDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseMove( GLdouble glX, GLdouble glY, bool ShiftDown, bool CtrlDown );
    void OnMouseEnter(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
	void OnSize();
	void OnRender(bool noColor = false);

	// Return the gate and wire lists for this page
	unordered_map < unsigned long, guiGate* >* getGateList();
	unordered_map < unsigned long, guiWire* >* getWireList();

	
	// Clears the page
	void clearCircuit();

	// Insert and remove gates and wires from this canvas
	void insertGate(unsigned long, guiGate*, float, float);
	void removeGate(unsigned long);
	void insertWire(guiWire*);
	void removeWire(unsigned long);
	
	// Deletes the currently selected gates and wires
	void deleteSelection();

	// Remove the selection flag from all gates or wires on the canvas
	void unselectAllGates();
	void unselectAllWires();
	
	// Handle copy and paste for this canvas
	void copyBlockToClipboard();
	void pasteBlockFromClipboard();

	// Pointer to the main application graphic circuit
	GUICircuit* getCircuit();

	// Tell the canvas which minimap it should use; sets the minimaps pointers and lists
	void setMinimap(klsMiniMap* minimap);
	
	// Zoom the canvas to fit all items within it:
	void setZoomAll();

	// Zoom the canvas in or out:
	void zoomIn();
	void zoomOut();

	void printLists();

	// Update the collision checker and refresh
	void Update();

	// Create a command to connect a wire to a gate.
	klsCommand * createGateWireConnectionCommand(IDType gateId, const string &hotspot, IDType wireId);

	// Create a command to connect a gate to a gate.
	klsCommand * createGateConnectionCommand(IDType gate1Id, const string &hotspot1, IDType gate2Id, const string &hotspot2);

private:

	// Contains all collision information for the page
	klsCollisionChecker collisionChecker;
	klsCollisionObject* mouse;
	klsCollisionObject* snapMouse;
	klsCollisionObject* dragselectbox;
	
	// Pointer to the main application graphic circuit
	GUICircuit* gCircuit;

	// Maps of the gates and wires on this page
	unordered_map< unsigned long, guiGate* > gateList;
	unordered_map< unsigned long, guiWire* > wireList;
	vector < unsigned long > selectedGates;
	vector < unsigned long > selectedWires;

	// Hotspot and wire highlights:
	unsigned long hotspotGate; // The gate in which a hotspot is highlighted.
	string hotspotHighlight; // The hotSpot to highlight when rendering. If == "", then none are highlighted.
	vector < GLPoint2f > potentialConnectionHotspots; // Points to highlight for connection when moving a gate.
	bool drawWireHover; // Whether or not to draw a wire hover X value.
	unsigned long wireHoverID;
	ConnectionSource currentConnectionSource;
	
	bool isWithinPaste; // If we are in paste then drag_selection is enabled until drop
	DragState currentDragState;
	cmdPasteBlock* pasteCommand; // Hold the paste command until the block is dropped

	// If we are in DRAG_SELECTION then we should hold gates' original position and flag
	//		that we want to save the move as a command in the undo stack
	vector < GateState > preMove;
	vector < WireState > preMoveWire;
	bool saveMove;

	// Pointer to the new gate in DRAG_NEWGATE mode until the gate is dropped
	guiGate* newDragGate;
	
};

#endif /*TESTGLCANVAS_H_*/
