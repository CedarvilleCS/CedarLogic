
#include "guiGateADC.h"
#include "../../dialog/ADCPopupDialog.h"

using namespace std;

//I am creating a guiGate for the ADC so that
//the ADC can have its own special pop-up window
guiGateADC::guiGateADC() {
	guiGate();
	aDCPopupDialog = NULL;
}

guiGateADC::~guiGateADC() {
	//Destroy is how you 'delete' wxwidget objects
	if (aDCPopupDialog != NULL) {
		aDCPopupDialog->Destroy();
		aDCPopupDialog = NULL;
	}
}


void guiGateADC::doParamsDialog(GUICircuit *gc, wxCommandProcessor *wxcmd) {
	if (aDCPopupDialog == NULL) {
		aDCPopupDialog = new ADCPopupDialog(this, (GUICircuit*)gc);
		aDCPopupDialog->notifyValueChanged();
	}
	aDCPopupDialog->Show(true);
}

void guiGateADC::setLogicParam(const std::string &paramName, const std::string &value) {
	guiGate::setLogicParam(paramName, value);

	if (paramName == "VALUE" && aDCPopupDialog != NULL)
		aDCPopupDialog->notifyValueChanged();
}