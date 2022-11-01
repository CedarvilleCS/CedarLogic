
#pragma once
#include "klsCommand.h"
#include <stack>
#include <vector>

class wxAuiNotebook;

//JV - cmdDeleteTab - delete a tab from canvasBook
class cmdDeleteTab : public klsCommand {
public:
	cmdDeleteTab(GUICircuit* gCircuit, GUICanvas* gCanvas, wxAuiNotebook* book,
		std::vector<GUICanvas *> *canvases, unsigned long ID);

	virtual ~cmdDeleteTab();

	bool Do();

	bool Undo();

protected:
	std::vector < unsigned long > gates;
	std::vector < unsigned long > wires;
	std::stack < klsCommand* > cmdList;
	wxAuiNotebook* canvasBook;
	std::vector< GUICanvas* >* canvases;
	unsigned long canvasID;

};