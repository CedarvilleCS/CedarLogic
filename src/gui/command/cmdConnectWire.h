
#pragma once
#include "klsCommand.h"

// cmdConnectWire - connects a wire to a gate hotspot
class cmdConnectWire : public klsCommand {
public:
	cmdConnectWire(GUICircuit* gCircuit, IDType wid, IDType gid,
		const std::string &hotspot, bool noCalcShape = false);

	cmdConnectWire(const std::string &def);

	bool Do() override;

	bool Undo() override;

	bool validateBusLines() const;

	virtual std::string toString() const override;

	virtual void setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas,
		TranslationMap &gateids, TranslationMap &wireids) override;

	IDType getGateId() const;

	const std::string & getHotspot() const;

	static void sendMessagesToConnect(GUICircuit *gCircuit, IDType wireId,
		IDType gateId, const std::string &hotspot, bool noCalcShape);

	static void sendMessagesToDisconnect(GUICircuit *gCircuit, IDType wireId,
		IDType gateId, const std::string &hotspot);

private:
	IDType gateId;
	IDType wireId;
	std::string hotspot;
	bool noCalcShape;
};
