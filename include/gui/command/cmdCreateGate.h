
#pragma once
#include "klsCommand.h"
#include <vector>

// cmdCreateGate - creates a gate on a given canvas at position (x,y)
class cmdCreateGate : public klsCommand {
public:
	cmdCreateGate(GUICanvas* gCanvas, GUICircuit* gCircuit,
		unsigned long gid, std::string gateType, float x, float y);

	cmdCreateGate(std::string def);

	bool Do();

	bool Undo();

	virtual std::string toString() const override;

	virtual void setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas,
		TranslationMap &gateids, TranslationMap &wireids) override;

	std::vector<klsCommand *> * getConnections();

protected:
	float x;
	float y;
	std::string gateType;
	unsigned long gid;
	std::vector<klsCommand *> proxconnects;
};