
// Previously, all the guiGates were in a single header.
// This header is a drop-in replacement for that one.
// To improve compile times, this header should be avoided.

#pragma once

#include "gate/gateHotspot.h"
#include "gate/guiGate.h"
#include "gate/guiGateTOGGLE.h"
#include "gate/guiGateKEYPAD.h"
#include "gate/guiGateREGISTER.h"
#include "gate/guiGatePULSE.h"
#include "gate/guiGateLED.h"
#include "gate/guiLabel.h"
#include "gate/guiTO_FROM.h"
#include "gate/guiGateRAM.h"
#include "gate/guiGateZ80.h"
#include "gate/guiGateADC.h"
#include "gate/guiGateBlackBox.h"