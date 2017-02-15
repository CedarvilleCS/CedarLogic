/*****************************************************************************
Project: CEDAR Logic Simulator
Added by Colin Broberg, 10/20/16
Purpose: A class to dynamically create gates for the purpose of heirarchy
*****************************************************************************/

#include "DynamicGate.h"
#include "MainApp.h"
#include "commands.h"
#include <string>

DECLARE_APP(MainApp)

// Create a new DynamicGate, doing so automatically createst the visual appearance, adds to library, and places in canvas
DynamicGate::DynamicGate(GUICanvas* canvas, GUICircuit* circuit, unsigned long gid, int in, int xLoc, int yLoc, string lType,
						 string gName, string gCaption, string guiType) {
	inputs = in;
	outputs = 1;
	x = xLoc;
	y = yLoc;
	name = gName;
	caption = gCaption;
	libGate.gateName = "Dynamic_Gate";
	libGate.caption = "This is a dynamic gate.";
	libGate.guiType = "";
	libGate.logicType = lType;

	calculateShape();
	calculateParams();
	addGateToLibrary();
	createGate(canvas, circuit, gid, x, y);
}

// Calculates the shape of the gate based upon number of inputs and outputs
void DynamicGate::calculateShape() {
	float height = inputs + 3;
	if (height < 5) { height = MIN_HEIGHT; }	// Default height of 5
	float width = inputs;
	if (width < 4) { width = MIN_WIDTH; }	// Default width of 4

	// Simple math vals to make calculations nicer looking
	float startX = (float)(-width / 2);
	float startY = (float)(-height / 2);

	// Create main shape
	libGate.shape.push_back(LibraryGateLine(startX, startY, startX, -startY));
	libGate.shape.push_back(LibraryGateLine(startX, startY, -startX, startY));
	libGate.shape.push_back(LibraryGateLine(-startX, startY, -startX, -startY));
	libGate.shape.push_back(LibraryGateLine(-startX, -startY, startX, -startY));

	// Update math vals
	startX = (float)(-width / 2);
	startY = (float)(-inputs / 2);

	// Add inputs
	for (int i = 0; i < inputs; i++) {
		libGate.shape.push_back(LibraryGateLine(startX - 1, startY + i, startX, startY + i));

		LibraryGateHotspot hotspot;
		hotspot.name = "IN_" + std::to_string(i);
		hotspot.isInput = true;
		hotspot.x = startX - 1;
		hotspot.y = startY + 1;
		hotspot.isInverted = false;
		hotspot.logicEInput = "";
		hotspot.busLines = 1;

		libGate.hotspots.push_back(hotspot);
	}

	// Add outputs
	startX = (float)(width / 2);
	startY = (float)(-outputs / 2);

	for (int i = 0; i < outputs; i++) {
		libGate.shape.push_back(LibraryGateLine(startX + 1, startY + i, startX, startY + i));

		LibraryGateHotspot hotspot;
		hotspot.name = "OUT_" + std::to_string(i);
		hotspot.isInput = false;
		hotspot.x = startX + 1;
		hotspot.y = startY + 1;
		hotspot.isInverted = false;
		hotspot.logicEInput = "";
		hotspot.busLines = 1;

		libGate.hotspots.push_back(hotspot);
	}
}

// Insert params
void DynamicGate::calculateParams() {
	libGate.logicParams["INPUT_BITS"] = to_string(inputs);
}

// Add gate to library -- duh
void DynamicGate::addGateToLibrary() {
	wxGetApp().gateNameToLibrary[libGate.gateName] = libName;
	wxGetApp().libraries[libName][libGate.gateName] = libGate;
	wxGetApp().libParser.addGate(libName, libGate);
}

// Creates gate with command
void DynamicGate::createGate(GUICanvas* canvas, GUICircuit* circuit, unsigned long gid, float xLoc, float yLoc) {
	cmdCreateGate * createGate = new cmdCreateGate(canvas, circuit, gid, libGate.gateName, xLoc, yLoc);
	createGate->Do();
}