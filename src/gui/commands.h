/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   commands: Implements a klsCommand object for each user interface command
*****************************************************************************/

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <map>
#include <hash_map>
#include <string>
#include <stack>
#include <vector>
#include "wx/cmdproc.h"
#include "gl_defs.h"
#include "wireSegment.h"
#include "GUICanvas.h"  // GateState, WireState

#include "wx/aui/auibook.h"

#include "../logic/logic_values.h"
using namespace std;

class GUICircuit;
class GUICanvas;

// Holds pointers to all of a gate's parameters
struct paramSet {
	map < string, string >* gParams;
	map < string, string >* lParams;
	paramSet( map < string, string >* g, map < string, string >* l ) { gParams = g; lParams = l; };
};

#include "command/klsCommand.h"
#include "command/cmdMoveGate.h"
#include "command/cmdMoveWire.h"

// cmdMoveSelection - move passed gates and wires
class cmdMoveSelection : public klsCommand {
protected:
	vector < unsigned long > gateList;
	vector < unsigned long > wireList;
	map < unsigned long, map < long, wireSegment > > oldSegMaps;
	map < unsigned long, map < long, wireSegment > > newSegMaps;		
	float startX, startY, endX, endY;
	int wireMove;
	vector < klsCommand* > proxconnects;
public:
	cmdMoveSelection( GUICircuit* gCircuit, vector < GateState > &preMove, vector < WireState > &preMoveWire, float startX, float startY, float endX, float endY );
	virtual ~cmdMoveSelection( void ) { return; };
	
	bool Do( void );
	bool Undo( void );
	
	vector < klsCommand* >* getConnections() { return &proxconnects; };
};

// cmdCreateGate - creates a gate on a given canvas at position (x,y)
class cmdCreateGate : public klsCommand {
protected:
	float x, y;
	string gateType;
	unsigned long gid;
	vector < klsCommand* > proxconnects;
	
public:
	cmdCreateGate( GUICanvas* gCanvas, GUICircuit* gCircuit, unsigned long gid, string gateType, float x, float y);
	cmdCreateGate( string def );
	virtual ~cmdCreateGate( void ) { return; };
	
	bool Do( void );
	bool Undo( void );
	virtual std::string toString() const override;
	void setPointers( GUICircuit* gCircuit, GUICanvas* gCanvas, hash_map < unsigned long, unsigned long > &gateids, hash_map < unsigned long, unsigned long > &wireids );
	
	vector < klsCommand* >* getConnections() { return &proxconnects; };
};


















// cmdConnectWire - connects a wire to a gate hotspot
class cmdConnectWire : public klsCommand {
public:
	cmdConnectWire(GUICircuit* gCircuit, IDType wid, IDType gid, const std::string &hotspot, bool noCalcShape = false);
	cmdConnectWire(const std::string &def);

	bool Do();
	bool Undo();

	bool validateBusLines() const;

	virtual std::string toString() const override;
	void setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas, hash_map < unsigned long, unsigned long > &gateids, hash_map < unsigned long, unsigned long > &wireids);
	IDType getGateId() const;
	const std::string & getHotspot() const;

	static void sendMessagesToConnect(GUICircuit *gCircuit, IDType wireId, IDType gateId, const std::string &hotspot, bool noCalcShape);
	static void sendMessagesToDisconnect(GUICircuit *gCircuit, IDType wireId, IDType gateId, const std::string &hotspot);

private:
	IDType gateId;
	IDType wireId;
	string hotspot;
	bool noCalcShape;
};









// cmdDisconnectWire - disconnects a wire from a gate hotspot
class cmdDisconnectWire : public klsCommand {
public:
	cmdDisconnectWire(GUICircuit* gCircuit, IDType wid, IDType gid, const string &hotspot, bool noCalcShape = false);

	bool Do();
	bool Undo();
	virtual std::string toString() const override;

private:
	IDType gateId;
	IDType wireId;
	std::string hotspot;
	bool noCalcShape;
};












// cmdCreateWire - creates a wire
class cmdCreateWire : public klsCommand {
public:
	cmdCreateWire(GUICanvas* gCanvas, GUICircuit* gCircuit, const std::vector<IDType> &wireIds, cmdConnectWire* conn1, cmdConnectWire* conn2);
	cmdCreateWire(const std::string &def);
	virtual ~cmdCreateWire();

	bool Do();
	bool Undo();

	bool validateBusLines() const;

	const std::vector<IDType> & getWireIds() const;

