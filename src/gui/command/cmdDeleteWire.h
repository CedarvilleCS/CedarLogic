
#pragma once
#include <vector>
#include <stack>
#include "klsCommand.h"

// cmdDeleteWire - Deletes a wire
class cmdDeleteWire : public klsCommand {
public:
	cmdDeleteWire(GUICircuit* gCircuit, GUICanvas* gCanvas, IDType wireId);

	virtual ~cmdDeleteWire();

	bool Do() override;

	bool Undo() override;

private:
	std::vector<IDType> wireIds;
	std::stack<klsCommand *> cmdList;
};
