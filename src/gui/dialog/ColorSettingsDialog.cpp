#include "ColorSettingsDialog.h"


BEGIN_EVENT_TABLE(ColorSettingsDialog,wxDialog)

	EVT_BUTTON(wxID_EXIT, ColorSettingsDialog::OnClose)

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
	: wxDialog(parent,wxID_ANY,"Color Settings", wxDefaultPosition, wxSize(400, 400), wxCLOSE_BOX | wxCAPTION)
{
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	
	wxGridSizer* colorGrid = new wxGridSizer(4, 5, 5);
	wxGridSizer* settingsGrid = new wxGridSizer(2, 5, 5);

	colorGrid->Add(new wxStaticText(this, wxID_ANY, "Background"), wxALIGN_LEFT);
	colorGrid->Add(makeButton(SchematicBackground,ColorPalette::SchematicBackground), 0, wxCENTER, 2);

	colorGrid->Add(new wxStaticText(this, wxID_ANY, "Grid"), wxALIGN_LEFT);
	colorGrid->Add(makeButton(SchematicGrid, ColorPalette::SchematicGrid));

	colorGrid->Add(new wxStaticText(this, wxID_ANY, "Gate"), wxALIGN_LEFT);
	colorGrid->Add(makeButton(GateShape, ColorPalette::GateShape));

	colorGrid->Add(new wxStaticText(this, wxID_ANY, "Hotspot"), wxALIGN_LEFT);
	colorGrid->Add(makeButton(GateHotspot, ColorPalette::GateHotspot));

	colorGrid->Add(new wxStaticText(this, wxID_ANY, "Overlap"), wxALIGN_LEFT);
	colorGrid->Add(makeButton(GateOverlap, ColorPalette::GateOverlap));

	colorGrid->Add(new wxStaticText(this, wxID_ANY, "High"), wxALIGN_LEFT);
	colorGrid->Add(makeButton(WireHigh, ColorPalette::WireHigh));

	colorGrid->Add(new wxStaticText(this, wxID_ANY, "Low"), wxALIGN_LEFT);
	colorGrid->Add(makeButton(WireLow, ColorPalette::WireLow));

	colorGrid->Add(new wxStaticText(this, wxID_ANY, "HI Z"), wxALIGN_LEFT);
	colorGrid->Add(makeButton(WireHiZ, ColorPalette::WireHiZ));

	colorGrid->Add(new wxStaticText(this, wxID_ANY, "Unknown"), wxALIGN_LEFT);
	colorGrid->Add(makeButton(WireUnknown, ColorPalette::WireUnknown));

	colorGrid->Add(new wxStaticText(this, wxID_ANY, "Conflict"), wxALIGN_LEFT);
	colorGrid->Add(makeButton(WireConflict, ColorPalette::WireConflict));

	colorGrid->Add(new wxStaticText(this, wxID_ANY, "Keypad Selection"), wxALIGN_LEFT);
	colorGrid->Add(makeButton(KeypadSelection, ColorPalette::KeypadSelection));

	colorGrid->Add(new wxStaticText(this, wxID_ANY, "Text"), wxALIGN_LEFT);
	colorGrid->Add(makeButton(Text, ColorPalette::Text));

	colorGrid->Add(new wxStaticText(this, wxID_ANY, "Selected Text"), wxALIGN_LEFT);
	colorGrid->Add(makeButton(TextSelected, ColorPalette::TextSelected));

	colorGrid->Add(new wxStaticText(this, wxID_ANY, "Selection"), wxALIGN_LEFT);
	colorGrid->Add(makeButton(SelectionBoxFill, ColorPalette::SelectionBoxFill));

	colorGrid->Add(new wxStaticText(this, wxID_ANY, "Selection Border"), wxALIGN_LEFT);
	colorGrid->Add(makeButton(SelectionBoxBorder, ColorPalette::SelectionBoxBorder));

	settingsGrid->Add(new wxButton(this, SaveColors, "Save Colors"), 0, wxCENTER | wxALL, 2);
	settingsGrid->Add(new wxButton(this, DefaultColors, "Set Default Colors"), 0, wxCENTER | wxALL, 2);
	settingsGrid->Add(new wxButton(this, LoadColors, "Load Colors"), 0, wxCENTER | wxALL, 2);
	settingsGrid->Add(new wxButton(this, DarkColors, "Set Dark Colors"), 0, wxCENTER | wxALL, 2);

	mainSizer->Add(colorGrid, 0, wxEXPAND | wxALL | wxCENTER, 5);
	mainSizer->Add(settingsGrid, 0, wxEXPAND | wxALL | wxCENTER, 5);
	mainSizer->Add(new wxButton(this, wxID_EXIT, "Close"), 0, wxALL | wxCENTER, 2);

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
		this->GetWindowChild(event.GetId())->SetBackgroundColour(color);
	}
}

void ColorSettingsDialog::OnClose(wxCommandEvent& event) {
	this->Close();
}

void ColorSettingsDialog::setColor(Color &color, wxColor newColor) {
	color.r = (newColor.Red() / 255.0f);
	color.g = (newColor.Green() / 255.0f);
	color.b = (newColor.Blue() / 255.0f);
	color.a = (newColor.Alpha() / 255.0f);
}

wxColor ColorSettingsDialog::getColor(const Color &color) {
	return wxColor(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
}

wxButton* ColorSettingsDialog::makeButton(const unsigned long id, const Color &color) {
	wxButton* button = new wxButton(this, id, "", wxDefaultPosition, wxSize(25, 25));
	button->SetBackgroundColour(getColor(color));
	return button;
}

void ColorSettingsDialog::saveColors(wxCommandEvent& event) {

}

void ColorSettingsDialog::loadColors(wxCommandEvent& event) {

}

void ColorSettingsDialog::setDefault(wxCommandEvent& event) {

}

void ColorSettingsDialog::setDark(wxCommandEvent& event) {

}