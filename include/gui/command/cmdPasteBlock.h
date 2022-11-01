
#pragma once
#include "klsCommand.h"
#include <vector>

// cmdPasteBlock - Paste's a block of gates/wires
class cmdPasteBlock : public klsCommand {
public:
	cmdPasteBlock(std::vector<klsCommand*> &cmdList);

	bool Do();

	bool Undo();

	void addCommand(klsCommand* cmd) { cmdList.push_back(cmd); };

private:
	std::vector<klsCommand *> cmdList;
	bool m_init;
};