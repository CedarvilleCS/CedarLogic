
#pragma once
#include "klsCommand.h"

class wxAuiNotebook;

//JV - cmdAddTab - add a new tab into canvasBook
class cmdAddTab : public klsCommand {
public:
	cmdAddTab(GUICircuit* gCircuit, wxAuiNotebook* book,
		std::vector<GUICanvas *> *canvases);

	bool Do() override;

	bool Undo() override;

private:
	wxAuiNotebook* canvasBook;
	std::vector<GUICanvas *>* canvases;
};
