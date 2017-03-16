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
	: wxDialog(parent,wxID_ANY,"Color Settings", wxDefaultPosition, wxSize(200, 720), wxCLOSE_BOX | wxSTAY_ON_TOP | wxCAPTION)
{
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	wxStaticBoxSizer* colorSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Colors");
	wxStaticBoxSizer* controlSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Settings");
	
	colorSizer->Add(new wxButton(this, SchematicBackground, "Background"), 0, wxCENTRE | wxALL, 2);
	colorSizer->Add(new wxButton(this, SchematicGrid, "Grid"), 0, wxCENTRE | wxALL, 2);
	colorSizer->Add(new wxButton(this, GateShape, "Gate"), 0, wxCENTRE | wxALL, 2);
	colorSizer->Add(new wxButton(this, GateHotspot, "Hotspot"), 0, wxCENTRE | wxALL, 2);
	colorSizer->Add(new wxButton(this, GateOverlap, "Overlap"), 0, wxCENTRE | wxALL, 2);
	colorSizer->Add(new wxButton(this, WireHigh, "High"), 0, wxCENTRE | wxALL, 2);
	colorSizer->Add(new wxButton(this, WireLow, "Low"), 0, wxCENTRE | wxALL, 2);
	colorSizer->Add(new wxButton(this, WireHiZ, "Hi Z"), 0, wxCENTRE | wxALL, 2);
	colorSizer->Add(new wxButton(this, WireUnknown, "Unknown"), 0, wxCENTRE | wxALL, 2);
	colorSizer->Add(new wxButton(this, WireConflict, "Conflict"), 0, wxCENTRE | wxALL, 2);
	colorSizer->Add(new wxButton(this, KeypadSelection, "Keypad Selection"), 0, wxCENTRE | wxALL, 2);
	colorSizer->Add(new wxButton(this, Text, "Text"), 0, wxCENTRE | wxALL, 2);
	colorSizer->Add(new wxButton(this, TextSelected, "Selected Text"), 0, wxCENTRE | wxALL, 2);
	colorSizer->Add(new wxButton(this, SelectionBoxFill, "Mouse Selection Fill"), 0, wxCENTRE | wxALL, 2);
	colorSizer->Add(new wxButton(this, SelectionBoxBorder, "Mouse Selection Border"), 0, wxCENTRE | wxALL, 2);

	controlSizer->Add(new wxButton(this, SaveColors, "Save Colors"), 0, wxCENTRE | wxALL, 2);
	controlSizer->Add(new wxButton(this, LoadColors, "Load Colors"), 0, wxCENTRE | wxALL, 2);
	controlSizer->Add(new wxButton(this, DefaultColors, "Set Default Colors"), 0, wxCENTRE | wxALL, 2);
	controlSizer->Add(new wxButton(this, DarkColors, "Set Dark Colors"), 0, wxCENTRE | wxALL, 2);
	
	mainSizer->Add(colorSizer, 0, wxEXPAND | wxALL, 5);
	mainSizer->Add(controlSizer, 0, wxEXPAND | wxALL, 5);
	mainSizer->Add(new wxButton(this, wxID_CLOSE, "Close"), 0, wxEXPAND | wxALL, 2);

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