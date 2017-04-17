
#pragma once
#include "klsCommand.h"

// cmdDisconnectWire - disconnects a wire from a gate hotspot
class cmdDisconnectWire : public klsCommand {
public:
	cmdDisconnectWire(GUICircuit* gCircuit, IDType wid, IDType gid,
		const std::string &hotspot, bool noCalcShape = false);

	bool Do() override;

	bool Undo() override;

	virtual std::string toString() const override;

private:
	IDType gateId;
	IDType wireId;
	std::string hotspot;
	bool noCalcShape;
};
