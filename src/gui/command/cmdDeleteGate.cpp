
#include "cmdDeleteGate.h"
#include <map>
#include "../GUICircuit.h"
#include "../wire/guiWire.h"
#include "../gate/guiGate.h"
#include "../widget/GUICanvas.h"
#include "../message/Message.h"
#include "../MainApp.h"
#include "cmdDisconnectWire.h"
#include "cmdDeleteWire.h"
#include "cmdMoveGate.h"
#include "cmdSetParams.h"

DECLARE_APP(MainApp);

cmdDeleteGate::cmdDeleteGate(GUICircuit* gCircuit, GUICanvas* gCanvas,
	IDType gateId) :
	klsCommand(true, "Delete Gate") {

	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
	this->gateId = gateId;
}

cmdDeleteGate::~cmdDeleteGate() {

	while (!(cmdList.empty())) {
		//		delete cmdList.top();
		cmdList.pop();
	}
}

bool cmdDeleteGate::Do() {

	//make sure the gate exists
	if ((gCircuit->getGates())->find(gateId) == (gCircuit->getGates())->end()) return false; //error: gate not found
	std::map<std::string, Point> gateConns = (*(gCircuit->getGates()))[gateId]->getHotspotList();
	auto connWalk = gateConns.begin();
	std::vector < int > deleteWires;
	//we will need to disconect all wires that connect to that gate from that gate
	//we iterate over the connections
	while (connWalk != gateConns.end()) {
		//if the connection is actually connected...
		if ((*(gCircuit->getGates()))[gateId]->isConnected(connWalk->first)) {
			//grab the wire on that connection
			guiWire* gWire = (*(gCircuit->getGates()))[gateId]->getConnection(connWalk->first);
			//create a disconnect command and do it
			cmdDisconnectWire* disconn = new cmdDisconnectWire(gCircuit, gWire->getID(), gateId, connWalk->first);
			cmdList.push(disconn);
			disconn->Do();

			//----------------------------------------------------------------------------------------
			//Joshua Lansford edit 11/02/06--Added so "buffer" ports on a gate don't contain
			//wire artifacts after the rest of the wire has been deleted.  A buffer is created
			//by haveing a input and output hotspot in the same location. 
			//if the number of things the wire has left to connect is only two, then delete the wire.

			//first thing we verify is that we only have two connections left.
			if ((*(gCircuit->getWires()))[gWire->getID()]->numConnections() == 2) {
				//now we get the gid from both those connections.
				//I copied the test above from the test above from below.
				//I don't know why they are getting another reference to the wire when
				//they have gWire.  I suppose gWire doesn't get updated or something.
				//So I will do my work off of a freshly fetched wire and call it gWire2
				guiWire* gWire2 = (*(gCircuit->getWires()))[gWire->getID()];

				std::vector < wireConnection > connections = gWire2->getConnections();
				if (connections[0].gid == connections[1].gid) {

					//now we have to make sure that the connections are the same pin by comparing their positions
					guiGate* possibleBuffGate = (*(gCircuit->getGates()))[connections[0].gid];
					std::string* hotspot1Name = &connections[0].connection;
					std::string* hotspot2Name = &connections[1].connection;

					float x1 = 0, y1 = 0, x2 = 0, y2 = 0;
					possibleBuffGate->getHotspotCoords(*hotspot1Name, x1, y1);
					possibleBuffGate->getHotspotCoords(*hotspot2Name, x2, y2);

					if (x1 == x2 && y1 == y2) {
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
		cmdDeleteWire* delwire = new cmdDeleteWire(gCircuit, gCanvas, deleteWires[i]);
		cmdList.push(delwire);
		delwire->Do();
	}

	float x, y;
	(*(gCircuit->getGates()))[gateId]->getGLcoords(x, y);
	guiGate* gGate = (*(gCircuit->getGates()))[gateId];
	cmdList.push(new cmdMoveGate(gCircuit, gateId, x, y, x, y));
	cmdList.push(new cmdSetParams(gCircuit, gateId, paramSet(gGate->getAllGUIParams(), gGate->getAllLogicParams()), true));

	gateType = (*(gCircuit->getGates()))[gateId]->getLibraryGateName();

	gCanvas->removeGate(gateId);
	gCircuit->deleteGate(gateId, true);
	std::string logicType = wxGetApp().libParser.getGateLogicType(gateType);
	if (logicType.size() > 0) {
		gCircuit->sendMessageToCore(new Message_DELETE_GATE(gateId));
	}
	return true;
}

bool cmdDeleteGate::Undo() {
	gCircuit->createGate(gateType, gateId, true);

	std::string logicType = wxGetApp().libParser.getGateLogicType(gateType);
	if (logicType.size() > 0) {
		gCircuit->sendMessageToCore(new Message_CREATE_GATE(logicType, gateId));
	}
	gCanvas->insertGate(gateId, (*(gCircuit->getGates()))[gateId], 0, 0);

	while (!(cmdList.empty())) {
		cmdList.top()->Undo();
		cmdList.pop();
	}
	return true;
}