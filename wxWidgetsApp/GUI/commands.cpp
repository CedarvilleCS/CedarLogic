/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   commands: Implements a klsCommand object for each user interface command
*****************************************************************************/

#include "commands.h"
#include "OscopeFrame.h"
#include <sstream>

#include "MainApp.h"
#include "GUICircuit.h"
#include "GUICanvas.h"
#include "guiGate.h"
#include "guiWire.h"
#include <string>
#include <stack>
#include <sstream>

DECLARE_APP(MainApp)

// CMDMOVEGATE

cmdMoveGate::cmdMoveGate( GUICircuit* gCircuit, unsigned long gid, float startX, float startY, float endX, float endY, bool uW ) : klsCommand(true, "Move Gate") {
	this->gCircuit = gCircuit;
	this->gid = gid;
	this->startX = startX;
	this->startY = startY;
	this->endX = endX;
	this->endY = endY;
	this->noUpdateWires = uW;
}

bool cmdMoveGate::Do() {
	if ( (gCircuit->getGates())->find(gid) == (gCircuit->getGates())->end() ) return false; // error, gate not found
	(*(gCircuit->getGates()))[gid]->setGLcoords(endX, endY, noUpdateWires);
	return true;
}

bool cmdMoveGate::Undo() {
	if ( (gCircuit->getGates())->find(gid) == (gCircuit->getGates())->end() ) return false; // error, gate not found
	(*(gCircuit->getGates()))[gid]->setGLcoords(startX, startY, noUpdateWires);
	return true;
}

string cmdMoveGate::toString() const {
	if ( (gCircuit->getGates())->find(gid) == (gCircuit->getGates())->end() ) return ""; // error, gate not found
	ostringstream oss;
	oss << "movegate " << gid << " " << startX << " " << startY << " " << endX << " " << endY;
	return oss.str();	
}

// CMDMOVEWIRE

cmdMoveWire::cmdMoveWire( GUICircuit* gCircuit, unsigned long wid, map < long, wireSegment > oldList, map < long, wireSegment > newList ) : klsCommand(true, "Move Wire") {
	this->gCircuit = gCircuit;
	this->wid = wid;
	oldSegList = oldList;
	newSegList = newList;
	delta = GLPoint2f(0,0);
}

cmdMoveWire::cmdMoveWire( GUICircuit* gCircuit, unsigned long wid, map < long, wireSegment > oldList, GLPoint2f delta ) : klsCommand(true, "Move Wire") {
	this->gCircuit = gCircuit;
	this->wid = wid;
	oldSegList = oldList;
	this->delta = delta;
}

cmdMoveWire::cmdMoveWire( string def ) : klsCommand( true, "Move Wire") {
	istringstream iss(def);
	// wire looks like "movewire wid" then series of segments
	string temp; char dump;
	iss >> temp >> wid >> temp;
	bool doneFirstSeg = false;
	long firstSegID = 0;
	// seg looks like "segment id bx,by,ex,ey {connection gid name} {isect key id}"
	while (temp == "vsegment" || temp == "hsegment") {
		bool isVertical = (temp == "vsegment");
		int segID;
		GLPoint2f begin, end;
		iss >> segID >> begin.x >> dump >> begin.y >> dump >> end.x >> dump >> end.y;
		newSegList[segID] = wireSegment( begin, end, isVertical, segID );
		if (!doneFirstSeg) {
			firstSegID = segID;
		}		
		iss >> temp;
		while (temp == "connection") {
			int gid; string name; wireConnection wc;
			iss >> gid >> name;
			wc.gid = gid; wc.connection = name;
			//wc.cGate = (*(gCircuit->getGates()))[gid];
			newSegList[segID].connections.push_back( wc );
			iss >> temp;
		}
		while (temp == "isect") {
			GLfloat key; long sid;
			iss >> key >> sid;
			newSegList[segID].intersects[key].push_back( sid );
			iss >> temp;
		}
		doneFirstSeg = true;
	}
	oldSegList = newSegList;
	delta = GLPoint2f(0,0);
}

bool cmdMoveWire::Do() {
	if ( (gCircuit->getWires())->find(wid) == (gCircuit->getWires())->end() ) return false; // error, wire not found
	if ( delta.x != 0 || delta.y != 0 ) {
		map < long, wireSegment >::iterator segWalk = oldSegList.begin();
		while (segWalk != oldSegList.end()) {
			for (unsigned int i = 0; i < (segWalk->second).connections.size(); i++) {
				(segWalk->second).connections[i].cGate = (*(gCircuit->getGates()))[(segWalk->second).connections[i].gid];
			}
			(segWalk->second).begin.x += delta.x; (segWalk->second).begin.y += delta.y;
			(segWalk->second).end.x += delta.x; (segWalk->second).end.y += delta.y;
			segWalk++;
		}
		(*(gCircuit->getWires()))[wid]->setSegmentMap(oldSegList);
	}
	else {
		map < long, wireSegment >::iterator segWalk = newSegList.begin();
		while (segWalk != newSegList.end()) {
			for (unsigned int i = 0; i < (segWalk->second).connections.size(); i++) {
				(segWalk->second).connections[i].cGate = (*(gCircuit->getGates()))[(segWalk->second).connections[i].gid];
			}
			segWalk++;
		}
		(*(gCircuit->getWires()))[wid]->setSegmentMap(newSegList);
	}
	(*(gCircuit->getWires()))[wid]->endSegDrag();
	return true;
}

