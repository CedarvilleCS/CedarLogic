
#include "cmdPasteBlock.h"
#include <sstream>
#include "gui/commands.h"
#include "gui/MainApp.h"
#include "gui/frame/OscopeFrame.h"
#include "gui/wire/guiWire.h"
#include "gui/gate/guiGate.h"
DECLARE_APP(MainApp);
using namespace std;

namespace {

	void increment(string &temp, string &pasteText) {

		string numEnd = "";	// String of numbers on end that we will build

							// Loop from end of temp to beginning, gathering up numbers to build unto numEnd
							// Starts at temp.length() - 2 so that it starts at the end minus one because 
							// temp always ends with a /t

		for (int i = temp.length() - 2; i > 0; i--) {
			if (isdigit(temp[i])) {
				numEnd = temp[i] + numEnd;
			}
			else {
				break;
			}
		}

		// If we have numbers to add and we are naming a junction_id
		if (numEnd != "" && temp.find("JUNCTION_ID") != std::string::npos) {
			string newPasteText;
			newPasteText = pasteText; // This string will be modified and rewritten to the clipboard so that subsequent pastes continue to increment

			temp.erase(temp.length() - 1 - numEnd.length(), numEnd.length() + 1); // Erase number at end of tag, add 1 to erase the \t also
			newPasteText.erase(newPasteText.length() - 2 - numEnd.length(), numEnd.length() + 2);  // Modify clipboard data similarly, but +2 so it erases the \n also

			int holder = stoi(numEnd);
			holder++; // The whole point of this -- increment number at end of tag by 1
			string s = to_string(holder) + "\t";

			temp += s; // Add it back to temp string
			newPasteText += s + "\n";

			pasteText = newPasteText;
		}
	}
}

cmdPasteBlock::cmdPasteBlock(std::string &pasteText, bool allowAutoIncrement,
	GUICircuit *gCircuit, GUICanvas *gCanvas) :
	klsCommand(true, "Paste") {

	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
	this->alreadyDone = false;

	if (pasteText.empty()) {
		return;
	}

	istringstream iss(pasteText);
	string temp;

	while (getline(iss, temp, '\n')) {
		klsCommand* cg = NULL;
		if (temp.substr(0, 10) == "creategate") cg = new cmdCreateGate(temp);
		else if (temp.substr(0, 9) == "setparams") {

			/* EDIT by Colin Broberg, 10/6/16
			logic to increment number on end of TO/FROM tag */
			bool enabled = wxGetApp().appSettings.autoIncrement && allowAutoIncrement;
			// If we are copying more than one thing, don't increment them -- that would be annoying
			if (enabled && pasteText.find("creategate", pasteText.find("creategate") + 1) == std::string::npos) {
				increment(temp, pasteText);
			}
			cg = new cmdSetParams(temp);
		}
		else if (temp.substr(0, 10) == "createwire") cg = new cmdCreateWire(temp);
		else if (temp.substr(0, 11) == "connectwire") cg = new cmdConnectWire(temp);
		else if (temp.substr(0, 8) == "movewire") cg = new cmdMoveWire(temp);
		else break;
		cmdList.push_back(cg);
		cg->setPointers(gCircuit, gCanvas, gateids, wireids);
	}
}

bool cmdPasteBlock::Do() {

	if (alreadyDone) {
		return true;
	}
	else {
		alreadyDone = true;
	}

	for (unsigned int i = 0; i < cmdList.size(); i++) {
		cmdList[i]->Do();
	}

	gCanvas->unselectAllGates();
	gCanvas->unselectAllWires();
	auto gateWalk = gateids.begin();
	while (gateWalk != gateids.end()) {
		(*(gCircuit->getGates()))[gateWalk->second]->select();
		gateWalk++;
	}
	auto wireWalk = wireids.begin();
	while (wireWalk != wireids.end()) {
		guiWire *wire = (*(gCircuit->getWires()))[wireWalk->second];
		if (wire != nullptr) {
			wire->select();
		}
		wireWalk++;
	}
	gCircuit->getOscope()->UpdateMenu();

	return true;
}

bool cmdPasteBlock::Undo() {

	alreadyDone = false;

	for (int i = cmdList.size() - 1; i >= 0; i--) {
		cmdList[i]->Undo();
	}

	return true;
}