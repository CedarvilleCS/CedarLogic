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

	EVT_BUTTON(SaveColors, ColorSettingsDialog::saveColors)
	EVT_BUTTON(LoadColors, ColorSettingsDialog::loadColors)
	EVT_BUTTON(DefaultColors, ColorSettingsDialog::setDefault)
	EVT_BUTTON(DarkColors, ColorSettingsDialog::setDark)

END_EVENT_TABLE()

ColorSettingsDialog::ColorSettingsDialog(wxFrame* parent)
	: wxDialog(parent,wxID_ANY,"Color Settings", wxDefaultPosition, wxDefaultSize, wxCLOSE_BOX | wxSTAY_ON_TOP | wxCAPTION)
{
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	wxStaticBoxSizer* colorSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Colors");
	wxBoxSizer* controlSizer = new wxBoxSizer(wxHORIZONTAL);
	
	colorSizer->Add(new wxButton(this, SchematicBackground, "Background"));
	colorSizer->Add(new wxButton(this, SchematicGrid, "Grid"));
	colorSizer->Add(new wxButton(this, GateShape, "Gate"));
	colorSizer->Add(new wxButton(this, GateHotspot, "Hotspot"));
	colorSizer->Add(new wxButton(this, GateOverlap, "Overlap"));
	colorSizer->Add(new wxButton(this, WireHigh, "High"));
	colorSizer->Add(new wxButton(this, WireLow, "Low"));
	colorSizer->Add(new wxButton(this, WireHiZ, "Hi Z"));
	colorSizer->Add(new wxButton(this, WireUnknown, "Unknown"));
	colorSizer->Add(new wxButton(this, WireConflict, "Conflict"));
	colorSizer->Add(new wxButton(this, KeypadSelection, "Keypad Selection"));
	colorSizer->Add(new wxButton(this, Text, "Text"));
	colorSizer->Add(new wxButton(this, TextSelected, "Selected Text"));
	colorSizer->Add(new wxButton(this, SelectionBoxFill, "Mouse Selection Fill"));
	colorSizer->Add(new wxButton(this, SelectionBoxBorder, "Mouse Selection Border"));

	controlSizer->Add(new wxButton(this, DefaultColors, "Set Default Colors"));
	controlSizer->Add(new wxButton(this, DarkColors, "Set Dark Colors"));
	controlSizer->Add(new wxButton(this, SaveColors, "Save Colors"));
	controlSizer->Add(new wxButton(this, LoadColors, "Load Colors"));
	controlSizer->Add(new wxButton(this, wxID_CLOSE, "Close"));

	mainSizer->Add(colorSizer);
	mainSizer->Add(controlSizer);

	this->SetSizer(mainSizer);
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
	color.r = (newColor.Red() / 255.0f);
	color.g = (newColor.Green() / 255.0f);
	color.b = (newColor.Blue() / 255.0f);
	color.a = (newColor.Alpha() / 255.0f);
}

void ColorSettingsDialog::saveColors(wxCommandEvent& event) {

}

void ColorSettingsDialog::loadColors(wxCommandEvent& event) {

}

void ColorSettingsDialog::setDefault(wxCommandEvent& event) {

}

void ColorSettingsDialog::setDark(wxCommandEvent& event) {

}