bool cmdMoveWire::Undo() {
	if ( (gCircuit->getWires())->find(wid) == (gCircuit->getWires())->end() ) return false; // error, wire not found
	map < long, wireSegment >::iterator segWalk = oldSegList.begin();
	while (segWalk != oldSegList.end()) {
		for (unsigned int i = 0; i < (segWalk->second).connections.size(); i++) {
			(segWalk->second).connections[i].cGate = (*(gCircuit->getGates()))[(segWalk->second).connections[i].gid];
		}
		segWalk++;
	}
	if ( delta.x != 0 || delta.y != 0 ) {
		map < long, wireSegment >::iterator segWalk = oldSegList.begin();
		while (segWalk != oldSegList.end()) {
			(segWalk->second).begin.x -= delta.x; (segWalk->second).begin.y -= delta.y;
			(segWalk->second).end.x -= delta.x; (segWalk->second).end.y -= delta.y;
			segWalk++;
		}
	}
	(*(gCircuit->getWires()))[wid]->setSegmentMap(oldSegList);
	return true;
}

void cmdMoveWire::setPointers( GUICircuit* gCircuit, GUICanvas* gCanvas, hash_map < unsigned long, unsigned long > &gateids, hash_map < unsigned long, unsigned long > &wireids ) {
	this->gCircuit = gCircuit;
	wid = wireids[wid];
	map < long, wireSegment >::iterator segWalk = newSegList.begin();
	while (segWalk != newSegList.end()) {
		for (unsigned int i = 0; i < (segWalk->second).connections.size(); i++) {
			(segWalk->second).connections[i].gid = gateids[(segWalk->second).connections[i].gid];
			(segWalk->second).connections[i].cGate = (*(gCircuit->getGates()))[(segWalk->second).connections[i].gid];
		}
		segWalk++;
	}
	oldSegList = newSegList;
}

string cmdMoveWire::toString() const {
	if ( (gCircuit->getWires())->find(wid) == (gCircuit->getWires())->end() ) return ""; // error, wire not found
	ostringstream oss;
	oss << "movewire " << wid << " ";
	// Step through the map, save each seg's info
	map < long, wireSegment >::const_iterator segWalk = newSegList.cbegin();
	while (segWalk != newSegList.cend()) {
		// seg looks like "segment id bx,by,ex,ey connection gid,name isect key,id"
		if ((segWalk->second).isVertical()) oss << "vsegment ";
		else oss << "hsegment ";
		// ID
		oss << (segWalk->second).id << " ";
		// position - begin/end points
		oss << (segWalk->second).begin.x << "," << (segWalk->second).begin.y << "," << (segWalk->second).end.x << "," << (segWalk->second).end.y << " ";
		// connections - gid and connection string
		for (unsigned int i = 0; i < (segWalk->second).connections.size(); i++) {
			oss << "connection ";
			oss << (segWalk->second).connections[i].gid << " " << (segWalk->second).connections[i].connection << " ";
		}
		// intersections - must store the intersection map
		map < GLfloat, vector < long > >::const_iterator isectWalk = (segWalk->second).intersects.cbegin();
		while (isectWalk != (segWalk->second).intersects.cend()) {
			for (unsigned int j = 0; j < (isectWalk->second).size(); j++) {
				oss << "isect ";
				oss << isectWalk->first << " " << (isectWalk->second)[j] << " ";
			}
			isectWalk++;
		}
		segWalk++;
	}
	oss << " done ";
	return oss.str();	
}

// CMDMOVESELECTION

cmdMoveSelection::cmdMoveSelection( GUICircuit* gCircuit, vector < GateState > &preMove, vector < WireState > &preMoveWire, float startX, float startY, float endX, float endY ) : klsCommand(true, "Move Selection") {
	for (unsigned int i = 0; i < preMove.size(); i++) gateList.push_back(preMove[i].id);
	for (unsigned int i = 0; i < preMoveWire.size(); i++) {
		wireList.push_back(preMoveWire[i].id);
		if ( (gCircuit->getWires())->find(preMoveWire[i].id) == (gCircuit->getWires())->end() ) continue; // error, wire not found
		oldSegMaps[preMoveWire[i].id] = preMoveWire[i].oldWireTree;
		newSegMaps[preMoveWire[i].id] = (*(gCircuit->getWires()))[preMoveWire[i].id]->getSegmentMap();
	}
	this->gCircuit = gCircuit;
	this->startX = startX;
	this->startY = startY;
	this->endX = endX;
	this->endY = endY;
	wireMove = 1;
}

bool cmdMoveSelection::Do() {
	for (unsigned int i = 0; i < gateList.size(); i++) {
		if ( (gCircuit->getGates())->find(gateList[i]) == (gCircuit->getGates())->end() ) continue; // error, gate not found
		(*(gCircuit->getGates()))[gateList[i]]->translateGLcoords(endX-startX, endY-startY);
		(*(gCircuit->getGates()))[gateList[i]]->finalizeWirePlacements();
	}
	for (unsigned int i = 0; i < wireList.size(); i++) {
		if ( (gCircuit->getWires())->find(wireList[i]) == (gCircuit->getWires())->end() ) continue; // error, wire not found
		(*(gCircuit->getWires()))[wireList[i]]->setSegmentMap(newSegMaps[wireList[i]]);
	}
	for (unsigned int i = 0; i < proxconnects.size(); i++) {
		proxconnects[i]->Do();
	}
	return true;
}

bool cmdMoveSelection::Undo() {
	for (unsigned int i = 0; i < gateList.size(); i++) {
		if ( (gCircuit->getGates())->find(gateList[i]) == (gCircuit->getGates())->end() ) continue; // error, gate not found
		(*(gCircuit->getGates()))[gateList[i]]->translateGLcoords(startX-endX, startY-endY);
		(*(gCircuit->getGates()))[gateList[i]]->finalizeWirePlacements();
	}
	for (unsigned int i = 0; i < wireList.size() && wireMove < 0; i++) {
		if ( (gCircuit->getWires())->find(wireList[i]) == (gCircuit->getWires())->end() ) continue; // error, wire not found
		(*(gCircuit->getWires()))[wireList[i]]->setSegmentMap(oldSegMaps[wireList[i]]);
	}
	wireMove = -1;
	for (unsigned int i = 0; i < proxconnects.size(); i++) {
		proxconnects[i]->Undo();
	}
	return true;
}

