#pragma once

#include "wx/colordlg.h"
#include "gui/graphics/ColorPalette.h"

enum {
SchematicBackground = wxID_HIGHEST + 1,
SchematicGrid,
GateShape,
GateHotspot,
GateOverlap,
WireHigh,
WireLow,
WireHiZ,
WireUnknown,
WireConflict,
KeypadSelection,
Text,
TextSelected,
SelectionBoxFill,
SelectionBoxBorder
};

class ColorSettingsDialog : public wxDialog {
public:
	ColorSettingsDialog(wxFrame* parent);

	void OnEditColor(wxCommandEvent& event);

private:

	void setColor(Color color, wxColour newColor);

	DECLARE_EVENT_TABLE()
};