#include "ColorSettingsDialog.h"


BEGIN_EVENT_TABLE(ColorSettingsDialog,wxDialog)

	EVT_BUTTON(SchematicBackground,ColorSettingsDialog::OnEditColor)

END_EVENT_TABLE()

ColorSettingsDialog::ColorSettingsDialog(wxFrame* parent)
	: wxDialog(parent,wxID_ANY,"Color Settings", wxDefaultPosition)
{
}

void ColorSettingsDialog::OnEditColor(wxCommandEvent& event) {
	
	wxColour color = wxGetColourFromUser();
	
	if (color.IsOk()) {
		switch (event.GetId) {
		case SchematicBackground: setColor(ColorPalette::SchematicBackground, color);
		}
	}
}

