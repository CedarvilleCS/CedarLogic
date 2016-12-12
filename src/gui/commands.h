/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   commands: Implements a klsCommand object for each user interface command
*****************************************************************************/

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <map>
#include <hash_map>
#include <string>
#include <stack>
#include <vector>
#include "wx/cmdproc.h"
#include "gl_defs.h"
#include "wireSegment.h"
#include "GUICanvas.h"  // GateState, WireState

#include "wx/aui/auibook.h"

#include "../logic/logic_values.h"
using namespace std;

class GUICircuit;
class GUICanvas;



#include "command/klsCommand.h"
#include "command/cmdMoveGate.h"
#include "command/cmdMoveWire.h"
#include "command/cmdMoveSelection.h"
#include "command/cmdCreateGate.h"
#include "command/cmdConnectWire.h"
#include "command/cmdDisconnectWire.h"
#include "command/cmdCreateWire.h"
#include "command/cmdDeleteWire.h"
#include "command/cmdDeleteGate.h"
#include "command/cmdDeleteSelection.h"
#include "command/cmdPasteBlock.h"
#include "command/cmdWireSegDrag.h"
#include "command/cmdSetParams.h"
#include "command/cmdDeleteTab.h"
// 1 left...

























//JV - cmdAddTab - add a new tab into canvasBook
class cmdAddTab : public klsCommand {
protected:
	wxAuiNotebook* canvasBook;
	vector< GUICanvas* >* canvases;

public:
	cmdAddTab(GUICircuit* gCircuit, wxAuiNotebook* book, vector< GUICanvas* >* canvases);
	virtual ~cmdAddTab(void);

	bool Do(void);
	bool Undo(void);
};

#endif /*COMMANDS_H_*/