// CMDCREATEGATE

cmdCreateGate::cmdCreateGate( GUICanvas* gCanvas, GUICircuit* gCircuit, unsigned long gid, string gateType, float x, float y) : klsCommand( true, "Create Gate") {
	this->gCanvas = gCanvas;
	this->gCircuit = gCircuit;
	this->gid = gid;
	this->gateType = gateType;
	this->x = x;
	this->y = y;
	this->fromString = false;
}

cmdCreateGate::cmdCreateGate( string def ) : klsCommand( true, "Create Gate" ) {
	istringstream iss(def);
	string dump;
	iss >> dump >> gid >> gateType >> x >> y;
	this->fromString = true;
}

bool cmdCreateGate::Do() {
	if (wxGetApp().libraries.size() == 0) return false; // No library loaded, so can't create gate

	gCircuit->createGate(gateType, gid, fromString);
	gCanvas->insertGate(gid, (*(gCircuit->getGates()))[gid], x, y);
	
	string logicType = wxGetApp().libParser.getGateLogicType( gateType );
	if( logicType.size() > 0 ) {
		ostringstream oss;
		gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_CREATE_GATE, new klsMessage::Message_CREATE_GATE(logicType, gid)));
	} // if( logic type is non-null )

	cmdSetParams setgateparams( gCircuit, gid, paramSet((*(gCircuit->getGates()))[gid]->getAllGUIParams(), (*(gCircuit->getGates()))[gid]->getAllLogicParams()), fromString );
	setgateparams.Do();

	// Must set hotspot params after the gate's params, because sometimes logic_params will create
	// the hotspots!
	if( logicType.size() > 0 ) {
		// Loop through the hotspots and pass logic core hotspot settings:
		LibraryGate libGate;
		wxGetApp().libParser.getGate(gateType, libGate);
		for( unsigned int i = 0; i < libGate.hotspots.size(); i++ ) {

			// Send the isInverted message:
			if( libGate.hotspots[i].isInverted ) {
				if ( libGate.hotspots[i].isInput ) {
					gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_INPUT_PARAM, new klsMessage::Message_SET_GATE_INPUT_PARAM(gid, libGate.hotspots[i].name, "INVERTED", "TRUE")));
				} else {
					gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_OUTPUT_PARAM, new klsMessage::Message_SET_GATE_OUTPUT_PARAM(gid, libGate.hotspots[i].name, "INVERTED", "TRUE")));
				}
			}

			// Send the logicEInput message:
			if( libGate.hotspots[i].logicEInput != "" ) {
				if ( libGate.hotspots[i].isInput ) {
					gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_INPUT_PARAM, new klsMessage::Message_SET_GATE_INPUT_PARAM(gid, libGate.hotspots[i].name, "E_INPUT", libGate.hotspots[i].logicEInput)));
				} else {
					gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_OUTPUT_PARAM, new klsMessage::Message_SET_GATE_OUTPUT_PARAM(gid, libGate.hotspots[i].name, "E_INPUT", libGate.hotspots[i].logicEInput)));
				}				
			}
		} // for( loop through the hotspots )
	} // if( logic type is non-null )


	for (unsigned int i = 0; i < proxconnects.size(); i++) {
		proxconnects[i]->Do();
	}
	return true;
}

bool cmdCreateGate::Undo() {
	for (unsigned int i = 0; i < proxconnects.size(); i++) {
		proxconnects[i]->Undo();
	}
	gCanvas->removeGate(gid);
	gCircuit->deleteGate(gid);
	string logicType = wxGetApp().libParser.getGateLogicType( gateType );
	if( logicType.size() > 0 ) {
		gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_DELETE_GATE, new klsMessage::Message_DELETE_GATE(gid)));
	}
	return true;
}

string cmdCreateGate::toString() const {
	ostringstream oss;
	oss << "creategate " << gid << " " << gateType << " " << x << " " << y;
	return oss.str();	
}

void cmdCreateGate::setPointers( GUICircuit* gCircuit, GUICanvas* gCanvas, hash_map < unsigned long, unsigned long > &gateids, hash_map < unsigned long, unsigned long > &wireids ) {
	// Find myself an appropriate ID for a new environment
	if (gateids.find(gid) != gateids.end()) {
		gid = gateids[gid];
	} else {
		gateids[gid] = gCircuit->getNextAvailableGateID();
		gid = gateids[gid];
	}
	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
}

// CMDDELETEGATE

cmdDeleteGate::cmdDeleteGate(GUICircuit* gCircuit, GUICanvas* gCanvas, unsigned long gid) : klsCommand(true, "Delete Gate") {
	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
	this->gid = gid;
}

cmdDeleteGate::~cmdDeleteGate ( void ) {
	while (!(cmdList.empty())) {
//		delete cmdList.top();
		cmdList.pop();
	}	
}

