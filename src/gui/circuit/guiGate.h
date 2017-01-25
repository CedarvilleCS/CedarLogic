
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

#include <float.h>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <string>
#include <fstream>
#include "wx/glcanvas.h"
#include "wx/docview.h"

#include "../../logic/logic_values.h"
#include "../XMLParser.h"
#include "../gl_text.h"
#include "../thread/Message.h"
#include "../dialog/RamPopupDialog.h"
#include "../dialog/Z80PopupDialog.h"
#include "../dialog/ADCPopupDialog.h"
#include "../gl_wrapper.h"

class guiWire;

using namespace std;


#define GATE_HOTSPOT_THICKNESS 0.05
#define SELECTED_LABEL_INTENSITY 0.50

// TO-FROM constants.
#define TO_FROM_TEXT_HEIGHT 1.5
#define TO_BUFFER 0.4
#define FROM_BUFFER 0.0
#define FROM_FIX_SHIFT 0.0
#define FLIPPED_OFFSET 0.5
