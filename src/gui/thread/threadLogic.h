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

#include <map>
#include "wx/wxprec.h"
#include "wx/wx.h"
#include "wx/thread.h"
#include "common.h"
#include "../message/Message.h"

using namespace std;

class Circuit;

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
    
    bool parseMessage(Message *input);

    void sendMessage(Message *message);
    
private:
	Circuit* cir;
	map < IDType, IDType >* logicIDs;
};

#endif /*THREADLOGIC_H_*/
