#ifndef ADCPOPUPDIALOG_H_
#define ADCPOPUPDIALOG_H_

class GUICircuit;
class guiGateADC;

#define ADC_TITLE _T("ADC")
#define ADC_X_POS 30
#define ADC_Y_POS 20
#define ADC_WIDTH 100
#define ADC_HEIGHT 30
#define ADC_INIT_VALUE_STR "19"
#define ADC_INIT_VALUE 19

class ADCPopupDialog : public wxDialog{
public:
	ADCPopupDialog( guiGateADC* newguiGateADC, GUICircuit* newGUICircuit );
	
	//event which is called when the slider is changed
	void OnSliderChanged( wxScrollEvent& event );
	
	//this is called by the ADCGuiGate when the value
	//property changes.
	void notifyValueChanged();

protected:
	DECLARE_EVENT_TABLE()
	
private:
	
	//this is the slider
	wxSlider* slider;
	wxStaticText* label;
	
	//pointers so that we can
	//get and send information
	guiGateADC* m_guiGateADC;
	GUICircuit* gUICircuit;
};

#endif /*ADCPOPUPDIALOG_H_*/
