
#include "cmdDeleteTab.h"
#include "wx/aui/auibook.h"
#include "../widget/GUICanvas.h"
#include "cmdDeleteSelection.h"

class guiGate;
class guiWire;

cmdDeleteTab::cmdDeleteTab(GUICircuit* gCircuit, GUICanvas* gCanvas,
	wxAuiNotebook* book, std::vector< GUICanvas* >* canvases,
	unsigned long ID) :
	klsCommand(true, "Delete Tab") {

	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
	this->canvasBook = book;
	this->canvases = canvases;
	this->canvasID = ID;


	std::unordered_map< unsigned long, guiGate* >* gateList = gCanvas->getGateList();
	auto thisGate = gateList->begin();
	while (thisGate != gateList->end()) {
		this->gates.push_back(thisGate->first);
		thisGate++;
	}
	std::unordered_map< unsigned long, guiWire* >* wireList = gCanvas->getWireList();
	auto thisWire = wireList->begin();
	while (thisWire != wireList->end()) {
		this->wires.push_back(thisWire->first);
		thisWire++;
	}
}

cmdDeleteTab::~cmdDeleteTab() {
	while (!(cmdList.empty())) {
		cmdList.pop();
	}
}

bool cmdDeleteTab::Do() {
	cmdList.push(new cmdDeleteSelection(gCircuit, gCanvas, gates, wires));
	cmdList.top()->Do();

	unsigned int canSize = canvases->size();
	//canvases->erase(canvases->begin() + canvasID);
	remove(canvases->begin(), canvases->end(), gCanvas);
	canvases->pop_back();
	if (canvasID < (canSize - 1)) {
		for (unsigned int i = canvasID; i < canSize; i++) {
			std::string text = "Page " + to_string(i);
			canvasBook->SetPageText(i, text);
		}
	}
	canvasBook->RemovePage(canvasID);
	//TODO fix canvases not refreshing
	gCanvas->Hide();
	return true;
}
bool cmdDeleteTab::Undo() {
	unsigned int canSize = canvases->size();
	canvases->insert(canvases->begin() + canvasID, gCanvas);
	std::ostringstream oss;
	oss << "Page " << canvasID + 1;
	canvasBook->InsertPage(canvasID, gCanvas, oss.str(), (false));
	if (canvasID < (canSize)) {
		for (unsigned int i = canvasID + 1; i < canSize + 1; i++) {
			std::string text = "Page " + to_string(i + 1);
			canvasBook->SetPageText(i, text);
		}
	}
	while (!(cmdList.empty())) {
		cmdList.top()->Undo();
		cmdList.pop();
	}
	return true;
}