// For compilers that supports precompilation , includes "wx/wx.h"
#include "wx/wxprec.h"
 
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <ostream>
#include <sstream>
#include <iomanip>
#include <wx/sizer.h>
#include "CMBParamDialog.h"
#include "guiGate.h"
#include "GUICircuit.h"
#include "commands.h"

using namespace std;

DECLARE_APP(MainApp)

// Pedro Casanova (casanova@ujaen.es) 2021/01-02
// This class is the special pop-up window that comes up when a CMB chip is double clicked.
CMBParamDialog::CMBParamDialog( guiGateCMB* newM_guiGateCMB, GUICircuit* newGUICircuit, wxCommandProcessor* wxcmd)
	: wxDialog(wxGetApp().GetTopWindow(), wxID_ANY, "CMB info",
		wxDefaultPosition, wxSize(700, 500),
		wxCAPTION | wxFRAME_TOOL_WINDOW)
{
	m_guiGateCMB = newM_guiGateCMB;
	guiCircuit = newGUICircuit;
	this->wxcmd = wxcmd;
		
	wxBoxSizer* topSizer = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer* buttonSizer = new wxBoxSizer( wxHORIZONTAL );
		
	okBtn = new wxButton( this, wxID_OK, "&OK");
	cancelBtn = new wxButton(this, wxID_CANCEL, "&Cancel");
	clearBtn = new wxButton(this, wxID_CLEAR, "C&lear");

	wxStaticText* label = new wxStaticText(this, wxID_ANY, "Combinational functions:");

	functionTX = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition,wxSize(680,190), wxTE_MULTILINE | wxHSCROLL);

	infoTX = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(680,190), wxTE_MULTILINE | wxTE_READONLY);
	
	topSizer->Add(label, wxSizerFlags(0).Align(0).Border(wxALL, 5));
	topSizer->Add(functionTX, wxSizerFlags(0).Align(1).Border(wxALL, 5));
	topSizer->Add(infoTX, wxSizerFlags(0).Align(1).Border(wxALL, 5));
		
	buttonSizer->Add(okBtn, wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 5));	
	buttonSizer->Add(cancelBtn, wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 5));
	buttonSizer->Add(clearBtn, wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 5));
	
	topSizer->Add( buttonSizer,wxSizerFlags(0).Align(0).Border(wxALL, 5 ));

	*infoTX << "Line structure to define a CMB:\n"
		<< "Zero: Oi=0\n"
		<< "One: Oi=1\n"
		<< "Mask: Oi=hexnumber"
		<< "First canonical form: Oi=S(ma,mb,...)\n"
		<< "Second canonical form: Oi=P(Ma,Mb,...)\n"
		<< "\nYou must include all outpus\n"
		<< "\nExamples: (4 inputs, 5 outputs)\n"
		<< "\tO0=0\n"
		<< "\tO1=1\n"
		<< "\tO2=55AA\n"
		<< "\tO3=S(1,2,3,5,7,11,13)\n"
		<< "\tO4=P(0,4,6,8,9,10,12,14,15)\n"
		;



	SetSizer( topSizer );
}

