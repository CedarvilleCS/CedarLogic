#include "ColorSettingsDialog.h"


BEGIN_EVENT_TABLE(ColorSettingsDialog,wxDialog)

	EVT_BUTTON(SchematicBackground, ColorSettingsDialog::OnEditColor)
	EVT_BUTTON(SchematicGrid, ColorSettingsDialog::OnEditColor)
	EVT_BUTTON(GateShape, ColorSettingsDialog::OnEditColor)
	EVT_BUTTON(GateHotspot, ColorSettingsDialog::OnEditColor)
	EVT_BUTTON(GateOverlap, ColorSettingsDialog::OnEditColor)
	EVT_BUTTON(WireHigh, ColorSettingsDialog::OnEditColor)
	EVT_BUTTON(WireLow, ColorSettingsDialog::OnEditColor)
	EVT_BUTTON(WireHiZ, ColorSettingsDialog::OnEditColor)
	EVT_BUTTON(WireUnknown, ColorSettingsDialog::OnEditColor)
	EVT_BUTTON(WireConflict, ColorSettingsDialog::OnEditColor)
	EVT_BUTTON(KeypadSelection, ColorSettingsDialog::OnEditColor)
	EVT_BUTTON(Text, ColorSettingsDialog::OnEditColor)
	EVT_BUTTON(TextSelected, ColorSettingsDialog::OnEditColor)
	EVT_BUTTON(SelectionBoxFill, ColorSettingsDialog::OnEditColor)
	EVT_BUTTON(SelectionBoxBorder, ColorSettingsDialog::OnEditColor)

END_EVENT_TABLE()

ColorSettingsDialog::ColorSettingsDialog(wxFrame* parent)
	: wxDialog(parent,wxID_ANY,"Color Settings", wxDefaultPosition, wxDefaultSize, wxCLOSE_BOX | wxSTAY_ON_TOP | wxCAPTION)
{
	wxPanel* panel = new wxPanel(this, wxID_ANY);

	wxString labels[] = { "Background", "Grid", "Gate", "Hotspot", "Overlap", "High Value", "Low Value", "High Z Value", "Unknown Value", "Conflict Value", "Keypad Selection", "Text", "Selected Text", "Mouse Selection Fill", "Mouse Selection Border" };

	for (auto i = 0; i < 15; i++)
	{
		wxButton* button = new wxButton(panel, i + wxID_HIGHEST + 1, labels[i]);
	}

	wxButton* default = new wxButton(panel, wxID_ANY, "Set Default Colors");
	wxButton* dark = new wxButton(panel, wxID_ANY, "Set Dark Colors");
	wxButton* save = new wxButton(panel, wxID_ANY, "Save Colors");
	wxButton* load = new wxButton(panel, wxID_ANY, "Load Colors");
}

void ColorSettingsDialog::OnEditColor(wxCommandEvent& event) {
	
	wxColour color = wxGetColourFromUser();
	
	if (color.IsOk()) {
		switch (event.GetId()) {
		case SchematicBackground: setColor(ColorPalette::SchematicBackground, color);
			break;
		case SchematicGrid: setColor(ColorPalette::SchematicGrid, color);
			break;
		case GateShape: setColor(ColorPalette::GateShape, color);
			break;
		case GateHotspot: setColor(ColorPalette::GateHotspot, color);
			break;
		case GateOverlap: setColor(ColorPalette::GateOverlap, color);
			break;
		case WireHigh: setColor(ColorPalette::WireHigh, color);
			break;
		case WireLow: setColor(ColorPalette::WireLow, color);
			break;
		case WireHiZ: setColor(ColorPalette::WireHiZ, color);
			break;
		case WireUnknown: setColor(ColorPalette::WireUnknown, color);
			break;
		case WireConflict: setColor(ColorPalette::WireConflict, color);
			break;
		case KeypadSelection: setColor(ColorPalette::KeypadSelection, color);
			break;
		case Text: setColor(ColorPalette::Text, color);
			break;
		case TextSelected: setColor(ColorPalette::TextSelected, color);
			break;
		case SelectionBoxFill: setColor(ColorPalette::SelectionBoxFill, color);
			break;
		case SelectionBoxBorder: setColor(ColorPalette::SelectionBoxBorder, color);
			break;
		}
	}
}

void ColorSettingsDialog::setColor(Color &color, wxColor newColor) {
	color.r = (newColor.Red() / 255);
	color.g = (newColor.Green() / 255);
	color.b = (newColor.Blue() / 255);
	color.a = (newColor.Alpha() / 255);
}