bool cmdDeleteGate::Do() {
	//make sure the gate exists
	if ( (gCircuit->getGates())->find(gid) == (gCircuit->getGates())->end() ) return false; //error: gate not found
	map< string, GLPoint2f > gateConns = (*(gCircuit->getGates()))[gid]->getHotspotList();
	map< string, GLPoint2f >::iterator connWalk = gateConns.begin();
	vector < int > deleteWires;
	//we will need to disconect all wires that connect to that gate from that gate
	//we iterate over the connections
	while (connWalk != gateConns.end()) {
		//if the connection is actually connected...
		if ((*(gCircuit->getGates()))[gid]->isConnected(connWalk->first)) {
			//grab the wire on that connection
			guiWire* gWire = (*(gCircuit->getGates()))[gid]->getConnection(connWalk->first);
			//create a disconnect command and do it
			cmdDisconnectWire* disconn = new cmdDisconnectWire( gCircuit, gWire->getID(), gid, connWalk->first );
			cmdList.push(disconn);
			disconn->Do();
			
			//----------------------------------------------------------------------------------------
			//Joshua Lansford edit 11/02/06--Added so "buffer" ports on a gate don't contain
			//wire artifacts after the rest of the wire has been deleted.  A buffer is created
			//by haveing a input and output hotspot in the same location. 
			//if the number of things the wire has left to connect is only two, then delete the wire.
			
			//first thing we verify is that we only have two connections left.
			if((*(gCircuit->getWires()))[gWire->getID()]->numConnections() == 2){
				//now we get the gid from both those connections.
				//I copied the test above from the test above from below.
				//I don't know why they are getting another reference to the wire when
				//they have gWire.  I suppose gWire doesn't get updated or something.
				//So I will do my work off of a freshly fetched wire and call it gWire2
				guiWire* gWire2 = (*(gCircuit->getWires()))[gWire->getID()];
				
				vector < wireConnection > connections = gWire2->getConnections();
				if( connections[0].gid == connections[1].gid ){
					
					//now we have to make sure that the connections are the same pin by comparing their positions
					guiGate* possibleBuffGate = (*(gCircuit->getGates()))[connections[0].gid];
					string* hotspot1Name = &connections[0].connection;
					string* hotspot2Name = &connections[1].connection;
					
					float x1 = 0, y1 = 0, x2 = 0, y2 = 0;
					possibleBuffGate->getHotspotCoords( *hotspot1Name, x1, y1 );
					possibleBuffGate->getHotspotCoords( *hotspot2Name, x2, y2 );
					
					if( x1 == x2 && y1 == y2 ){
						//this wire has met the requierments for being cooked.
						//so we will scedual it for being delted.
						deleteWires.push_back(gWire->getID());
					}
				}
			}
			//coment on edit. I compiled and tested this edit.
			//It doesn't delete wires that connect two different pins
			//on one chip when a gate is deleted.  It does delete a wire
			//connecting and input and a output that are in the same location
			//when you delete another gate
			//end of edit---------------the else on the following if was added as well------------------
			else if ((*(gCircuit->getWires()))[gWire->getID()]->numConnections() < 2) deleteWires.push_back(gWire->getID());
		}
		connWalk++;
	}
	
	for (unsigned int i = 0; i < deleteWires.size(); i++) {
		cmdDeleteWire* delwire = new cmdDeleteWire( gCircuit, gCanvas, deleteWires[i] );
		cmdList.push(delwire);
		delwire->Do();
	}

	float x, y;
	(*(gCircuit->getGates()))[gid]->getGLcoords(x, y);
	guiGate* gGate = (*(gCircuit->getGates()))[gid];
	cmdList.push ( new cmdMoveGate( gCircuit, gid, x, y, x, y ) );
	cmdList.push ( new cmdSetParams( gCircuit, gid, paramSet(gGate->getAllGUIParams(), gGate->getAllLogicParams()), true ) );
	
	gateType = (*(gCircuit->getGates()))[gid]->getLibraryGateName();
	
	gCanvas->removeGate(gid);
	gCircuit->deleteGate(gid, true);
	string logicType = wxGetApp().libParser.getGateLogicType( gateType );
	if( logicType.size() > 0 ) {
		gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_DELETE_GATE, new klsMessage::Message_DELETE_GATE(gid)));
	}
	return true;
}

bool cmdDeleteGate::Undo() {
	gCircuit->createGate(gateType, gid, true);

	string logicType = wxGetApp().libParser.getGateLogicType( gateType );
	if( logicType.size() > 0 ) {
		gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_CREATE_GATE, new klsMessage::Message_CREATE_GATE(logicType, gid)));
	}
	gCanvas->insertGate(gid, (*(gCircuit->getGates()))[gid], 0, 0);

	while (!(cmdList.empty())) {
		cmdList.top()->Undo();
		cmdList.pop();
	}
	return true;
}

// CMDDELETESELECTION

cmdDeleteSelection::cmdDeleteSelection(GUICircuit* gCircuit, GUICanvas* gCanvas, vector < unsigned long > &gates, vector < unsigned long > &wires) : klsCommand(true, "Delete Selection") {
	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
	for (unsigned int i = 0; i < gates.size(); i++) this->gates.push_back(gates[i]);
	for (unsigned int i = 0; i < wires.size(); i++) this->wires.push_back(wires[i]);
}

cmdDeleteSelection::~cmdDeleteSelection( void ) {
	while (!(cmdList.empty())) {
		delete cmdList.top();
		cmdList.pop();
	}	
}

bool cmdDeleteSelection::Do() {
	for (unsigned int i = 0; i < wires.size(); i++) {
		cmdList.push( new cmdDeleteWire( gCircuit, gCanvas, wires[i] ) );
		cmdList.top()->Do();
	}
	for (unsigned int i = 0; i < gates.size(); i++) {
		cmdList.push( new cmdDeleteGate( gCircuit, gCanvas, gates[i] ) );
		cmdList.top()->Do();
	}
	if (gCircuit->getOscope() != NULL) gCircuit->getOscope()->UpdateMenu();

	return true;
}

bool cmdDeleteSelection::Undo() {
	while (!(cmdList.empty())) {
		cmdList.top()->Undo();
		delete cmdList.top();
		cmdList.pop();
	}
	if (gCircuit->getOscope() != NULL) gCircuit->getOscope()->UpdateMenu();
	return true;
}







