#include "cmdCreateGateStruct.h"
#include <sstream>
#include "../GUICanvas.h"
#include "../guiWire.h"

// Pedro Casanova (casanova@ujaen.es) 2021/01-02
// cmdCreateGateStruct - Create a two level gate struct and others
cmdCreateGateStruct::cmdCreateGateStruct(GUICanvas* gCanvas, GUICircuit* gCircuit, guiGate* gGate) :
	klsCommand(false, "Create Gate Struct") {

	this->gCanvas = gCanvas;
	this->gCircuit = gCircuit;
	this->gGate = gGate;	

}

cmdCreateGateStruct::~cmdCreateGateStruct() {

}

bool cmdCreateGateStruct::Do() {

	map<string, string> gParamList = *gGate->getAllGUIParams();

	if (gGate->getLibraryGateName() == "%_16_WIRES") {
		map<string, string> gParamList = *gGate->getAllGUIParams();

		unsigned long nWires = atoi(gParamList.at("NUMBER").c_str());

		float separation = 1;
		if (gParamList.at("SEPARATION") == "narrow") separation = 0.5f;

		bool vertical = true;
		if (gParamList.at("ROTATION") == "horizontal") vertical = false;

		float x, y;
		gGate->getGLcoords(x, y);

		ostringstream type;
		type << "@@_WIRE_" << gParamList.at("LENGTH");

		for (unsigned long i = 0; i < nWires; i++)
		{
			int newGID = gCircuit->getNextAvailableGateID();
			cmdCreateGate* creategatecommand = new cmdCreateGate(gCircuit->gCanvas, gCircuit, newGID, type.str(), x, y);
			cmdList.push_back(creategatecommand);
			creategatecommand->Do();
			cmdSetParams setgateparams(gCircuit, newGID, paramSet((*(gCircuit->getGates()))[newGID]->getAllGUIParams(), (*(gCircuit->getGates()))[newGID]->getAllLogicParams()));
			setgateparams.Do();


			if (vertical) {
				x = x - separation;
			} else {
				ParameterMap lParams;
				ParameterMap gParams;
				gParams["angle"] = "90";
				paramSet pSet(&gParams, &lParams);
				cmdSetParams* paramsetcommand = new cmdSetParams(gCircuit, newGID, pSet);
				cmdList.push_back(paramsetcommand);
				paramsetcommand->Do();
				y = y - separation;
			}
		}
		return true;
	}

	vector <string> inputNames;
	vector <unsigned long> gates;	
	vector <vector<unsigned long>> connections;	
	vector <bool> inputInverter;
	bool noLinkInverter = false;
	float separation = 1;
	unsigned long nInputs = 0;	
	string structType = gParamList.at("STRUCT_TYPE");
	if (gGate->getLibraryGateName() == "%_31_GATES") {
		for (unsigned int i = 1; i <= 8; i++) {
			ostringstream oss;
			oss << "G" << i;
			if (gParamList.at(oss.str()) != "0")
				gates.push_back(atoi(gParamList.at(oss.str()).c_str()));
		}
	}
	else {
		noLinkInverter = (gParamList.at("NO_LINK_INVERTER") == "true") ? true : false;
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
		for (unsigned int i = 1; i <= 8; i++) {
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

	vector <int> gateID;
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
	GLfloat x0, y0;
	gGate->getGLcoords(x0, y0);
	float x, y;

	// Create first level gates
	x = x0 + 2.5f;
	y = y0 + height / 2.0f;
	for (unsigned long i = 0; i < gates.size(); i++)
	{
		y = y - gates[i] / 2.0f;

		gateID.push_back(gCircuit->getNextAvailableGateID());
		ostringstream type;
		if (gates[i] > 1)
			type << prefIn << gates[i] - 1 << gateIn << gates[i];
		else
			type << "@@_NOWIRE_3X0";

		cmdCreateGate* creategatecommand = new cmdCreateGate(gCircuit->gCanvas, gCircuit, gateID[i], type.str(), x, y);
		cmdList.push_back(creategatecommand);
		creategatecommand->Do();
		cmdSetParams setgateparams(gCircuit, gateID[i], paramSet((*(gCircuit->getGates()))[gateID[i]]->getAllGUIParams(), (*(gCircuit->getGates()))[gateID[i]]->getAllLogicParams()));
		setgateparams.Do();


		y = y - gates[i] / 2.0f;
	}
	int gateoutID = gCircuit->getNextAvailableGateID();
	{
		// Create second level gate
		ostringstream type;
		if (gates.size() > 1)
			type << prefOut << gates.size() - 1 << gateOut << gates.size();
		else
			type << "@@_NOWIRE_3X0";

		x = x + 7;
		cmdCreateGate* creategatecommand = new cmdCreateGate(gCircuit->gCanvas, gCircuit, gateoutID, type.str(), x, y0);
		cmdList.push_back(creategatecommand);
		creategatecommand->Do();
		cmdSetParams setgateparams(gCircuit, gateoutID, paramSet((*(gCircuit->getGates()))[gateoutID]->getAllGUIParams(), (*(gCircuit->getGates()))[gateoutID]->getAllLogicParams()));
		setgateparams.Do();

	}

	// Connect first level gates outputs to second level gate inputs
	GLPoint2f delta(2, 0);
	float yout, yin;
	yout = y0 + height / 2.0f;
	yin = y0 + gates.size() / 2.0;
	unsigned long i0 = 0;
	for (unsigned long i = 0; i < gates.size(); i++)
	{
		vector<IDType> wireIds;
		wireIds.push_back(gCircuit->getNextAvailableWireID());

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
			delta.x = 2 - i * 0.5f;
			i0 = i;
		} else {
			delta.x = -2.5 + (i - i0) * 0.5f;
		}

		yout = yout - gates[i] / 2.0f;
		yin = yin - 1;

		if (delta.x != 0) {
			guiWire* gWire = gCircuit->getWires()->at(wireIds[0]);
			cmdMoveWire* movewire = new cmdMoveWire(gCircuit, wireIds[0], gWire->getSegmentMap(), delta);
			movewire->Do();
		}
	}
	if (gGate->getLibraryGateName() == "%_31_GATES") return true;

	// Create wires
	vector <int> wireID;
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
		cmdSetParams setgateparams(gCircuit, wireID[i], paramSet((*(gCircuit->getGates()))[wireID[i]]->getAllGUIParams(), (*(gCircuit->getGates()))[wireID[i]]->getAllLogicParams()));
		setgateparams.Do();

	}

	if (!noLinkInverter) {
		vector <int> invID;
		vector <int> linkID;

		y = y + 1;
		for (unsigned long i = 0; i < nInputs; i++)
		{
			{
				// Create link
				linkID.push_back(gCircuit->getNextAvailableGateID());
				cmdCreateGate* creategatecommand = new cmdCreateGate(gCircuit->gCanvas, gCircuit, linkID[i], "DI_LINK", x + 2 * i * separation, y + 1.5f);
				cmdList.push_back(creategatecommand);
				creategatecommand->Do();
				cmdSetParams setgateparams(gCircuit, linkID[i], paramSet((*(gCircuit->getGates()))[linkID[i]]->getAllGUIParams(), (*(gCircuit->getGates()))[linkID[i]]->getAllLogicParams()));
				setgateparams.Do();
				ParameterMap lParams;
				ParameterMap gParams;
				lParams["JUNCTION_ID"] = inputNames.at(i);
				gParams["angle"] = "90";
				gParams["TEXT_HEIGHT"] = (separation == 1) ? "0.5" :"0.4";
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
					cmdSetParams setgateparams(gCircuit, invID[i], paramSet((*(gCircuit->getGates()))[invID[i]]->getAllGUIParams(), (*(gCircuit->getGates()))[invID[i]]->getAllLogicParams()));
					setgateparams.Do();
					gCircuit->getGates()->at(invID[i])->setGUIParam("angle", "270");
				}
				{
					// Connect inverter output to wire
					vector<IDType> wireIds;
					wireIds.push_back(gCircuit->getNextAvailableWireID());

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
				cmdSetParams setgateparams(gCircuit, linknotID, paramSet((*(gCircuit->getGates()))[linknotID]->getAllGUIParams(), (*(gCircuit->getGates()))[linknotID]->getAllLogicParams()));
				setgateparams.Do();
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
				wireIds.push_back(gCircuit->getNextAvailableWireID());

				cmdConnectWire* connectwire1 = new cmdConnectWire(gCircuit, wireIds[0], linknotID, "IN_0");
				cmdConnectWire* connectwire2 = new cmdConnectWire(gCircuit, wireIds[0], wireID[2 * i + 1], "N_IN0");

				cmdCreateWire* createWire = new cmdCreateWire(gCircuit->gCanvas, gCircuit, wireIds, connectwire1, connectwire2);
				cmdList.push_back(createWire);
				createWire->Do();
			}
			{
				// Connect link to wire
				vector<IDType> wireIds;
				wireIds.push_back(gCircuit->getNextAvailableWireID());

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

	// Connect first level gate inputs to wires
	unsigned long pos = 0;
	for (unsigned int i = 0; i < connections.size(); i++)
	{
		vector<unsigned long> conns = connections.at(i);
		unsigned int posGate = 0;
		for (unsigned int j = 0; j < conns.size(); j++) {
			unsigned long wire = conns.at(j);
			vector<IDType> wireIds;
			wireIds.push_back(gCircuit->getNextAvailableWireID());

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
	return true;
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