
#pragma once
#include "klsCommand.h"
#include "../guiGate.h"

// Pedro Casanova (casanova@ujaen.es) 2021/01-02
// cmdCreateGateStruct - Create a two level gate struct
class cmdCreateGateStruct : public klsCommand {
public:
	cmdCreateGateStruct(GUICanvas* gCanvas, GUICircuit* gCircuit, guiGate* gGate); 
	
	cmdCreateGateStruct(const std::string &def);

	virtual ~cmdCreateGateStruct();

	bool Do();

	bool Undo();

	virtual std::string toString() const override;

	virtual void setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas,
		TranslationMap &gateids, TranslationMap &wireids) override;

private:
	guiGate* gGate;
	//map<string, string> gParamList;
	std::vector<klsCommand *> cmdList;
};