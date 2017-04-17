
#pragma once
#include "klsCommand.h"

// cmdPasteBlock - Paste's a block of gates/wires
class cmdPasteBlock : public klsCommand {
public:

	// right now, creating the command runs it.
	cmdPasteBlock(std::string &copyText, bool allowAutoIncrement, GUICircuit *gCircuit,
		GUICanvas *gCanvas);

	bool Do();

	bool Undo();

	void addCommand(klsCommand* cmd) { cmdList.push_back(cmd); };

	const std::vector<klsCommand *> & getCommands() const { return cmdList; }

	// Get ids for gates created by pasting.
	// The second value in each pair is the valid id.
	const TranslationMap & getGateIds() const;

	// Get ids for wires created by pasting.
	// The second value in each pair is the valid id.
	const TranslationMap & getWireIds() const;

private:
	std::vector<klsCommand *> cmdList;

	TranslationMap gateids;
	TranslationMap wireids;

	bool alreadyDone;
};