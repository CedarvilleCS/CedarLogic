
#pragma once
#include "klsCommand.h"
#include "../guiGate.h"

// Pedro Casanova (casanova@ujaen.es) 2021/01-03
// cmdCreateGateStruct - Create a two level gate struct, wires, PLD ...
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
	unsigned long getGID();

	unsigned long getWID();

	void drawWires();

	void drawCircuit();

	void drawPLD();


	guiGate* gGate;
	std::vector<klsCommand *> cmdList;
	string gateLibraryName;
	map<string, string> gParamList;
	GLfloat x0, y0;	
	vector<unsigned long> gids;
	vector<unsigned long> wids;
	unsigned long countGID;
	unsigned long countWID;

};
