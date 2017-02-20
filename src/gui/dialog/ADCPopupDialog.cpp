


#include <ostream>
#include <sstream>
#include <iomanip>
#include "wx/sizer.h"
#include "../gate/guiGate.h"
#include "../GUICircuit.h"
#include "../frame/MainFrame.h"
#include "ADCPopupDialog.h"


#define ID_SLIDER (wxID_HIGHEST + 1)

using namespace std;

DECLARE_APP(MainApp)


BEGIN_EVENT_TABLE(ADCPopupDialog, wxDialog)
	EVT_SCROLL(ADCPopupDialog::OnSliderChanged)
END_EVENT_TABLE()

ADCPopupDialog::ADCPopupDialog( guiGateADC* newguiGateADC, GUICircuit* newGUICircuit )
	:wxDialog( wxGetApp().GetTopWindow(), -1, ADC_TITLE, 
	    wxPoint(ADC_X_POS, ADC_Y_POS), 
	    wxSize(ADC_WIDTH, ADC_HEIGHT),
	    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER ){
	m_guiGateADC = newguiGateADC;
	gUICircuit = newGUICircuit;
	
	wxBoxSizer* topSizer = new wxBoxSizer( wxHORIZONTAL );

	slider = new wxSlider( this, ID_SLIDER, ADC_INIT_VALUE, 0, 255 );
	label = new wxStaticText( this, -1, "XXX");
	
	topSizer->Add( slider, wxSizerFlags(0).Align(0).Border(wxALL, 5 ) );
	topSizer->Add( label, wxSizerFlags(0).Align(0).Border(wxALL, 5 ) );
	
	SetSizer( topSizer );
	topSizer->SetSizeHints( this );
	
	notifyValueChanged();
}

//event which is called when the slider is changed
void ADCPopupDialog::OnSliderChanged( wxScrollEvent& event ){
	
	//here we send the command to the core.  The label gets updated
	//when the change gets poped back up to us
	gUICircuit->sendMessageToCore(new Message_SET_GATE_PARAM(m_guiGateADC->getID(), "VALUE", slider->GetValue()));
	gUICircuit->sendMessageToCore(new Message_UPDATE_GATES());
}

//this is so we can update the pop-up about the current value
void ADCPopupDialog::notifyValueChanged(){
	string currentValueAsString = m_guiGateADC->getLogicParam( "VALUE" );
	if( currentValueAsString == "" ){
		currentValueAsString = ADC_INIT_VALUE_STR;
	}
	int currentValue = 0;
	istringstream valueReader( currentValueAsString );
	valueReader >> currentValue;
	label->SetLabel(currentValueAsString);
	if( slider->GetValue() != currentValue ){
		slider->SetValue( currentValue );
	}
}
