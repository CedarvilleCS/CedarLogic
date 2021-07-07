
#pragma once
#include "klsCommand.h"

// Pedro Casanova (casanova@ujaen.es) 2020/04-12

// cmdMergeWire - Merge two wires
class cmdMergeWire : public klsCommand {
public:
	cmdMergeWire(GUICanvas* gCanvas, GUICircuit* gCircuit,
		const std::vector<IDType> &wireIds, IDType wid1, IDType wid2);

	virtual ~cmdMergeWire();

	bool Do();

	bool Undo();

	bool validateBusLines() const;

	const std::vector<IDType> & getWireIds() const;

	virtual std::string toString() const override;

	virtual void setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas,
		TranslationMap &gateids, TranslationMap &wireids) override;

private:
	std::vector<IDType> wireIds;
	IDType wid1;
	IDType wid2;
	std::vector<klsCommand *> cmdList;
};