/*

TYLER DRAKE TODO BUS

Alright... We're like 'THIS' close.
Right now, the messages sent to the logic core look like: connect gate1, out_0 to wire[busline0].
Basically, the way I think it should look.
Also, in the gui, the buses look mostly right.

I need to create a logic gate for bus-ends!
It should take the buslines data and make hotspot *underscore* *number* outputs/inputs.
Use the junction logic_gate as a template!


make sure the mapping of gui hotspots to logic hotspots is okay.
So, buslines of 8 and hotspot "out" gives: out_0, out_1, ..., out_7.

*/





// CMDCONNECTWIRE

cmdConnectWire::cmdConnectWire(GUICircuit* gCircuit, IDType wid, IDType gid, const string &hotspot, bool noCalcShape) : klsCommand(true, "Connection") {
	this->gCircuit = gCircuit;
	this->wireId = wid;
	this->gateId = gid;
	this->hotspot = hotspot;
	this->noCalcShape = noCalcShape;
}

cmdConnectWire::cmdConnectWire(const string &def) : klsCommand(true, "Connection") {
	istringstream iss(def);
	string dump;
	iss >> dump >> wireId >> gateId >> hotspot;
	noCalcShape = true;
}

bool cmdConnectWire::Do() {

	if ((gCircuit->getWires())->find(wireId) == (gCircuit->getWires())->end()) return false; // error: wire not found
	if ((gCircuit->getGates())->find(gateId) == (gCircuit->getGates())->end()) return false; // error: gate not found

	guiGate* gate = gCircuit->getGates()->at(gateId);
	string hotspotPal = gate->getHotspotPal(hotspot);

	if (hotspotPal != "") {
		sendMessagesToConnect(gCircuit, wireId, gateId, hotspotPal, noCalcShape);
	}
	sendMessagesToConnect(gCircuit, wireId, gateId, hotspot, noCalcShape);

	return true;
}

bool cmdConnectWire::Undo() {

	if ((gCircuit->getWires())->find(wireId) == (gCircuit->getWires())->end()) return false; // error: wire not found
	if ((gCircuit->getGates())->find(gateId) == (gCircuit->getGates())->end()) return false; // error: gate not found

	guiGate* gate = gCircuit->getGates()->at(gateId);
	string hotspotPal = gate->getHotspotPal(hotspot);

	if (hotspotPal != "") {
		sendMessagesToDisconnect(gCircuit, wireId, gateId, hotspotPal);
	}
	sendMessagesToDisconnect(gCircuit, wireId, gateId, hotspot);

	return true;
}

bool cmdConnectWire::validateBusLines() const {

	guiWire *wire = (*gCircuit->getWires())[wireId];
	guiGate *gate = (*gCircuit->getGates())[gateId];

	int busLines = gate->getHotspot(hotspot)->getBusLines();

	// No connections on this wire? Cool. We'll make it a bus when we connect.
	if (wire->getConnections().empty()) {
		return true;
	}

	return wire->getIDs().size() == busLines;
}

string cmdConnectWire::toString() const {
	ostringstream oss;
	oss << "connectwire " << wireId << ' ' << gateId << ' ' << hotspot;
	return oss.str();
}

void cmdConnectWire::setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas, hash_map < unsigned long, unsigned long > &gateids, hash_map < unsigned long, unsigned long > &wireids) {
	gateId = gateids[gateId];
	wireId = wireids[wireId];
	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
}

IDType cmdConnectWire::getGateId() const {
	return gateId;
}

const std::string & cmdConnectWire::getHotspot() const {
	return hotspot;
}

void cmdConnectWire::sendMessagesToConnect(GUICircuit *gCircuit, IDType wireId, IDType gateId, const std::string &hotspot, bool noCalcShape) {

	guiGate *gate = gCircuit->getGates()->at(gateId);
	guiWire *wire = gCircuit->getWires()->at(wireId);

	// Grab the bus-lines from the wire.
	vector<IDType> wireIds = wire->getIDs();

	// Add the connection in the gui.
	gCircuit->setWireConnection(wireIds, gateId, hotspot, noCalcShape);

	// When connecting a gui hotspot to its underlying inputs/outputs,
	// single wires require no changes to their hotspot name, but
	// bus-lines need to have their bit appended.
	vector<string> internalHotspots;
	if (wireIds.size() == 1) {
		internalHotspots.push_back(hotspot);
	}
	else {
		for (int i = 0; i < wireIds.size(); i++) {
			internalHotspots.push_back(hotspot + "_" + to_string(i));
		}
	}

	bool isInput = gate->isConnectionInput(hotspot);

	// Connect each of wire's bus-lines to its corresponding gate hotspot.
	for (int i = 0; i < internalHotspots.size(); i++) {
		if (isInput) {
			gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_INPUT,
				new klsMessage::Message_SET_GATE_INPUT(gateId, internalHotspots[i], wireIds[i])));
		}
		else {
			gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_OUTPUT,
				new klsMessage::Message_SET_GATE_OUTPUT(gateId, internalHotspots[i], wireIds[i])));
		}
	}
}

