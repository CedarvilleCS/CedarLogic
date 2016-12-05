//Author: Julian Pernia
//Date: 10/4/2016
//Thread to create temporary files to reduce lost work upon crash or system failure.

#ifndef AUTO_SAVE_THREAD_H
#define AUTO_SAVE_THREAD_H

class autoSaveThread : public wxThread
{
public:
	autoSaveThread();

	virtual void *Entry();

	virtual void OnExit();

private:

	time_t timeout;
	const int WAIT_TIME = 300;

};

#endif //AUTO_SAVE_THREAD_H