/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   CircuitPrint: Implements a printout object for print and print preview
*****************************************************************************/

#ifndef CIRCUITPRINT_H_
#define CIRCUITPRINT_H_

#include "wx/wxprec.h"
#include "wx/wx.h"
#include "wx/print.h"
#include "wx/printdlg.h"

#include "GUICanvas.h"
#include "GUICircuit.h"

class CircuitPrint : public wxPrintout {
public:
	CircuitPrint(GUICanvas* can, wxString strFile, wxChar *title = _T("Circuit Printout")):wxPrintout(title) { sourceCanvas = can; filename = strFile; }

	// Functions implemented for wxPrintout	
	bool OnPrintPage(int page);
	bool HasPage(int page);
	bool OnBeginDocument(int startPage, int endPage);
	void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);
		
	void DrawPageOne(wxDC *dc);

private:
	wxString filename;
	GUICanvas* sourceCanvas;
};

#endif /*CIRCUITPRINT_H_*/
