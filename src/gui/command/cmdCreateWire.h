
#pragma once
#include <vector>
#include "klsCommand.h"

class cmdConnectWire;

// cmdCreateWire - creates a wire
class cmdCreateWire : public klsCommand {
public:
	cmdCreateWire(GUICanvas* gCanvas, GUICircuit* gCircuit,
		const std::vector<IDType> &wireIds, cmdConnectWire* conn1,
		cmdConnectWire* conn2);

	cmdCreateWire(const std::string &def);

	virtual ~cmdCreateWire();

	bool Do() override;

	bool Undo() override;

	bool validateBusLines() const;

	const std::vector<IDType> & getWireIds() const;

	virtual std::string toString() const override;

	virtual void setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas,
		TranslationMap &gateids, TranslationMap &wireids) override;

private:
	std::vector<IDType> wireIds;
	cmdConnectWire* conn1;
	cmdConnectWire* conn2;
};