void cmdConnectWire::sendMessagesToDisconnect(GUICircuit *gCircuit, IDType wireId, IDType gateId, const std::string &hotspot) {

	guiGate *gate = gCircuit->getGates()->at(gateId);
	guiWire *wire = gCircuit->getWires()->at(wireId);

	// Grab the bus-lines from the wire.
	vector<IDType> wireIds = wire->getIDs();
	
	// Remove the connection in the gui.
	int temp;
	gate->removeConnection(hotspot, temp);
	wire->removeConnection(gate, hotspot);

	// When disconnecting a gui hotspot from its underlying inputs/outputs,
	// single wires require no changes to their hotspot name, but
	// bus-lines need to have their bit appended.
	vector<string> internalHotspots;
	if (wireIds.size() == 1) {
		internalHotspots.push_back(hotspot);
	}
	else {
		for (int i = 0; i < wireIds.size(); i++) {
			internalHotspots.push_back(hotspot + "_" + to_string(i));
		}
	}

	bool isInput = gate->isConnectionInput(hotspot);

	// Disconnect each of wire's bus-lines from its corresponding gate hotspot.
	for (int i = 0; i < internalHotspots.size(); i++) {
		if (isInput) {
			gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_INPUT,
				new klsMessage::Message_SET_GATE_INPUT(gateId, internalHotspots[i], 0, true)));
		}
		else {
			gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_OUTPUT,
				new klsMessage::Message_SET_GATE_OUTPUT(gateId, internalHotspots[i], 0, true)));
		}
	}
}








// CMDDISCONNECTWIRE

cmdDisconnectWire::cmdDisconnectWire(GUICircuit* gCircuit, IDType wireId, IDType gateId, const string &hotspot, bool noCalcShape) : klsCommand(true, "Disconnection") {
	this->gCircuit = gCircuit;
	this->wireId = wireId;
	this->gateId = gateId;
	this->hotspot = hotspot;
	this->noCalcShape = noCalcShape;
}

bool cmdDisconnectWire::Do() {

	if ((gCircuit->getWires())->find(wireId) == (gCircuit->getWires())->end()) return false; // error: wire not found
	if ((gCircuit->getGates())->find(gateId) == (gCircuit->getGates())->end()) return false; // error: gate not found


	guiGate* gate = gCircuit->getGates()->at(gateId);
	string hotspotPal = gate->getHotspotPal(hotspot);

	if (hotspotPal != "") {
		cmdConnectWire::sendMessagesToDisconnect(gCircuit, wireId, gateId, hotspotPal);
	}
	cmdConnectWire::sendMessagesToDisconnect(gCircuit, wireId, gateId, hotspot);

	return true;
}

bool cmdDisconnectWire::Undo() {

	if ((gCircuit->getWires())->find(wireId) == (gCircuit->getWires())->end()) return false; // error: wire not found
	if ((gCircuit->getGates())->find(gateId) == (gCircuit->getGates())->end()) return false; // error: gate not found

	guiGate* gate = gCircuit->getGates()->at(gateId);
	string hotspotPal = gate->getHotspotPal(hotspot);

	if (hotspotPal != "") {
		cmdConnectWire::sendMessagesToConnect(gCircuit, wireId, gateId, hotspotPal, noCalcShape);
	}
	cmdConnectWire::sendMessagesToConnect(gCircuit, wireId, gateId, hotspot, noCalcShape);

	return true;
}

string cmdDisconnectWire::toString() const {
	ostringstream oss;
	oss << "disconnectwire " << wireId << " " << gateId << " " << hotspot;
	return oss.str();
}









































// CMDCREATEWIRE

cmdCreateWire::cmdCreateWire(GUICanvas* gCanvas, GUICircuit* gCircuit, const vector<IDType> &wireIds, cmdConnectWire* conn1, cmdConnectWire* conn2) : klsCommand(true, "Create Wire") {
	this->gCanvas = gCanvas;
	this->gCircuit = gCircuit;
	this->wireIds = wireIds;
	this->conn1 = conn1;
	this->conn2 = conn2;
}

cmdCreateWire::cmdCreateWire(const string &def) : klsCommand(true, "Create Wire") {
	istringstream iss(def);
	string dump;
	iss >> dump;
	
	// The list of id's is followed by a non-integer.
	// So we can just read until we can't get an integer.
	IDType tempId;
	while (iss >> tempId) {
		wireIds.push_back(tempId);
	}
	iss.clear();

	string wireid, gateid, hotspot;
	iss >> dump >> wireid >> gateid >> hotspot;
	conn1 = new cmdConnectWire(dump + " " + wireid + " " + gateid + " " + hotspot);
	iss >> dump >> wireid >> gateid >> hotspot;
	conn2 = new cmdConnectWire(dump + " " + wireid + " " + gateid + " " + hotspot);
}

cmdCreateWire::~cmdCreateWire() {
	delete conn1;
	delete conn2;
}

bool cmdCreateWire::Do() {

	guiWire *wire = gCircuit->createWire(wireIds);
	gCanvas->insertWire(wire);

	for (IDType wireId : wireIds) {
		gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_CREATE_WIRE,
			new klsMessage::Message_CREATE_WIRE(wireId)));
	}

	conn1->Do();
	conn2->Do();

	return true;
}

bool cmdCreateWire::Undo() {

	conn1->Undo();
	conn2->Undo();
	
	for (IDType wireId : wireIds) {
		gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_DELETE_WIRE,
			new klsMessage::Message_DELETE_WIRE(wireId)));
	}

	gCanvas->removeWire(wireIds[0]);
	gCircuit->deleteWire(wireIds[0]);

	return true;
}

bool cmdCreateWire::validateBusLines() const {

	IDType gate1Id = conn1->getGateId();
	IDType gate2Id = conn2->getGateId();

	guiGate *gate1 = (*gCircuit->getGates())[gate1Id];
	guiGate *gate2 = (*gCircuit->getGates())[gate2Id];

	string hotspot1 = conn1->getHotspot();
	string hotspot2 = conn2->getHotspot();

	int busLines1 = gate1->getHotspot(hotspot1)->getBusLines();
	int busLines2 = gate2->getHotspot(hotspot2)->getBusLines();

	return busLines1 == busLines2 && busLines1 == wireIds.size();
}

const vector<IDType> & cmdCreateWire::getWireIds() const {
	return wireIds;
}

