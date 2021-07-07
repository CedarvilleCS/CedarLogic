#include "cmdCreateGateStruct.h"
#include <sstream>
#include "../GUICanvas.h"
#include "../guiWire.h"

// Pedro Casanova (casanova@ujaen.es) 2021/01-03
// cmdCreateGateStruct - Create a two level gate struct, wires, PLD ...
cmdCreateGateStruct::cmdCreateGateStruct(GUICanvas* gCanvas, GUICircuit* gCircuit, guiGate* gGate) :
	klsCommand(true, "Create Gate Struct") {

	this->gCanvas = gCanvas;
	this->gCircuit = gCircuit;
	this->gGate = gGate;	

	gateLibraryName = gGate->getLibraryGateName();
	gParamList = *gGate->getAllGUIParams();		
	gGate->getGLcoords(x0, y0);

}

cmdCreateGateStruct::~cmdCreateGateStruct() {

}

bool cmdCreateGateStruct::Do() {

	countGID = 0;
	countWID = 0;
		
	if (gateLibraryName == "%%_16_WIRES") {
		drawWires();
		return true;
	} 
	if (gateLibraryName == "%%_38_PLD") {
		drawPLD();
		return true;
	}
	if (gateLibraryName == "%%_31_GATES" || gateLibraryName == "%%_34_CIRCUIT") {
		drawCircuit();
		return true;
	}

	return false;
}

bool cmdCreateGateStruct::Undo() {

	for (int i = cmdList.size() - 1; i >= 0; i--) {
		cmdList[i]->Undo();
		cmdList.pop_back();
	}
	return true;
}

std::string cmdCreateGateStruct::toString() const {
	std::ostringstream oss;
	oss << "creategatestruct  ";
	return oss.str();
}

void cmdCreateGateStruct::setPointers(GUICircuit* gCircuit, GUICanvas* gCanvas,
	TranslationMap &gateids, TranslationMap &wireids) {

	this->gCircuit = gCircuit;
	this->gCanvas = gCanvas;
}

unsigned long cmdCreateGateStruct::getGID() {
	unsigned long GID;
	if (gids.size() == countGID) {
		GID = gCircuit->getNextAvailableGateID();
		gids.push_back(GID);
	}
	else
		GID = gids[countGID];
	countGID++;
	return GID;
}

unsigned long cmdCreateGateStruct::getWID() {
	unsigned long WID;
	if (wids.size() == countWID) {
		WID = gCircuit->getNextAvailableWireID();
		wids.push_back(WID);
	}
	else
		WID = wids[countWID];
	countWID++;
	return WID;
}

void cmdCreateGateStruct::drawWires() {

	unsigned long nWires = atoi(gParamList.at("NUMBER").c_str());
	float x = x0, y = y0;
	float separation = 1;
	if (gParamList.at("SEPARATION") == "narrow") separation = 0.5f;

	bool vertical = true;
	if (gParamList.at("ROTATION") == "horizontal") vertical = false;

	ostringstream type;
	type << "@@_WIRE_" << gParamList.at("LENGTH");

	for (unsigned long i = 0; i < nWires; i++)
	{
		unsigned long GID = getGID();

		cmdCreateGate* creategatecommand = new cmdCreateGate(gCircuit->gCanvas, gCircuit, GID, type.str(), x, y);
		cmdList.push_back(creategatecommand);
		creategatecommand->Do();

		if (vertical) {
			x = x - separation;
		}
		else {
			ParameterMap lParams;
			ParameterMap gParams;
			gParams["angle"] = "90";
			paramSet pSet(&gParams, &lParams);
			cmdSetParams* paramsetcommand = new cmdSetParams(gCircuit, GID, pSet);
			cmdList.push_back(paramsetcommand);
			paramsetcommand->Do();
			y = y - separation;
		}
	}

}

