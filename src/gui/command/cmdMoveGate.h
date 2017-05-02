
#pragma once
#include "klsCommand.h"

// cmdMoveGate - moving a gate from point a(x,y) to point b(x,y)
class cmdMoveGate : public klsCommand {
public:
	cmdMoveGate(GUICircuit* gCircuit, unsigned long gid, float startX,
		float startY, float endX, float endY, bool uW = false);

	bool Do();

	bool Undo();

	virtual std::string toString() const override;

protected:
	unsigned long gid;
	float startX;
	float startY;
	float endX;
	float endY;
	bool noUpdateWires;
};