
#pragma once
#include <vector>
#include "klsCommand.h"
#include "gui/graphics/gl_defs.h"

// cmdCreateGate - creates a gate on a given canvas at position (x,y)
class cmdCreateGate : public klsCommand {
public:
	cmdCreateGate(GUICanvas* gCanvas, GUICircuit* gCircuit,
		unsigned long gid, std::string gateType, float x, float y);

	cmdCreateGate(std::string def);

	bool Do() override;

	bool Undo() override;

	virtual std::string toString() const override;

	virtual void setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas,
		TranslationMap &gateids, TranslationMap &wireids) override;

	std::vector<klsCommand *> * getConnections();

	Point getPosition() const;

	std::string getGateType() const;

protected:
	float x;
	float y;
	std::string gateType;
	unsigned long gid;
	std::vector<klsCommand *> proxconnects;
};
