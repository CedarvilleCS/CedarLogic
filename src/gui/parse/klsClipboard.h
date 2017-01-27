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

#include <vector>
using namespace std;

class cmdPasteBlock;
class GUICircuit;
class GUICanvas;

class klsClipboard {
public:
	klsClipboard() { return; };
	virtual ~klsClipboard() { return; };
	
	cmdPasteBlock* pasteBlock( GUICircuit* gCircuit, GUICanvas* gCanvas );
	void copyBlock( GUICircuit* gCircuit, GUICanvas* gCanvas, vector < unsigned long > gates, vector < unsigned long > wires );
};

#endif /*KLSCLIPBOARD_H_*/
