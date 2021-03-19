// For compilers that supports precompilation , includes "wx/wx.h"
#include "wx/wxprec.h"
 
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <ostream>
#include <sstream>
#include <iomanip>
#include <wx/sizer.h>
#include "FSMParamDialog.h"
#include "guiGate.h"
#include "GUICircuit.h"
#include "commands.h"

using namespace std;

DECLARE_APP(MainApp)

// Pedro Casanova (casanova@ujaen.es) 2021/01-03
// This class is the special pop-up window that comes up when a FSM chip is double clicked.
FSMParamDialog::FSMParamDialog( guiGateFSM* newM_guiGateFSM, GUICircuit* newGUICircuit, wxCommandProcessor* wxcmd)
	: wxDialog(wxGetApp().GetTopWindow(), wxID_ANY, "FSM info",
		wxDefaultPosition, wxSize(700, 500),
		wxCAPTION | wxFRAME_TOOL_WINDOW)
{
	m_guiGateFSM = newM_guiGateFSM;
	guiCircuit = newGUICircuit;
	this->wxcmd = wxcmd;
		
	wxBoxSizer* topSizer = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer* buttonSizer = new wxBoxSizer( wxHORIZONTAL );
		
	okBtn = new wxButton( this, wxID_OK, "&OK");
	cancelBtn = new wxButton(this, wxID_CANCEL, "&Cancel");
	clearBtn = new wxButton(this, wxID_CLEAR, "C&lear");

	wxStaticText* label = new wxStaticText(this, wxID_ANY, "FSM transitions and outputs:");

	stateTX = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition,wxSize(680,190), wxTE_MULTILINE | wxHSCROLL);

	infoTX = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(680,190), wxTE_MULTILINE | wxTE_READONLY);
	
	topSizer->Add(label, wxSizerFlags(0).Align(0).Border(wxALL, 5));
	topSizer->Add(stateTX, wxSizerFlags(0).Align(1).Border(wxALL, 5));
	topSizer->Add(infoTX, wxSizerFlags(0).Align(1).Border(wxALL, 5));
		
	buttonSizer->Add(okBtn, wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 5));	
	buttonSizer->Add(cancelBtn, wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 5));
	buttonSizer->Add(clearBtn, wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 5));
	
	topSizer->Add( buttonSizer,wxSizerFlags(0).Align(0).Border(wxALL, 5 ));

	*infoTX << "Line structure to define a FSM:\n"
		<< "\tMoore model without inputs:\t\t"
		<< "STATE/OUTPUT-NEXTSTATE\n"
		<< "\tMoore model:\t\t\t"
		<< "STATE/OUTPUT INPUT-NEXTSTATE INPUT-NEXTSTATE ...\n"
		<< "\tMealy model:\t\t\t"
		<< "STATE INPUT-NEXTSTATE/OUTPUT INPUT-NEXTSTATE/OUTPUT ...\n"
		<< "\nFirst line state is reset state\n"
		<< "Mixed model are allowed\n"
		<< "Don´t care inputs all allowed (use X)\n"
		<< "Not defined inputs transitions remain in the same state with all outputs to cero\n"
		<< "\nExamples: (2 inputs, 4 outputs)\n"
		<< "\tQ0/0000-Q1\n"
		<< "\tQ1/0001 0X-Q1 10-Q2 11-Q3\n"
		<< "\tQ2 0X-Q0/0000 1X-Q3/1111\n"
		<< "\tQ3/0011 XX-Q0\n"
		;

	SetSizer( topSizer );
}

