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
#include "../../logic/logic_values.h"
#include "Message.h"
#include <deque>

using namespace std;

class Circuit;

class threadLogic : public wxThread
{
public:
    threadLogic();
	
    // thread execution starts here
    virtual void *Entry();

	void processAllLogicMessages();

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit();
    
    bool processLogicMessage(Message *input);

	void pushMessageToLogic(Message* message);
	void pushMessageToGui(Message* message);

	Message* popLogicMessage();
	Message* popGuiMessage();

	bool hasLogicMessage();
	bool hasGuiMessage();

	void clearMessagesToLogic();
	void clearMessagesToGui();
	void clearAllMessages();

private:
	wxMutex messageLogicMutex;
	wxMutex messageGuiMutex;

	Circuit* cir;
	map < IDType, IDType >* logicIDs;

	deque< Message *> guiToLogic;
	deque< Message *> logicToGui;
};

#endif /*THREADLOGIC_H_*/
