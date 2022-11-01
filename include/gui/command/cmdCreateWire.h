
#pragma once
#include "klsCommand.h"
#include <vector>

class cmdConnectWire;

// cmdCreateWire - creates a wire
class cmdCreateWire : public klsCommand {
public:
	cmdCreateWire(GUICanvas* gCanvas, GUICircuit* gCircuit,
		const std::vector<IDType> &wireIds, cmdConnectWire* conn1,
		cmdConnectWire* conn2);

	cmdCreateWire(const std::string &def);

	virtual ~cmdCreateWire();

	bool Do();

	bool Undo();

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