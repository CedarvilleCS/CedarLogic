
#pragma once
#include <vector>
#include "common.h"
using namespace std;

class klsCommand;
class GUICircuit;
class GUICanvas;

class klsClipboard {
public:
	static klsCommand * pasteBlock(GUICircuit* gCircuit, GUICanvas* gCanvas);

	static void copyBlock(GUICircuit* gCircuit, GUICanvas* gCanvas,
			const vector<IDType> &gates, const vector<IDType> &wires);

	static std::string getCopyText(GUICircuit* gCircuit, GUICanvas* gCanvas,
		const vector<IDType> &gates, const vector<IDType> &wires);
};
