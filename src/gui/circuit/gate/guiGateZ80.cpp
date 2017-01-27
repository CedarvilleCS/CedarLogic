
#include "guiGateZ80.h"
#include "../../dialog/Z80PopupDialog.h"

using namespace std;

//I am creating a guiGate for the z80 so that
//the Z80 can have its own special pop-up window
guiGateZ80::guiGateZ80() {
	guiGate();
	z80PopupDialog = NULL;
}

guiGateZ80::~guiGateZ80() {
	//Destroy is how you 'delete' wxwidget objects
	if (z80PopupDialog != NULL) {
		z80PopupDialog->Destroy();
	}
}


void guiGateZ80::doParamsDialog(GUICircuit *gc, wxCommandProcessor *wxcmd) {
	if (z80PopupDialog == NULL) {
		z80PopupDialog = new Z80PopupDialog(this, (GUICircuit*)gc);
	}
	z80PopupDialog->Show(true);
}

void guiGateZ80::setLogicParam(const std::string &paramName, const std::string &value) {


	guiGate::setLogicParam(paramName, value);

	if (z80PopupDialog != NULL) {
		z80PopupDialog->NotifyOfRegChange();
	}

}