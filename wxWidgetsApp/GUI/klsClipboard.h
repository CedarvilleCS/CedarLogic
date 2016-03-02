/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   klsClipboard: handles copy and paste of blocks
*****************************************************************************/

#ifndef KLSCLIPBOARD_H_
#define KLSCLIPBOARD_H_

#include "MainApp.h"
#include "commands.h"
#include "GUICanvas.h"
#include "GUICircuit.h"
#include "guiGate.h"
#include "guiWire.h"
#include "wx/clipbrd.h"
#include "wx/dataobj.h"

using namespace std;

class klsClipboard {
public:
	klsClipboard() { return; };
	~klsClipboard() { return; };
	
	cmdPasteBlock* pasteBlock( GUICircuit* gCircuit, GUICanvas* gCanvas );
	void copyBlock( GUICircuit* gCircuit, GUICanvas* gCanvas, vector < unsigned long > gates, vector < unsigned long > wires );
	
private:
	
};

#endif /*KLSCLIPBOARD_H_*/
