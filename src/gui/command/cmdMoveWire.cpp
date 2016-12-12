
#include "cmdMoveWire.h"
#include <sstream>
#include "../GUICircuit.h"
#include "../guiWire.h"

cmdMoveWire::cmdMoveWire(GUICircuit* gCircuit, unsigned long wid,
		const SegmentMap &oldList, const SegmentMap &newList) :
			klsCommand(true, "Move Wire") {

	this->gCircuit = gCircuit;
	this->wid = wid;
	oldSegList = oldList;
	newSegList = newList;
	delta = GLPoint2f(0, 0);
}

cmdMoveWire::cmdMoveWire(GUICircuit* gCircuit, unsigned long wid,
		const SegmentMap &oldList, GLPoint2f delta) :
			klsCommand(true, "Move Wire") {

	this->gCircuit = gCircuit;
	this->wid = wid;
	oldSegList = oldList;
	this->delta = delta;
}

cmdMoveWire::cmdMoveWire(string def) : klsCommand(true, "Move Wire") {

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
		newSegList[segID] = wireSegment(begin, end, isVertical, segID);
		if (!doneFirstSeg) {
			firstSegID = segID;
		}
		iss >> temp;
		while (temp == "connection") {
			int gid; string name; wireConnection wc;
			iss >> gid >> name;
			wc.gid = gid; wc.connection = name;
			//wc.cGate = (*(gCircuit->getGates()))[gid];
			newSegList[segID].connections.push_back(wc);
			iss >> temp;
		}
		while (temp == "isect") {
			GLfloat key; long sid;
			iss >> key >> sid;
			newSegList[segID].intersects[key].push_back(sid);
			iss >> temp;
		}
		doneFirstSeg = true;
	}
	oldSegList = newSegList;
	delta = GLPoint2f(0, 0);
}

bool cmdMoveWire::Do() {

	if ((gCircuit->getWires())->find(wid) == (gCircuit->getWires())->end()) return false; // error, wire not found

	if (delta.x != 0 || delta.y != 0) {
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

	if ((gCircuit->getWires())->find(wid) == (gCircuit->getWires())->end()) return false; // error, wire not found

	map < long, wireSegment >::iterator segWalk = oldSegList.begin();
	while (segWalk != oldSegList.end()) {
		for (unsigned int i = 0; i < (segWalk->second).connections.size(); i++) {
			(segWalk->second).connections[i].cGate = (*(gCircuit->getGates()))[(segWalk->second).connections[i].gid];
		}
		segWalk++;
	}
	if (delta.x != 0 || delta.y != 0) {
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

string cmdMoveWire::toString() const {

	if ((gCircuit->getWires())->find(wid) == (gCircuit->getWires())->end()) return ""; // error, wire not found

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

void cmdMoveWire::setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas,
		TranslationMap &gateids, TranslationMap &wireids) {

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
