//Author: Julian Pernia
//Date: 10/4/2016
//Implementing auto save thread

#include "autoSaveThread.h"
#include <string>
#include "../frame/MainFrame.h"
#include "../MainApp.h"

DECLARE_APP(MainApp)

autoSaveThread::autoSaveThread() : wxThread()
{

}

void *autoSaveThread::Entry()
{
	time(&timeout);
	int waitTime;

	while (wxGetApp().mainframe == NULL)
	{
		Sleep(10);
	}

	MainFrame *frame = wxGetApp().mainframe;

	while (!TestDestroy())
	{
		//We only want auto save to happen every WAIT_TIME seconds
		waitTime = (int)difftime(time(NULL), timeout);
		if (waitTime > WAIT_TIME)
		{
			if (frame != NULL && frame->fileIsDirty() && !frame->isHandlingEvent())
			{
				frame->autosave();
			}
			time(&timeout);
		}
		Sleep(10);
	}
	frame = NULL;
	return NULL;
}

void autoSaveThread::OnExit()
{

}