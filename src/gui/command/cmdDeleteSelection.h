
#pragma once
#include "klsCommand.h"
#include <stack>

// cmdDeleteSelection - Deletes a selection of gates/wires
class cmdDeleteSelection : public klsCommand {
public:
	cmdDeleteSelection(GUICircuit* gCircuit, GUICanvas* gCanvas,
		std::vector<unsigned long> &gates, std::vector<unsigned long> &wires);

	virtual ~cmdDeleteSelection();

	bool Do() override;

	bool Undo() override;

private:
	std::vector<unsigned long> gates;
	std::vector<unsigned long> wires;
	std::stack<klsCommand *> cmdList;
};