	virtual std::string toString() const override;
	void setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas, hash_map < unsigned long, unsigned long > &gateids, hash_map < unsigned long, unsigned long > &wireids);

private:
	std::vector<IDType> wireIds;
	cmdConnectWire* conn1;
	cmdConnectWire* conn2;
};



























// cmdDeleteWire - Deletes a wire
class cmdDeleteWire : public klsCommand {
public:
	cmdDeleteWire( GUICircuit* gCircuit, GUICanvas* gCanvas, IDType wireId);
	virtual ~cmdDeleteWire();
	
	bool Do();
	bool Undo();

private:
	std::vector<IDType> wireIds;
	stack < klsCommand* > cmdList;

};














// cmdDeleteGate - Deletes a gate
class cmdDeleteGate : public klsCommand {
protected:
	unsigned long gid;
	stack < klsCommand* > cmdList;
	string gateType;
	
public:
	cmdDeleteGate( GUICircuit* gCircuit, GUICanvas* gCanvas, unsigned long gid);
	virtual ~cmdDeleteGate( void );
	
	bool Do( void );
	bool Undo( void );
};

// cmdDeleteSelection - Deletes a selection of gates/wires
class cmdDeleteSelection : public klsCommand {
protected:
	vector < unsigned long > gates;
	vector < unsigned long > wires;
	stack < klsCommand* > cmdList;
	
public:
	cmdDeleteSelection( GUICircuit* gCircuit, GUICanvas* gCanvas, vector < unsigned long > &gates, vector < unsigned long > &wires);
	virtual ~cmdDeleteSelection( void );
	
	bool Do( void );
	bool Undo( void );
};

// cmdSetParams - Sets a gate's parameters
class cmdSetParams : public klsCommand {
protected:
	unsigned long gid;
	map < string, string > oldGUIParamList;
	map < string, string > newGUIParamList;
	map < string, string > oldLogicParamList;
	map < string, string > newLogicParamList;
	
public:
	cmdSetParams( GUICircuit* gCircuit, unsigned long gid, paramSet pSet, bool setMode = false );
	cmdSetParams( string def );
	virtual ~cmdSetParams( void ) { return; };
	
	bool Do( void );
	bool Undo( void );
	virtual std::string toString() const override;
	void setPointers( GUICircuit* gCircuit, GUICanvas* gCanvas, hash_map < unsigned long, unsigned long > &gateids, hash_map < unsigned long, unsigned long > &wireids );
};


// cmdPasteBlock - Paste's a block of gates/wires
class cmdPasteBlock : public klsCommand {
protected:
	vector < klsCommand* > cmdList;
	bool m_init;
public:
	cmdPasteBlock( vector < klsCommand* > &cmdList );
	virtual ~cmdPasteBlock( void ) { return; };
	
	bool Do( void );
	bool Undo( void );
	
	void addCommand ( klsCommand* cmd ) { cmdList.push_back( cmd ); };
};

// cmdWireSegDrag - Set's a wire's tree after dragging a segment
class cmdWireSegDrag : public klsCommand {
protected:
	map < long, wireSegment > oldSegMap;
	map < long, wireSegment > newSegMap;
	unsigned long wireID;
public:
	cmdWireSegDrag( GUICircuit* gCircuit, GUICanvas* gCanvas, unsigned long wireID );
	
	bool Do( void );
	bool Undo( void );
};

//JV - cmdDeleteTab - delete a tab from canvasBook
class cmdDeleteTab : public klsCommand {
protected:
	vector < unsigned long > gates;
	vector < unsigned long > wires;
	stack < klsCommand* > cmdList;
	wxAuiNotebook* canvasBook;
	vector< GUICanvas* >* canvases;
	unsigned long canvasID;

public:
	cmdDeleteTab(GUICircuit* gCircuit, GUICanvas* gCanvas, wxAuiNotebook* book, vector< GUICanvas* >* canvases, unsigned long ID);
	virtual ~cmdDeleteTab(void);

	bool Do(void);
	bool Undo(void);
};

//JV - cmdAddTab - add a new tab into canvasBook
class cmdAddTab : public klsCommand {
protected:
	wxAuiNotebook* canvasBook;
	vector< GUICanvas* >* canvases;

public:
	cmdAddTab(GUICircuit* gCircuit, wxAuiNotebook* book, vector< GUICanvas* >* canvases);
	virtual ~cmdAddTab(void);

	bool Do(void);
	bool Undo(void);
};

#endif /*COMMANDS_H_*/
