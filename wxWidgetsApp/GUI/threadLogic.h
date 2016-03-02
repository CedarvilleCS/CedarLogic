/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   threadLogic: Main execution point of logic thread
*****************************************************************************/

#ifndef THREADLOGIC_H_
#define THREADLOGIC_H_

#include "wx/wxprec.h"
#include "wx/wx.h"
#include "wx/thread.h"
#include "klsMessage.h"
#include "../logic/logic_defaults.h"
#include "../logic/logic_wire.h"
#include "../logic/logic_gate.h"
#include "../logic/logic_circuit.h"
#include "../logic/logic_event.h"
#include <string>
#include <fstream>

using namespace std;

class threadLogic : public wxThread
{
public:
    threadLogic();
	
    // thread execution starts here
    virtual void *Entry();
	void checkMessages();
    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit();
    
    bool parseMessage(klsMessage::Message input);
    void sendMessage(klsMessage::Message message);
    
private:
	Circuit* cir;
	map < IDType, IDType >* logicIDs;
	ofstream logfile;
};

#endif /*THREADLOGIC_H_*/
