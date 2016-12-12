
#include "cmdPasteBlock.h"

cmdPasteBlock::cmdPasteBlock(std::vector<klsCommand *> &cmdList) :
		klsCommand(true, "Paste") {

	for (unsigned int i = 0; i < cmdList.size(); i++) this->cmdList.push_back(cmdList[i]);

	m_init = false;
}

bool cmdPasteBlock::Do() {

	if (!m_init) {
		m_init = true;
		return true;
	}

	for (unsigned int i = 0; i < cmdList.size(); i++) cmdList[i]->Do();

	return true;
}

bool cmdPasteBlock::Undo() {

	for (int i = cmdList.size() - 1; i >= 0; i--) {
		cmdList[i]->Undo();
	}

	return true;
}