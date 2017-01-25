
#pragma once
#include "guiGate.h"

class guiGatePULSE : public guiGate {
public:
	guiGatePULSE();

	Message_SET_GATE_PARAM* checkClick(GLfloat x, GLfloat y);
};