
#pragma once
#include "guiGate.h"

class guiGateBlackBox : public guiGate {
public:

	~guiGateBlackBox();

	virtual void setGUIParam(const std::string &paramName,
		const std::string &value) override;

private:

	void createInternals(const std::string &internals);
};