void cmdCreateGateStruct::drawCircuit() {

	vector <string> inputNames;
	vector <unsigned long> gates;
	vector <vector<unsigned long>> connections;
	vector <bool> inputInverter;
	bool noLinkInput = false;
	float separation = 1;
	unsigned long nInputs = 0;
	string structType = gParamList.at("STRUCT_TYPE");
	if (gateLibraryName == "%%_31_GATES") {
		for (unsigned int i = 1; i <= 16; i++) {
			ostringstream oss;
			oss << "G" << i;
			if (gParamList.at(oss.str()) != "0")
				gates.push_back(atoi(gParamList.at(oss.str()).c_str()));
		}
	}
	else {
		noLinkInput = (gParamList.at("NO_LINK_INPUT") == "true") ? true : false;
		if (gParamList.at("SEPARATION") == "narrow") separation = 0.5f;
		istringstream iss(gParamList.at("INPUT_NAMES"));

		while (true) {
			bool inverter = true;
			string inputName;
			iss >> inputName;
			if (inputName[inputName.length() - 1] == '-') {
				inputName = inputName.substr(0, inputName.length() - 1);
				inverter = false;
			}
			if (inputName != "") {
				inputNames.push_back(inputName);
				inputInverter.push_back(inverter);
				nInputs++;
			}
			if (iss.eof()) break;
		}
		for (unsigned int i = 1; i <= 16; i++) {
			ostringstream oss;
			oss << "G" << i;
			if (gParamList.at(oss.str()) != "")
			{
				vector<unsigned long> terms;
				istringstream iss(gParamList.at(oss.str()));
				unsigned long countInputs = 0;
				while (true)
				{
					string term;
					iss >> term;
					if (term != "") {
						unsigned long termcol;
						for (unsigned int i = 0; i < nInputs; i++)
							if (term.substr(0, 1) == "/") {
								if (term.substr(1) == inputNames.at(i)) {
									termcol = 2 * i + 1;
									break;
								}
							}
							else {
								if (term == inputNames.at(i)) {
									termcol = 2 * i;
									break;
								}
							}
						terms.push_back(termcol);
						countInputs++;
					}
					if (iss.eof()) break;
				}
				gates.push_back(countInputs);
				connections.push_back(terms);
			}
		}
	}

	vector <unsigned long> gateID;
	string gateIn, gateOut;
	string prefIn, prefOut;
	if (structType == "AND-OR")
	{
		gateIn = "_SAND";
		prefIn = "A";
		gateOut = "_SOR";
		prefOut = "C";
	}
	else 	if (structType == "OR-AND") {
		gateIn = "_SOR";
		prefIn = "C";
		gateOut = "_SAND";
		prefOut = "A";
	}
	else 	if (structType == "NAND-NAND") {
		gateIn = "_SNAND";
		prefIn = "B";
		gateOut = "_SNAND";
		prefOut = "B";
	}
	else 	if (structType == "NOR-NOR") {
		gateIn = "_SNOR";
		prefIn = "D";
		gateOut = "_SNOR";
		prefOut = "D";
	}

	float height = 0;
	for (unsigned int i = 0; i < gates.size(); i++)
		height = height + gates[i];

	float x, y;

	// Create first level gates
	x = x0 + 2.5f;
	y = y0 + height / 2.0f;
	for (unsigned long i = 0; i < gates.size(); i++)
	{
		y = y - gates[i] / 2.0f;

		gateID.push_back(gCircuit->getNextAvailableGateID());
		ostringstream type;
		if (gates[i] == 1)
			type << "@@_NOWIRE_3X0";
		else {
			unsigned char ch = 48 + gates[i] - 1;
			if (gates[i] > 10)
				ch = 55 + gates[i] - 1;
			type << prefIn << ch << gateIn << gates[i];
		}

		cmdCreateGate* creategatecommand = new cmdCreateGate(gCircuit->gCanvas, gCircuit, gateID[i], type.str(), x, y);
		cmdList.push_back(creategatecommand);
		creategatecommand->Do();

		y = y - gates[i] / 2.0f;
	}
	int gateoutID = gCircuit->getNextAvailableGateID();
	{
		// Create second level gate
		ostringstream type;
		if (gates.size() == 1)
			type << "@@_NOWIRE_3X0";
		else {
			unsigned char ch = 48 + (char)gates.size() - 1;
			if (gates.size() > 10)
				ch = 55 + (char)gates.size() - 1;

			type << prefOut << ch << gateOut << gates.size();
		}

		x = x + 6;
		if (gates.size() > 8)
			x = x + 4;

		cmdCreateGate* creategatecommand = new cmdCreateGate(gCircuit->gCanvas, gCircuit, gateoutID, type.str(), x, y0);
		cmdList.push_back(creategatecommand);
		creategatecommand->Do();

	}

	// Connect first level gates outputs to second level gate inputs
	GLPoint2f delta(2, 0);
	float yout, yin;
	float deltaOffset = 1.5;
	if (gates.size() > 8) deltaOffset = 3.5;
	yout = y0 + height / 2.0f;
	yin = y0 + gates.size() / 2.0;
	unsigned long i0 = 0;
	for (unsigned long i = 0; i < gates.size(); i++)
	{
		vector<IDType> wireIds;
		wireIds.push_back(getWID());

		ostringstream hotspotOut;
		if (gates[i] > 1)
			hotspotOut << "OUT";
		else
			hotspotOut << "N_IN1";
		cmdConnectWire* connectwire1 = new cmdConnectWire(gCircuit, wireIds[0], gateID[i], hotspotOut.str());

		ostringstream hotspotIn;
		if (gates.size() > 1)
			hotspotIn << "IN_" << i;
		else
			hotspotIn << "N_IN0";
		cmdConnectWire* connectwire2 = new cmdConnectWire(gCircuit, wireIds[0], gateoutID, hotspotIn.str());

		cmdCreateWire* createWire = new cmdCreateWire(gCircuit->gCanvas, gCircuit, wireIds, connectwire1, connectwire2);
		cmdList.push_back(createWire);
		createWire->Do();

		yout = yout - gates[i] / 2.0f;
		if (yout >= yin) {
			delta.x = deltaOffset - i * 0.5f;
			i0 = i;
		}
		else {
			delta.x = -deltaOffset - 0.5 + (i - i0) * 0.5f;
		}

		yout = yout - gates[i] / 2.0f;
		yin = yin - 1;

		if (delta.x != 0) {
			guiWire* gWire = gCircuit->getWires()->at(wireIds[0]);
			cmdMoveWire* movewire = new cmdMoveWire(gCircuit, wireIds[0], gWire->getSegmentMap(), delta);
			movewire->Do();
		}
	}
	if (gateLibraryName == "%%_31_GATES") return;

	// Create wires
	vector <unsigned long> wireID;
	x = x0 - 2 * nInputs * separation + 1;
	if (separation == 0.5f) x = x - 0.5f;
	y = y0 + height / 2.0f;

	ostringstream type;
	type << "@@_WIRE_" << height;

	for (unsigned long i = 0; i < 2 * nInputs; i++)
	{
		wireID.push_back(gCircuit->getNextAvailableGateID());
		cmdCreateGate* creategatecommand = new cmdCreateGate(gCircuit->gCanvas, gCircuit, wireID[i], type.str(), x + i * separation, y);
		cmdList.push_back(creategatecommand);
		creategatecommand->Do();

	}

	if (!noLinkInput) {
		vector <unsigned long> invID;
		vector <unsigned long> linkID;

		y = y + 1;
		for (unsigned long i = 0; i < nInputs; i++)
		{
			{
				// Create link
				linkID.push_back(gCircuit->getNextAvailableGateID());
				cmdCreateGate* creategatecommand = new cmdCreateGate(gCircuit->gCanvas, gCircuit, linkID[i], "DI_LINK", x + 2 * i * separation, y + 1.5f);
				cmdList.push_back(creategatecommand);
				creategatecommand->Do();
				ParameterMap lParams;
				ParameterMap gParams;
				lParams["JUNCTION_ID"] = inputNames.at(i);
				gParams["angle"] = "90";
				gParams["TEXT_HEIGHT"] = (separation == 1) ? "0.5" : "0.4";
				paramSet pSet(&gParams, &lParams);
				cmdSetParams* paramsetcommand = new cmdSetParams(gCircuit, linkID[i], pSet);
				paramsetcommand->Do();
			}
			if (inputInverter[i]) {
				{
					// Create inverter
					invID.push_back(gCircuit->getNextAvailableGateID());
					cmdCreateGate* creategatecommand = new cmdCreateGate(gCircuit->gCanvas, gCircuit, invID[i], "AF_MINI_NOT", x + 2 * i * separation + separation, y);
					cmdList.push_back(creategatecommand);
					creategatecommand->Do();
					ParameterMap lParams;
					ParameterMap gParams;
					gParams["angle"] = "270";
					paramSet pSet(&gParams, &lParams);
					cmdSetParams* paramsetcommand = new cmdSetParams(gCircuit, invID[i], pSet);
					cmdList.push_back(paramsetcommand);
					paramsetcommand->Do();
				}
				{
					// Connect inverter output to wire
					vector<IDType> wireIds;
					wireIds.push_back(getWID());

					cmdConnectWire* connectwire1 = new cmdConnectWire(gCircuit, wireIds[0], invID[i], "OUT_0");
					cmdConnectWire* connectwire2 = new cmdConnectWire(gCircuit, wireIds[0], wireID[2 * i + 1], "N_IN0");

					cmdCreateWire* createWire = new cmdCreateWire(gCircuit->gCanvas, gCircuit, wireIds, connectwire1, connectwire2);
					cmdList.push_back(createWire);
					createWire->Do();
				}
			}
			else {
				// Create not link
				unsigned long linknotID = gCircuit->getNextAvailableGateID();
				cmdCreateGate* creategatecommand = new cmdCreateGate(gCircuit->gCanvas, gCircuit, linknotID, "DI_LINK", x + 2 * i * separation + separation, y + 1.5f);
				cmdList.push_back(creategatecommand);
				creategatecommand->Do();
				ParameterMap lParams;
				ParameterMap gParams;
				lParams["JUNCTION_ID"] = "/" + inputNames.at(i);
				gParams["angle"] = "90";
				gParams["TEXT_HEIGHT"] = (separation == 1) ? "0.5" : "0.4";
				paramSet pSet(&gParams, &lParams);
				cmdSetParams* paramsetcommand = new cmdSetParams(gCircuit, linknotID, pSet);
				paramsetcommand->Do();

				// Connect link to wire
				vector<IDType> wireIds;
				wireIds.push_back(getWID());

				cmdConnectWire* connectwire1 = new cmdConnectWire(gCircuit, wireIds[0], linknotID, "IN_0");
				cmdConnectWire* connectwire2 = new cmdConnectWire(gCircuit, wireIds[0], wireID[2 * i + 1], "N_IN0");

				cmdCreateWire* createWire = new cmdCreateWire(gCircuit->gCanvas, gCircuit, wireIds, connectwire1, connectwire2);
				cmdList.push_back(createWire);
				createWire->Do();
			}
			{
				// Connect link to wire
				vector<IDType> wireIds;
				wireIds.push_back(getWID());

				cmdConnectWire* connectwire1 = new cmdConnectWire(gCircuit, wireIds[0], linkID[i], "IN_0");
				cmdConnectWire* connectwire2 = new cmdConnectWire(gCircuit, wireIds[0], wireID[2 * i], "N_IN0");

				cmdCreateWire* createWire = new cmdCreateWire(gCircuit->gCanvas, gCircuit, wireIds, connectwire1, connectwire2);
				cmdList.push_back(createWire);
				createWire->Do();

				if (inputInverter[i]) {
					// Connect inverter input to wire
					cmdConnectWire* connectwire = new cmdConnectWire(gCircuit, wireIds[0], invID[i], "IN_0");
					cmdList.push_back(connectwire);
					connectwire->Do();
				}
			}
		}
	}

	if (gParamList.at("OUTPUT_NAME") != "") {
		// Create output link
		x = x0 + 11;
		if (gates.size() > 8) x = x + 4;
		y = y0;
		unsigned long GID = gCircuit->getNextAvailableGateID();
		cmdCreateGate* creategatecommand = new cmdCreateGate(gCircuit->gCanvas, gCircuit, GID, "DI_LINK", x, y);
		cmdList.push_back(creategatecommand);
		creategatecommand->Do();
		ParameterMap lParams;
		ParameterMap gParams;
		lParams["JUNCTION_ID"] = gParamList.at("OUTPUT_NAME");
		gParams["TEXT_HEIGHT"] = (separation == 1) ? "0.5" : "0.4";
		paramSet pSet(&gParams, &lParams);
		cmdSetParams* paramsetcommand = new cmdSetParams(gCircuit, GID, pSet);
		paramsetcommand->Do();

		ostringstream hotspotOut;
		if (gates.size() > 1)
			hotspotOut << "OUT";
		else
			hotspotOut << "N_IN1";

		vector<IDType> wireIds;
		wireIds.push_back(getWID());

		cmdConnectWire* connectwire1 = new cmdConnectWire(gCircuit, wireIds[0], gateoutID, hotspotOut.str());
		cmdConnectWire* connectwire2 = new cmdConnectWire(gCircuit, wireIds[0], GID, "IN_0");

		cmdCreateWire* createWire = new cmdCreateWire(gCircuit->gCanvas, gCircuit, wireIds, connectwire1, connectwire2);
		cmdList.push_back(createWire);
		createWire->Do();
	}

	// Connect first level gate inputs to wires
	unsigned long pos = 0;
	for (unsigned int i = 0; i < connections.size(); i++)
	{
		vector<unsigned long> conns = connections.at(i);
		unsigned int posGate = 0;
		for (unsigned int j = 0; j < conns.size(); j++) {
			unsigned long wire = conns.at(j);
			vector<IDType> wireIds;
			wireIds.push_back(getWID());

			ostringstream hotspotgate;
			if (conns.size() == 1)
				hotspotgate << "N_IN0";
			else
				hotspotgate << "IN_" << posGate;

			ostringstream hotspotwire;
			hotspotwire << "N_IN" << 1 + 2 * pos;

			cmdConnectWire* connectwire1 = new cmdConnectWire(gCircuit, wireIds[0], gateID[i], hotspotgate.str());
			cmdConnectWire* connectwire2 = new cmdConnectWire(gCircuit, wireIds[0], wireID[wire], hotspotwire.str());

			cmdCreateWire* createWire = new cmdCreateWire(gCircuit->gCanvas, gCircuit, wireIds, connectwire1, connectwire2);
			cmdList.push_back(createWire);
			createWire->Do();
			pos++;
			posGate++;
		}
	}
}