void FSMParamDialog::OnBtnOK(wxCommandEvent& event) {

	string stateText = stateTX->GetValue() + "\n";
	vector <string> lines;

	while (true)
	{
		long posfin = stateText.find('\n');
		if (posfin == -1)
			posfin = stateText.size() - 1;
		if (posfin <= 0)
			break;
		string line = stateText.substr(0, posfin);
		stateText = stateText.substr(posfin + 1);

		// Remove spaces before and after text
		for (unsigned long i = 0; i < line.size(); i++)
			if (line[i] != ' ') {
				line = line.substr(i);
				break;
			}
		for (unsigned long i = line.size() - 1; i > 0; i--)
			if (line[i] == ' ')
				line = line.substr(0, i);
			else
				break;

		lines.push_back(line);
	}

	map <string, string>* logicParams = m_guiGateFSM->getAllLogicParams();

	unsigned long inBits;
	istringstream(logicParams->find("INPUT_BITS")->second.c_str()) >> inBits;
	string inX = "";
	for (unsigned long i = 0; i < inBits; i++)
		inX = inX + "X";

	unsigned long outBits;
	istringstream(logicParams->find("OUTPUT_BITS")->second.c_str()) >> outBits;
	string out0 = "";
	for (unsigned long i = 0; i < outBits; i++)
		out0 = out0 + "0";

	unsigned long error = 0;
	unsigned long stateError;
	unsigned long line;
	vector <string> states;
	vector <string> nxstates;
	for (line = 0; line < lines.size(); line++) {
		long pSpace;
		long pSlash;
		long pArrow;
		string state;
		string nxstate;
		string inputs;
		string outputs;

		ostringstream oss;

		pSpace = lines[line].find(' ');
		if (pSpace < 0)	// EST/OUT-EST		Moore no inputs
		{
			pSlash = lines[line].find('/');
			pArrow = lines[line].find('-');
			if (pSlash < 0 || pArrow < 0) {error = 1; break; }
			if (pSlash == 0) { error = 1; break; }
			if (pSlash > pArrow) { error = 1; break; }
			if (inBits>0) { error = 2; break; }
			state = lines[line].substr(0, pSlash);
			for (unsigned long i = 0; i < states.size(); i++)
				if (state == states[i]) {
					error = 11;
					stateError = i;
					break;
				}
			if (error) break;
			states.push_back(state);
			outputs = lines[line].substr(pSlash + 1, pArrow - pSlash-1);
			if (outputs.length()!=outBits) { error = 3; break; }
			if (!chkBits(&outputs)) { error = 6; break; }
			nxstate = lines[line].substr(pArrow + 1);			
			if (nxstate == "") { error = 4; break; }
			nxstates.push_back(nxstate);
			oss << state << "/" << outputs << "-" << nxstate;
		}
		else {
			pSlash = lines[line].find('/');
			if (pSlash < 0) { error = 1; break; }
			if (pSlash < pSpace) // EST/OUT IN-EST IN-EST ...		Moore
			{
				state = lines[line].substr(0, pSlash);
				for (unsigned int i = 0; i < states.size(); i++)
					if (state == states[i]) {
						error = 11;
						stateError = i;
						break;
					}
				if (error) break;
				states.push_back(state);
				outputs = lines[line].substr(pSlash + 1, pSpace - pSlash - 1);
				if (outputs.length() != outBits) { error = 3; break; }
				if (!chkBits(&outputs)) { error = 6; break; }
				oss << state << "/" << outputs;
			}
			else	// EST IN-EST/OUT IN-EST/OUT ...		Mealy
			{
				state = lines[line].substr(0, pSpace);
				for (unsigned long i = 0; i < states.size(); i++)
					if (state == states[i]) {
						error = 11;
						stateError = i;
						break;
					}
				if (error) break;
				states.push_back(state);
				oss << state;
			}
			istringstream iss(lines[line].substr(pSpace + 1));
			while (!iss.eof()) {
				string trans;				
				iss >> trans;
				pArrow = trans.find('-');
				pSlash = trans.find('/');
				if (pArrow<0) { error = 1; break; }
				if (pSlash < 0)		// Moore
				{
					inputs = trans.substr(0, pArrow);					
					if (inputs.length() != inBits) { error = 2; break; }
					if (!chkBits(&inputs,true)) { error = 5; break; }
					nxstate = trans.substr(pArrow + 1);
					if (nxstate == "") { error = 4; break; }
					nxstates.push_back(nxstate);
					oss << " " << inputs << "-" << nxstate;
				}
				else				// Mealy
				{
					if (pSlash < pArrow) { error = 1; break; }
					inputs = trans.substr(0, pArrow);
					if (inputs.length() != inBits) { error = 2; break; }
					if (!chkBits(&inputs, true)) { error = 5; break; }
					outputs = trans.substr(pSlash+1);
					if (outputs.length() != outBits) { error = 3; break; }
					if (!chkBits(&outputs)) { error = 6; break; }
					nxstate = trans.substr(pArrow + 1, pSlash - pArrow - 1);
					if (nxstate == "") { error = 4; break; }
					nxstates.push_back(nxstate);
					oss << " " << inputs << "-" << nxstate << "/" << outputs;
				}
			}
		}
		if (error) break;
		lines[line] = oss.str();
	}

	if (!error)
		for (unsigned long i = 0; i < nxstates.size(); i++) {
			bool found = false;
			for (unsigned long j = 0; j < states.size(); j++)
				if (nxstates[i] == states[j]) {found = true; break;}
			if (!found)	{ stateError = i;  error = 10; break; }
		}

	if (error) {
		ostringstream msgError;
		switch (error)
		{
		case 1:
			msgError << "Incorrect sintax";
			break;
		case 2:
			msgError << "Incorrect input length";
			break;
		case 3:
			msgError << "Incorrect output length";
			break;
		case 4:
			msgError << "Incorrect next state";
			break;
		case 5:
			msgError << "Invalid input value";
			break;
		case 6:
			msgError << "Invalid output value";
			break;
		case 10:
			msgError << "State not found";
			break;
		case 11:
			msgError << "Duplicate state";
			break;
		}
		
		if (error == 10)
			msgError << ": " << nxstates[stateError];
		else if (error == 11)
			msgError << ": " << states[stateError];

		if (error!=10)
			msgError << " at line:\n" << lines[line];

		wxMessageBox(msgError.str(), "Error in FSM description");
		return;
	}

	map < string, string > lParamList;

	map <string, string>::iterator lparamsWalk = logicParams->begin();
	while (lparamsWalk != logicParams->end()) {
		if ((lparamsWalk->first).substr(0, 6) == "State:") {
			lParamList[lparamsWalk->first] = "";
		}
		lparamsWalk++;
	}

	for (unsigned long i = 0; i < lines.size(); i++) {
		ostringstream oss;
		oss << "State:" << i;
		lParamList[oss.str()] = lines[i];
	}

	lParamList["CLEAR_FSM"] = "true";
	wxcmd->Submit(new cmdSetParams(guiCircuit, m_guiGateFSM->getID(), paramSet(NULL, &lParamList),false));

	EndModal(wxID_OK);

}

void FSMParamDialog::OnBtnCancel( wxCommandEvent& event ){
	EndModal(wxID_CANCEL);
}

void FSMParamDialog::OnBtnClear(wxCommandEvent& event) {
	stateTX->Clear();
}

BEGIN_EVENT_TABLE(FSMParamDialog, wxDialog)
	EVT_BUTTON(wxID_CLEAR, FSMParamDialog::OnBtnClear)
	EVT_BUTTON(wxID_CANCEL, FSMParamDialog::OnBtnCancel)
	EVT_BUTTON(wxID_OK, FSMParamDialog::OnBtnOK)
END_EVENT_TABLE()

