
#pragma once
#include "klsCommand.h"

// cmdPasteBlock - Paste's a block of gates/wires
class cmdPasteBlock : public klsCommand {
public:

	// right now, creating the command runs it.
	cmdPasteBlock(std::string &copyText, bool allowAutoIncrement);

	bool Do();

	bool Undo();

	void addCommand(klsCommand* cmd) { cmdList.push_back(cmd); };

	const std::vector<klsCommand *> & getCommands() const { return cmdList; }

private:
	std::vector<klsCommand *> cmdList;
	bool m_init;
};