string cmdCreateWire::toString() const {
	ostringstream oss;
	oss << "createwire ";

	// No need to put a count because thing after ids is a non-int.
	for (IDType id : wireIds) {
		oss << id << ' ';
	}
		
	oss << conn1->toString() << ' ' << conn2->toString();
	return oss.str();
}

void cmdCreateWire::setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas, hash_map < unsigned long, unsigned long > &gateids, hash_map < unsigned long, unsigned long > &wireids) {
	
	// remap ids.
	// notice the difference between wireIds and wireids.
	for (IDType &id : wireIds) {
		if (wireids.find(id) != wireids.end()) {
			id = wireids[id];
		}
		else {
			wireids[id] = gCircuit->getNextAvailableWireID();
			id = wireids[id];
		}
	}

	conn1->setPointers(gCircuit, gCanvas, gateids, wireids);
	conn2->setPointers(gCircuit, gCanvas, gateids, wireids);
	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
}






























// CMDDELETEWIRE

cmdDeleteWire::cmdDeleteWire(GUICircuit* gCircuit, GUICanvas* gCanvas, IDType wireId) : klsCommand(true, "Delete Wire") {
	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
	this->wireIds = { wireId };
}

cmdDeleteWire::~cmdDeleteWire() {
	while (!(cmdList.empty())) {
		delete cmdList.top();
		cmdList.pop();
	}
}

bool cmdDeleteWire::Do() {

	if ((gCircuit->getWires())->find(wireIds[0]) == (gCircuit->getWires())->end()) return false; //error: wire not found

	guiWire *wire = gCircuit->getWires()->at(wireIds[0]);

	wireIds = wire->getIDs();

	vector < wireConnection > destroyList = wire->getConnections();
	cmdMoveWire* movewire = new cmdMoveWire(gCircuit, wireIds[0], wire->getSegmentMap(), GLPoint2f(0, 0));
	cmdList.push(movewire);

	for (unsigned int j = 0; j < destroyList.size(); j++) {
		cmdDisconnectWire* disconn = new cmdDisconnectWire(gCircuit, wireIds[0], destroyList[j].cGate->getID(), destroyList[j].connection, true);
		cmdList.push(disconn);
		disconn->Do();
	}
	gCanvas->removeWire(wireIds[0]);
	gCircuit->deleteWire(wireIds[0]);

	for (IDType id : wireIds) {
		gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_DELETE_WIRE,
			new klsMessage::Message_DELETE_WIRE(id)));
	}

	return true;
}

bool cmdDeleteWire::Undo() {

	guiWire* gWire = gCircuit->createWire(wireIds);

	for (IDType id : wireIds) {
		gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_CREATE_WIRE,
			new klsMessage::Message_CREATE_WIRE(id)));
	}

	while (!(cmdList.empty())) {
		cmdList.top()->Undo();
		delete cmdList.top();
		cmdList.pop();
	}

	gCanvas->insertWire(gWire);

	return true;
}
























// CMDSETPARAMS

cmdSetParams::cmdSetParams(GUICircuit* gCircuit, unsigned long gid, paramSet pSet, bool setMode) : klsCommand(true, "Set Parameter") {
	if ((gCircuit->getGates())->find(gid) == (gCircuit->getGates())->end()) return; // error: gate not found
	this->gCircuit = gCircuit;
	this->gid = gid;
	this->fromString = setMode;
	// Save the original set of parameters
	map < string, string >::iterator paramWalk = (*(gCircuit->getGates()))[gid]->getAllGUIParams()->begin();
	while (paramWalk != (*(gCircuit->getGates()))[gid]->getAllGUIParams()->end()) {
		oldGUIParamList[paramWalk->first] = paramWalk->second;
		paramWalk++;
	}
	paramWalk = (*(gCircuit->getGates()))[gid]->getAllLogicParams()->begin();
	while (paramWalk != (*(gCircuit->getGates()))[gid]->getAllLogicParams()->end()) {
		oldLogicParamList[paramWalk->first] = paramWalk->second;
		paramWalk++;
	}
	// Now grab the new ones...
	if (pSet.gParams != NULL) {
		paramWalk = pSet.gParams->begin();
		while (paramWalk != pSet.gParams->end()) {
			newGUIParamList[paramWalk->first] = paramWalk->second;
			paramWalk++;
		}
	}
	if (pSet.lParams != NULL) {
		paramWalk = pSet.lParams->begin();
		while (paramWalk != pSet.lParams->end()) {
			newLogicParamList[paramWalk->first] = paramWalk->second;
			paramWalk++;
		}
	}
}

cmdSetParams::cmdSetParams(string def) : klsCommand(true, "Set Parameter") {
	this->fromString = true;
	istringstream iss(def);
	string dump; char comma;
	unsigned long numgParams, numlParams;
	iss >> dump >> gid >> numgParams >> comma >> numlParams;
	for (unsigned int i = 0; i < numgParams; i++) {
		string paramName, paramVal;
		iss >> paramName;
		getline(iss, paramVal, '\t');
		newGUIParamList[paramName] = paramVal.substr(1, paramVal.size() - 1);
		oldGUIParamList[paramName] = newGUIParamList[paramName];
	}
	for (unsigned int i = 0; i < numlParams; i++) {
		string paramName, paramVal;
		iss >> paramName;
		getline(iss, paramVal, '\t');
		newLogicParamList[paramName] = paramVal.substr(1, paramVal.size() - 1);
		oldLogicParamList[paramName] = newLogicParamList[paramName];
	}
}

