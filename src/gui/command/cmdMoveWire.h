
#pragma once
#include "klsCommand.h"
#include <map>
#include "../wire/wireSegment.h"

// Just a map of wire segments
typedef std::map<long, wireSegment> SegmentMap;

// cmdMoveWire - moving a wire and storing it's segment maps (old and new)
class cmdMoveWire : public klsCommand {
public:
	cmdMoveWire(GUICircuit* gCircuit, unsigned long wid,
		const SegmentMap &oldList, const SegmentMap &newList);

	cmdMoveWire(GUICircuit* gCircuit, unsigned long wid,
		const SegmentMap &oldList, Point delta);

	cmdMoveWire(std::string def);

	bool Do(void);

	bool Undo(void);

	virtual std::string toString() const override;

	virtual void setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas,
		TranslationMap &gateids, TranslationMap &wireids) override;

protected:
	unsigned long wid;
	SegmentMap oldSegList;
	SegmentMap newSegList;
	Point delta;
};