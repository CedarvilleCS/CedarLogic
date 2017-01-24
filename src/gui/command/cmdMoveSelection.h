
#pragma once
#include "klsCommand.h"
#include <vector>
#include <map>
#include "../circuit/wireSegment.h"
#include "../widget/GUICanvas.h"

// Just a map of wire segments
typedef std::map<long, wireSegment> SegmentMap;

// cmdMoveSelection - move passed gates and wires
class cmdMoveSelection : public klsCommand {
public:
	cmdMoveSelection(GUICircuit* gCircuit, vector<GateState> &preMove,
		vector<WireState> &preMoveWire, float startX, float startY,
		float endX, float endY);

	bool Do();

	bool Undo();

	vector<klsCommand *> * getConnections();

protected:
	vector<unsigned long> gateList;
	vector<unsigned long> wireList;
	map<unsigned long, SegmentMap> oldSegMaps;
	map<unsigned long, SegmentMap> newSegMaps;
	float startX, startY, endX, endY;
	int wireMove;
	vector<klsCommand *> proxconnects;
};