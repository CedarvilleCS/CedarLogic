
#pragma once
#include "klsCommand.h"
#include <map>

typedef std::map<std::string, std::string> ParameterMap;

// Holds pointers to all of a gate's parameters
struct paramSet {

	ParameterMap *gParams;
	ParameterMap *lParams;

	paramSet(ParameterMap *g, ParameterMap* l);
};

// Pedro Casanova (casanova@ujaen.es) 2021/01-02
// Added canUndo in contructors to prevent FSM and CMB undo descriptions
// cmdSetParams - Sets a gate's parameters
class cmdSetParams : public klsCommand {
public:
	cmdSetParams(GUICircuit* gCircuit, unsigned long gid, paramSet pSet, bool canUndo = true, bool setMode = false);

	cmdSetParams(std::string def, bool canUndo = true);

	bool Do();

	bool Undo();

	virtual std::string toString() const override;

	virtual void setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas,
		TranslationMap &gateids, TranslationMap &wireids) override;

protected:
	unsigned long gid;
	ParameterMap oldGUIParamList;
	ParameterMap newGUIParamList;
	ParameterMap oldLogicParamList;
	ParameterMap newLogicParamList;
};