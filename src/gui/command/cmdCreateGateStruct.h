
#pragma once
#include "klsCommand.h"
#include "../guiGate.h"

// Pedro Casanova (casanova@ujaen.es) 2021/01-02
// cmdCreateGateStruct - Create a two level gate struct and others
class cmdCreateGateStruct : public klsCommand {
public:
	cmdCreateGateStruct(GUICanvas* gCanvas, GUICircuit* gCircuit, guiGate* gGate); 
	
	virtual ~cmdCreateGateStruct();

	bool Do();

	bool Undo();

	virtual std::string toString() const override;

	virtual void setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas,
		TranslationMap &gateids, TranslationMap &wireids) override;

private:
	guiGate* gGate;
	std::vector<klsCommand *> cmdList;
};