void CMBParamDialog::OnBtnOK(wxCommandEvent& event) {

	string functionText = functionTX->GetValue() + "\n";
	vector <string> lines;

	while (true)
	{
		int posfin = functionText.find('\n');
		if (posfin == -1)
			posfin = functionText.size() - 1;
		if (posfin <= 0)
			break;
		string line = functionText.substr(0, posfin);
		functionText = functionText.substr(posfin + 1);

		// Remove spaces before and after text
		for (unsigned int i = 0; i < line.size(); i++)
			if (line[i] != ' ') {
				line = line.substr(i);
				break;
			}
		for (unsigned int i = line.size() - 1; i > 0; i--)
			if (line[i] == ' ')
				line = line.substr(0, i);
			else
				break;

		lines.push_back(line);
	}

	map < string, string > lParamList;
	map < string, string > gParamList;

	map <string, string>* logicParams = m_guiGateCMB->getAllLogicParams();

	unsigned int inBits = atoi(logicParams->find("INPUT_BITS")->second.c_str());
	unsigned int outBits = atoi(logicParams->find("OUTPUT_BITS")->second.c_str());
	unsigned int error = 0;
	unsigned int line;
	vector <string> outputs;

	if (lines.size() != outBits) {
		error = 10; 
	} else {
		for (line = 0; line < lines.size(); line++) {
			int pEqual;
			string output;
			ostringstream oss;

			pEqual = lines[line].find('=');
			if (pEqual < 0) { error = 1; break; }
			output = lines[line].substr(0, pEqual);
			if (output.length() < 2) { error = 2; break; }
			if (output.substr(0, 1) != "O" && output.substr(0, 1) != "o") { error = 2; break; }
			lines[line][0] = 'O';
			if (!chkDigits(output.substr(1,output.length()-1))) { error = 2; break; }
			unsigned int nOutput = atoi(output.substr(1).c_str());
			if (nOutput >= outBits) { error = 2; break; }
			for(unsigned int i = 0; i < outputs.size();i++)
				if (outputs[i] == output) { error = 5; break; }
			if (error) break;
			outputs.push_back(output);
			if (lines[line].substr(pEqual + 1) == "0" || lines[line].substr(pEqual + 1) == "1") {
				if ((int)lines[line].length() > pEqual + 2) { error = 3; break; }
			} else if (chkHexDigits(&lines[line].substr(pEqual + 1)) && inBits>1) {
				if (lines[line].substr(pEqual + 1).length()!=pow(2,inBits-2)) { error = 4; break; }
				string hexValue = lines[line].substr(pEqual + 1);
				chkHexDigits(&hexValue, true);
				for (unsigned int i = 0; i < hexValue.length(); i++)
					lines[line][pEqual + i + 1] = hexValue[i];
			} else if (lines[line].substr(pEqual + 1, 1) == "P" || lines[line].substr(pEqual + 1, 1) == "p" 
				|| lines[line].substr(pEqual + 1, 1) == "S" || lines[line].substr(pEqual + 1, 1) == "s") {	
				if ((int)lines[line].length() < pEqual + 5) { error = 3; break; }
				if (lines[line].substr(pEqual + 2, 1) != "(") { error = 3; break; }
				if (lines[line].substr(lines[line].length() - 1, 1) != ")") { error = 3; break; }
				if (lines[line].substr(pEqual + 1, 1) == "p") lines[line][pEqual + 1] = 'P';
				if (lines[line].substr(pEqual + 1, 1) == "s") lines[line][pEqual + 1] = 'S';
				istringstream iss(lines[line].substr(pEqual + 3, lines[line].length() - pEqual - 4) + ",");
				string chkCommas = iss.str();
				for (unsigned int i = 0; i < chkCommas.length(); i++)
					if (chkCommas[i] == ',')
						chkCommas[i] = '0';
				if (!chkDigits(chkCommas)) { error = 4; break; }
				while (true) {
					char dump;
					int term;
					iss >> term >> dump;
					if (iss.eof()) break;
					if (dump != ',') { error = 4; break; }
					if (term < 0 || term >= pow(2, inBits)) { error = 4; break; }
				}
				if (error) break;
			}
			else { error = 3; break; }
		}
	}

	if (error) {
		ostringstream msgError;
		switch (error)
		{
		case 1:
			msgError << "Incorrect sintax";
			break;
		case 2:
			msgError << "Incorrect output name";
			break;
		case 3:
			msgError << "Incorrect function";
			break;
		case 4:
			msgError << "Incorrect term value";
			break;
		case 5:
			msgError << "Duplicate output name";
			break;
		case 10:
			msgError << "Invalid number of functions";
			break;
		}
		if (error<10)
			msgError << " in line:\n" << lines[line];

		wxMessageBox(msgError.str(), "Error in CMB functions");
		return;
	}


	map <string, string>::iterator lparamsWalk = logicParams->begin();
	while (lparamsWalk != logicParams->end()) {
		if ((lparamsWalk->first).substr(0, 9) == "Function:") {
			lParamList[lparamsWalk->first] = "";
		}
		lparamsWalk++;
	}

	for (unsigned int i = 0; i < lines.size(); i++) {
		for (line = 0; line < outputs.size(); line++)
			if (atoi(outputs[line].substr(1).c_str()) == i) break;

		ostringstream oss;
		oss << "Function:" << i;
		lParamList[oss.str()] = lines[line];
	}
	
	wxcmd->Submit(new cmdSetParams(guiCircuit, m_guiGateCMB->getID(), paramSet(&gParamList, &lParamList)));

	EndModal(wxID_OK);

}

void CMBParamDialog::OnBtnCancel( wxCommandEvent& event ){
	EndModal(wxID_CANCEL);
}

void CMBParamDialog::OnBtnClear(wxCommandEvent& event) {
	functionTX->Clear();
}

BEGIN_EVENT_TABLE(CMBParamDialog, wxDialog)
	EVT_BUTTON(wxID_CLEAR, CMBParamDialog::OnBtnClear)
	EVT_BUTTON(wxID_CANCEL, CMBParamDialog::OnBtnCancel)
	EVT_BUTTON(wxID_OK, CMBParamDialog::OnBtnOK)
END_EVENT_TABLE()

