
#pragma once

#include "gui/MainApp.h"
#include "wx/glcanvas.h"
#include "common.h"
#include <string>

const int GATEIMAGESIZE = 32;
const int IMAGESIZE = 34;

class guiGate;

class gateImage : public wxGLCanvas {

	wxDECLARE_EVENT_TABLE();

public:
    gateImage(const std::string &gateName, wxWindow *parent);

	virtual ~gateImage();

    void OnPaint(wxPaintEvent& event);

    void OnMouseEvent(wxMouseEvent& event);

	void OnEnterWindow(wxMouseEvent& event);

	void OnLeaveWindow(wxMouseEvent& event);

	void OnEraseBackground(wxEraseEvent& event);

private:

	void setViewport(guiGate *m_gate);



	std::string gateName;

	bool mouseHover;



	// GLContext shared for all gateImage's.
	static wxGLContext *glContext;

	// gateImage counter so wxGLContext can be destroyed eventually.
	static int count;
};