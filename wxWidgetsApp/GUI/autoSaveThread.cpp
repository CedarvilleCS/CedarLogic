//Author: Julian Pernia
//Date: 10/4/2016
//Implementing auto save thread

#include "autoSaveThread.h"
#include <string>
#include "MainFrame.h"
#include "MainApp.h"

DECLARE_APP(MainApp)

autoSaveThread::autoSaveThread() : wxThread()
{

}

void *autoSaveThread::Entry()
{
	time(&timeout);
	int waitTime;

	while (!TestDestroy())
	{
		//We only want auto save to happen every WAIT_TIME seconds
		waitTime = (int)difftime(time(NULL),timeout);
		if (waitTime > WAIT_TIME && wxGetApp().mainframe != NULL && wxGetApp().mainframe->FileIsDirty())
		{
			wxGetApp().mainframe->OnThreadSave();
			time(&timeout);
		}
	}

	return NULL;
}

void autoSaveThread::OnExit()
{

}