void cmdCreateGateStruct::drawPLD() {

	string PLDType = gParamList.at("PLD_TYPE");
	unsigned long inBits = atoi(gParamList.at("INPUT_BITS").c_str());
	unsigned long outBits = atoi(gParamList.at("OUTPUT_BITS").c_str());
	unsigned long inORBits = pow(2, inBits);
	if (PLDType != "PROM")
		inORBits = atoi(gParamList.at("OR_INPUTS").c_str());
	float x, y;
	string connections = gParamList.at("CONNECTIONS");

	vector <unsigned long> notBufID;
	vector <unsigned long> andID;
	vector <unsigned long> orID;
	vector <unsigned long> wireAndID;
	vector <unsigned long> wireOrID;
	x = x0;
	y = y0;
	for (unsigned long i = 0; i < inBits; i++) {
		unsigned long GID = getGID();
		notBufID.push_back(GID);
		cmdCreateGate* creategatecommand = new cmdCreateGate(gCircuit->gCanvas, gCircuit, GID, "AO_MINI_NOTBUF", x, y);
		cmdList.push_back(creategatecommand);
		creategatecommand->Do();
		ParameterMap gParams;
		gParams["mirror"] = "true";
		paramSet pSet(&gParams, NULL);
		cmdSetParams* paramsetcommand = new cmdSetParams(gCircuit, GID, pSet);
		paramsetcommand->Do();
		y = y - 2;
	}

	if (PLDType == "PAL")
		x = x0 + inORBits * outBits + 0.5;
	else
		x = x0 + inORBits + 0.5;
	y = y0 + 0.5;
	for (unsigned long i = 0; i < 2 * inBits; i++) {
		unsigned long GID = getGID();
		cmdCreateGate* creategatecommand = new cmdCreateGate(gCircuit->gCanvas, gCircuit, GID, "@@_WIRE_0", x, y);
		cmdList.push_back(creategatecommand);
		creategatecommand->Do();
		ParameterMap gParams;
		gParams["angle"] = "90";
		paramSet pSet(&gParams, NULL);
		cmdSetParams* paramsetcommand = new cmdSetParams(gCircuit, GID, pSet);
		paramsetcommand->Do();

		vector<IDType> wireIds;
		wireIds.push_back(getWID());
		wireAndID.push_back(wireIds[0]);
		ostringstream hotspot;
		if (i / 2.0f == i / 2)
			hotspot << "OUTINV_0";
		else
			hotspot << "OUT_0";

		cmdConnectWire* connectwire1 = new cmdConnectWire(gCircuit, wireIds[0], notBufID[i / 2], hotspot.str());
		cmdConnectWire* connectwire2 = new cmdConnectWire(gCircuit, wireIds[0], GID, "N_IN0");

		cmdCreateWire* createWire = new cmdCreateWire(gCircuit->gCanvas, gCircuit, wireIds, connectwire1, connectwire2);
		cmdList.push_back(createWire);
		createWire->Do();

		y = y - 1;
	}
	x = x0 + 1;
	y = y0 - 2 * inBits + 0.5;
	unsigned long nAnd;
	if (PLDType == "PAL")
		nAnd = inORBits * outBits;
	else
		nAnd = inORBits;
	for (unsigned long i = 0; i < nAnd; i++) {
		unsigned long GID = getGID();
		andID.push_back(GID);
		ostringstream type;
		type << "@@_LAND_" << 2 * inBits;
		cmdCreateGate* creategatecommand = new cmdCreateGate(gCircuit->gCanvas, gCircuit, GID, type.str(), x, y);
		cmdList.push_back(creategatecommand);
		creategatecommand->Do();
		ParameterMap gParams;
		if (PLDType == "PROM")
			gParams["CROSS_JUNCTION"] = "false";
		gParams["angle"] = "270";
		paramSet pSet(&gParams, NULL);
		cmdSetParams* paramsetcommand = new cmdSetParams(gCircuit, GID, pSet);
		paramsetcommand->Do();
		x = x + 1;
	}
	x = x0 + 1;
	y = y0 - 2 * inBits - outBits - 0.5;
	for (unsigned long i = 0; i < nAnd; i++) {
		unsigned long GID = getGID();
		cmdCreateGate* creategatecommand = new cmdCreateGate(gCircuit->gCanvas, gCircuit, GID, "@@_WIRE_0", x, y);
		cmdList.push_back(creategatecommand);
		creategatecommand->Do();

		vector<IDType> wireIds;
		wireIds.push_back(getWID());
		wireOrID.push_back(wireIds[0]);
		cmdConnectWire* connectwire1 = new cmdConnectWire(gCircuit, wireIds[0], andID[i], "OUT");
		cmdConnectWire* connectwire2 = new cmdConnectWire(gCircuit, wireIds[0], GID, "N_IN0");

		cmdCreateWire* createWire = new cmdCreateWire(gCircuit->gCanvas, gCircuit, wireIds, connectwire1, connectwire2);
		cmdList.push_back(createWire);
		createWire->Do();

		x = x + 1;
	}
	x = x0 + nAnd + 1;
	y = y0 - 2 * inBits - 1;
	for (unsigned long i = 0; i < outBits; i++) {
		unsigned long GID = getGID();
		orID.push_back(GID);
		ostringstream type;
		type << "@@_LOR_" << nAnd;
		cmdCreateGate* creategatecommand = new cmdCreateGate(gCircuit->gCanvas, gCircuit, GID, type.str(), x, y);
		cmdList.push_back(creategatecommand);
		creategatecommand->Do();
		if (PLDType != "PAL") {
			ParameterMap gParams;
			gParams["CROSS_JUNCTION"] = "true";
			paramSet pSet(&gParams, NULL);
			cmdSetParams* paramsetcommand = new cmdSetParams(gCircuit, GID, pSet);
			paramsetcommand->Do();
		}
		y = y - 1;
	}

	if (PLDType == "PROM")
		for (unsigned int i = 0; i < pow(2, inBits); i++) {
			unsigned int val = i;
			for (unsigned int j = inBits; j > 0; j--) {
				if (val >= pow(2, j - 1)) {
					val -= pow(2, j - 1);
					ostringstream hotspot;
					hotspot << "IN_" << 2 * inBits - 1 - 2 * (j - 1) - 1;

					cmdConnectWire *connectWire = new cmdConnectWire(gCircuit, wireAndID[2 * (j - 1) + 1], andID[i], hotspot.str());
					cmdList.push_back(connectWire);
					connectWire->Do();
				}
				else {
					ostringstream hotspot;
					hotspot << "IN_" << 2 * inBits - 1 - 2 * (j - 1);

					cmdConnectWire *connectWire = new cmdConnectWire(gCircuit, wireAndID[2 * (j - 1)], andID[i], hotspot.str());
					cmdList.push_back(connectWire);
					connectWire->Do();
				}
			}
		}
	else if (PLDType == "PAL")
		for (unsigned int i = 0; i < outBits; i++) {
			for (unsigned int j = 0; j < inORBits; j++) {
				ostringstream hotspot;
				hotspot << "IN_" << inORBits * i + j;

				cmdConnectWire *connectWire = new cmdConnectWire(gCircuit, wireOrID[nAnd - inORBits * i - j - 1], orID[i], hotspot.str());
				cmdList.push_back(connectWire);
				connectWire->Do();
			}
		}
	if (connections != "")
	{
		if (PLDType == "PROM") {
			if (connections.length() == pow(2, inBits)*outBits) {
				for (unsigned int i = 0; i < outBits; i++)
					for (unsigned int j = 0; j < pow(2, inBits); j++)
						if (connections.substr(i*pow(2, inBits) + j, 1) == "1") {
							ostringstream hotspot;
							hotspot << "IN_" << nAnd - j - 1;
							cmdConnectWire *connectWire = new cmdConnectWire(gCircuit, wireOrID[j], orID[i], hotspot.str());
							cmdList.push_back(connectWire);
							connectWire->Do();
						}
			}
		}
		else if (PLDType == "PAL") {
			if (connections.length() == 2 * inBits * nAnd) {
				for (unsigned int i = 0; i < nAnd; i++)
					for (unsigned int j = 0; j < 2 * inBits; j++)
						if (connections.substr(i * 2 * inBits + j, 1) == "1") {
							ostringstream hotspot;
							hotspot << "IN_" << 2 * inBits - j - 1;
							cmdConnectWire *connectWire = new cmdConnectWire(gCircuit, wireAndID[j], andID[i], hotspot.str());
							cmdList.push_back(connectWire);
							connectWire->Do();
						}
			}
		}
		else {
			if (connections.length() == 2 * inBits * nAnd + outBits * nAnd) {
				for (unsigned int i = 0; i < nAnd; i++)
					for (unsigned int j = 0; j < 2 * inBits; j++)
						if (connections.substr(i * 2 * inBits + j, 1) == "1") {
							ostringstream hotspot;
							hotspot << "IN_" << 2 * inBits - j - 1;
							cmdConnectWire *connectWire = new cmdConnectWire(gCircuit, wireAndID[j], andID[i], hotspot.str());
							cmdList.push_back(connectWire);
							connectWire->Do();
						}
				for (unsigned int i = 0; i < outBits; i++)
					for (unsigned int j = 0; j < nAnd; j++)
						if (connections.substr(2 * inBits * nAnd + i * nAnd + j, 1) == "1") {
							ostringstream hotspot;
							hotspot << "IN_" << nAnd - j - 1;
							cmdConnectWire *connectWire = new cmdConnectWire(gCircuit, wireOrID[j], orID[i], hotspot.str());
							cmdList.push_back(connectWire);
							connectWire->Do();
						}
			}
		}
	}

}