bool cmdSetParams::Do() {
	if ((gCircuit->getGates())->find(gid) == (gCircuit->getGates())->end()) return false; // error: gate not found
	map < string, string >::iterator paramWalk = newLogicParamList.begin();
	vector < string > dontSendMessages;
	LibraryGate lg = wxGetApp().libraries[(*(gCircuit->getGates()))[gid]->getLibraryName()][(*(gCircuit->getGates()))[gid]->getLibraryGateName()];
	for (unsigned int i = 0; i < lg.dlgParams.size(); i++) {
		if (lg.dlgParams[i].isGui) continue;
		if (lg.dlgParams[i].type == "FILE_IN" || lg.dlgParams[i].type == "FILE_OUT") dontSendMessages.push_back(lg.dlgParams[i].name);
	}
	while (paramWalk != newLogicParamList.end()) {
		(*(gCircuit->getGates()))[gid]->setLogicParam(paramWalk->first, paramWalk->second);
		bool found = false;
		for (unsigned int i = 0; i < dontSendMessages.size() && !found; i++) {
			if (dontSendMessages[i] == paramWalk->first) found = true;
		}
		if (!found) gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(gid, paramWalk->first, paramWalk->second)));
		paramWalk++;
	}
	paramWalk = newGUIParamList.begin();
	while (paramWalk != newGUIParamList.end()) {
		(*(gCircuit->getGates()))[gid]->setGUIParam(paramWalk->first, paramWalk->second);
		paramWalk++;
	}
	if (!fromString && (*(gCircuit->getGates()))[gid]->getGUIType() == "TO" && gCircuit->getOscope() != NULL) gCircuit->getOscope()->UpdateMenu();
	return true;
}

bool cmdSetParams::Undo() {
	if ((gCircuit->getGates())->find(gid) == (gCircuit->getGates())->end()) return false; // error: gate not found
	map < string, string >::iterator paramWalk = oldLogicParamList.begin();
	vector < string > dontSendMessages;
	LibraryGate lg = wxGetApp().libraries[(*(gCircuit->getGates()))[gid]->getLibraryName()][(*(gCircuit->getGates()))[gid]->getLibraryGateName()];
	for (unsigned int i = 0; i < lg.dlgParams.size(); i++) {
		if (lg.dlgParams[i].isGui) continue;
		if (lg.dlgParams[i].type == "FILE_IN" || lg.dlgParams[i].type == "FILE_OUT") dontSendMessages.push_back(lg.dlgParams[i].name);
	}
	while (paramWalk != oldLogicParamList.end()) {
		(*(gCircuit->getGates()))[gid]->setLogicParam(paramWalk->first, paramWalk->second);
		bool found = false;
		for (unsigned int i = 0; i < dontSendMessages.size() && !found; i++) {
			if (dontSendMessages[i] == paramWalk->first) found = true;
		}
		if (!found) gCircuit->sendMessageToCore(klsMessage::Message(klsMessage::MT_SET_GATE_PARAM, new klsMessage::Message_SET_GATE_PARAM(gid, paramWalk->first, paramWalk->second)));
		paramWalk++;
	}
	paramWalk = oldGUIParamList.begin();
	while (paramWalk != oldGUIParamList.end()) {
		(*(gCircuit->getGates()))[gid]->setGUIParam(paramWalk->first, paramWalk->second);
		paramWalk++;
	}
	if (!fromString && (*(gCircuit->getGates()))[gid]->getGUIType() == "TO") gCircuit->getOscope()->UpdateMenu();
	return true;
}

string cmdSetParams::toString() const {
	ostringstream oss;
	oss << "setparams " << gid << " " << newGUIParamList.size() << "," << newLogicParamList.size() << " ";
	map < string, string >::const_iterator paramWalk = newGUIParamList.cbegin();
	while (paramWalk != newGUIParamList.cend()) {
		oss << paramWalk->first << " " << paramWalk->second << "\t";
		paramWalk++;
	}
	paramWalk = newLogicParamList.begin();
	while (paramWalk != newLogicParamList.end()) {
		oss << paramWalk->first << " " << paramWalk->second << "\t";
		paramWalk++;
	}
	return oss.str();
}

void cmdSetParams::setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas, hash_map < unsigned long, unsigned long > &gateids, hash_map < unsigned long, unsigned long > &wireids) {
	gid = gateids[gid];
	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
}

// CMDPASTEBLOCK

cmdPasteBlock::cmdPasteBlock(vector < klsCommand* > &cmdList) : klsCommand(true, "Paste") {
	for (unsigned int i = 0; i < cmdList.size(); i++) this->cmdList.push_back(cmdList[i]);
	m_init = false;
}

bool cmdPasteBlock::Do() {
	if (!m_init) {
		m_init = true;
		return true;
	}
	for (unsigned int i = 0; i < cmdList.size(); i++) cmdList[i]->Do();
	return true;
}

bool cmdPasteBlock::Undo() {
	for (int i = cmdList.size()-1; i >= 0; i--) {
		cmdList[i]->Undo();
	}
	return true;
}

// CMDWIRESEGDRAG

cmdWireSegDrag::cmdWireSegDrag(GUICircuit* gCircuit, GUICanvas* gCanvas, unsigned long wireID) : klsCommand(true, "Wire Shape") {
	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
	this->wireID = wireID;
	if ( (gCircuit->getWires())->find(wireID) == (gCircuit->getWires())->end() ) return; // error: wire not found
	oldSegMap = (*(gCircuit->getWires()))[wireID]->getOldSegmentMap();
	newSegMap = (*(gCircuit->getWires()))[wireID]->getSegmentMap();
}

bool cmdWireSegDrag::Do() {
	if ( (gCircuit->getWires())->find(wireID) == (gCircuit->getWires())->end() ) return false; // error: wire not found
	(*(gCircuit->getWires()))[wireID]->setSegmentMap(newSegMap);
	return true;
}

bool cmdWireSegDrag::Undo() {
	if ( (gCircuit->getWires())->find(wireID) == (gCircuit->getWires())->end() ) return false; // error: wire not found
	(*(gCircuit->getWires()))[wireID]->setSegmentMap(oldSegMap);
	return true;
}
