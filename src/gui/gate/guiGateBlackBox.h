
#pragma once
#include "guiGate.h"
#include <vector>
#include "common.h"

class guiGateBlackBox : public guiGate {
public:

	guiGateBlackBox(GUICircuit *circuit);

	~guiGateBlackBox();

	virtual void setGUIParam(const std::string &paramName,
		const std::string &value) override;

private:

	void createInternals(const std::string &internals);

	GUICircuit *circuit;
	std::vector<IDType> gateIds;
	std::vector<IDType> wireIds;

	static int bBoxCount;
};