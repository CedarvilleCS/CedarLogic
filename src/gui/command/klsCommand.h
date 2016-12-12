
#pragma once

#include <string>
#include <hash_map>
#include "wx/cmdproc.h"
#include "../../logic/logic_values.h"

class GUICircuit;
class GUICanvas;

// Map of IDType to IDType used in copy-pasting.
// These "unsigned long"s need to be replaced by IDType's.
// Not sure why, but doing that right now is causing a crash on copy/paste.
// TODO TYLER: Find out why, make replacement, remove TODO and description.
typedef std::hash_map<unsigned long, unsigned long> TranslationMap;

// klsCommand - base class for commands.
class klsCommand : public wxCommand {
public:
	klsCommand(bool canUndo, const char *name);

	virtual ~klsCommand() = default;

	virtual std::string toString() const;

	virtual void setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas,
		TranslationMap &gateids, TranslationMap &wireids);

protected:
	GUICircuit *gCircuit;
	GUICanvas *gCanvas;
	bool fromString;
};