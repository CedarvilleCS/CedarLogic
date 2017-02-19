/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   CircuitParse: uses XMLParser to load and save user circuit files.
*****************************************************************************/

#include "CircuitParse.h"
#include "../frame/OscopeFrame.h"
#include "../MainApp.h"
#include <fstream>
#include <sstream>

#include "XMLParser.h"
#include "../gate/guiGate.h"
#include "../wire/guiWire.h"
#include "../GUICircuit.h"
#include "../widget/GUICanvas.h"
#include <map>
#include <unordered_map>
#include "../../version.h"
#include "../command/cmdConnectWire.h"

DECLARE_APP(MainApp)

CircuitParse::CircuitParse(GUICanvas* glc) {
	// this constructor did not initialiize all its data members, I corrected that
	// note:  gCanvases and fileName are initialized by base class default constructors   KAS
	mParse = nullptr;
	gCanvas = glc;
}

CircuitParse::CircuitParse(string fileName, vector< GUICanvas* > glc) {
	gCanvases = glc;
	gCanvas = glc[0];

	fstream x(fileName.c_str(), ios::in);
	mParse = new XMLParser(&x, false);
	this->fileName = fileName;
}

CircuitParse::~CircuitParse() {
	delete mParse;
}

void CircuitParse::loadFile(string fileName) {
	fstream x(fileName.c_str(), ios::in);
	mParse = new XMLParser(&x, false);
	this->fileName = fileName;
}

vector<GUICanvas*> CircuitParse::parseFile() {

	string firstTag = mParse->readTag();

	// Cedar Logic 2.0 and up has version information to keep old versions
	// of Cedar Logic from opening new, incompatable files.
	if (firstTag == "version") {
		std::string versionNumber = mParse->readTagValue("version");
		if (versionNumber > VERSION_NUMBER_STRING()) {

			//show error message!!! And quit.
			wxMessageBox("This file was made with a newer version of Cedar Logic. "
				"Go to 'Help\\Download Latest Version...' to open this file."
				"Close CedarLogic without saving to avoid overwriting your work!!!", "Version Error!");

			return gCanvases;
		}
		mParse->readCloseTag();
		firstTag = mParse->readTag();
	}
	
	// need to throw exception
	if (firstTag != "circuit") return gCanvases;
	
	// Read the currentPage tag.
	if( mParse->readTag() == "CurrentPage" ) {
		string currentPage = mParse->readTagValue( "CurrentPage" );
		mParse->readCloseTag();
	}
	
	do { // while next tag is not close circuit
		string temp = mParse->readTag();
		char pageNum = temp[temp.size()-1] - '0';
		if ((int)pageNum != 0) {
			gCanvas = new GUICanvas(gCanvases[0]->GetParent(), gCanvases[0]->getCircuit(), wxID_ANY, wxDefaultPosition, gCanvases[0]->GetSize(), wxWANTS_CHARS);
			gCanvases.push_back(gCanvas);
		}
		else {
			gCanvas = gCanvases[(int)pageNum];			
		}
		
		string pageTag = temp;
		// while next tag is not close page
 		while (!mParse->isCloseTag(mParse->getCurrentIndex())) {
 			temp = mParse->readTag();
 			
 			if( temp == "PageViewport" ) {
	 			// Read the last page viewport:
				string pageView = mParse->readTagValue( "PageViewport" );
				
				// Set the page viewport:
				istringstream iss(pageView);
				GLPoint2f topLeft( 0, 0 );
				GLPoint2f bottomRight( 50, 50 );
				char dump;
				iss >> topLeft.x >> dump >> topLeft.y >> dump >> bottomRight.x >> dump >> bottomRight.y;
				gCanvas->setViewport(topLeft, bottomRight);

				mParse->readCloseTag();
			}
			else if (temp == "gate") {
				string type, ID, position;
				vector < gateConnector > inputs, outputs;
				vector < parameter > params;
				gateConnector* gc;
				parameter* pParam;
				do { // get full gate structure
					temp = mParse->readTag(); // get tag
					if (temp == "ID") { // get ID
						ID = mParse->readTagValue(temp);
					} else if (temp == "type") { // get type
						type = mParse->readTagValue(temp);
						
						//***********************************
						//Edit by Joshua Lansford 4/4/07
						//We have eliminated a couple of gate
						//types.
						//Opening a file with an outdated
						//ram file will crash the system.
						//I don't think it does so with
						//the outdated flip-flops, anyways,
						//this bit of code will change the
						//gate type of the outdated gate
						//to a new gate type that is supported
						//without crashing the program.
						if( type == "AM_RAM_16x16_Single_Port" ){
							//there is no different between these two types.
							//the AM_RAM_16x16_Single_Port was an experiment
							//before we converted all the gates.
							//Thus no warning needs to be given.
							type = "AM_RAM_16x16";
						}else if( type == "AA_DFF" ){
							wxMessageBox("The High Active Reset D flip flop has been deprecated.  Automatically replacing with a Low active version", "Old gate", wxOK | wxICON_ASTERISK, NULL);
							type = "AE_DFF_LOW";
						}else if( type == "BA_JKFF" ){
							wxMessageBox("The High Active Reset JK flip flop has been deprecated.  Automatically replacing with a Low active version", "Old gate", wxOK | wxICON_ASTERISK, NULL);
							type = "BE_JKFF_LOW";
						}else if( type == "BA_JKFF_NT" ){
							wxMessageBox("The High Active Reset negitive triggered JK flip flop has been deprecated.  Automatically replacing with a Low active version", "Old gate", wxOK | wxICON_ASTERISK, NULL);
							type = "BE_JKFF_LOW_NT";
						}
						//**********************************
						
					} else if (temp == "position") { // get position
						position = mParse->readTagValue(temp);
					} else if (temp == "input") { // get input
						temp = mParse->readTag(); // get input ID
						gc = new gateConnector();
						gc->connectionID = mParse->readTagValue(temp);
						mParse->readCloseTag();

						istringstream iss(mParse->readTagValue("input"));
						IDType tempId;
						while (iss >> tempId) {
							gc->wireIds.push_back(tempId);
						}
						iss.clear();

						inputs.push_back(*gc);
						delete gc;
					} else if (temp == "output") { // get output
						temp = mParse->readTag();
						gc = new gateConnector();
						gc->connectionID = mParse->readTagValue(temp);
						mParse->readCloseTag();

						istringstream iss(mParse->readTagValue("output"));
						IDType tempId;
						while (iss >> tempId) {
							gc->wireIds.push_back(tempId);
						}
						iss.clear();

						outputs.push_back(*gc);
						delete gc;
					} else if (temp == "gparam" || temp == "lparam") { // get parameter
						string paramData = mParse->readTagValue(temp);
						string x, y;
						istringstream iss(paramData);
						iss >> x;
						getline(iss, y, '\n');
						pParam = new parameter(x, y.substr(1,y.size()-1), (temp == "gparam"));
						params.push_back(*pParam);
						delete pParam;
					}
					// ADD OTHER TAGS FOR GATE HERE
						// ALSO MODIFY parseGateToSend
					mParse->readCloseTag(); // </>
				} while (!mParse->isCloseTag(mParse->getCurrentIndex()));
				mParse->readCloseTag(); // >gate
				parseGateToSend(type, ID, position, inputs, outputs, params);
			}
			else if (temp == "wire") {
				//**********************************
				parseWireToSend();
			}
		}
		mParse->readTagValue(pageTag);
		mParse->readCloseTag();
	} while (!mParse->isCloseTag(mParse->getCurrentIndex()));

	mParse->readCloseTag();

	// This hack works in conjunction with the one at the beginning of saveCircuit.
	// There is only ever one tab when the dummy circuit is loaded.
	if (mParse->readTag() == "throw_away") {
		mParse->readCloseTag();
		gCanvases[0]->clearCircuit();
		gCanvas->clearCircuit();
		gCanvas->getCircuit()->reInitializeLogicCircuit();
		return parseFile();
	}

	gCanvas->getCircuit()->getOscope()->UpdateMenu();
	return gCanvases;
}

void CircuitParse::parseGateToSend(string type, string ID, string position, vector < gateConnector > &inputs, vector < gateConnector > &outputs, vector < parameter > &params) {
	// If no library was loaded, then don't try to make a gate from one
	if (wxGetApp().libraries.size() == 0) return;
	ostringstream oss;
	// Check the gate ID to see if it is taken
	long id;
	float x, y;
	istringstream issb(ID);
	issb >> id;
	
	string logicType = wxGetApp().libParser.getGateLogicType( type );
	if ( logicType.size() > 0 )
		gCanvas->getCircuit()->sendMessageToCore(new Message_CREATE_GATE(wxGetApp().libraries[wxGetApp().gateNameToLibrary[type]][type].logicType, id));
	// Create gate for GUI
	istringstream issa(position.substr(0,position.find(",")+1));
	issa >> x;
	issa.str(position.substr(position.find(",")+1,position.size()-position.find(",")-1));
	issa >> y;
	guiGate* newGate = gCanvas->getCircuit()->createGate( type, id, true );
	if (newGate == NULL) return; // IN CASE OF ERROR
	gCanvas->insertGate(id, newGate, x, y);
	for (unsigned int i = 0; i < params.size(); i++) {
		if (!(params[i].isGUI)) {
			newGate->setLogicParam( params[i].paramName, params[i].paramValue );
			gCanvas->getCircuit()->sendMessageToCore(new Message_SET_GATE_PARAM(id, params[i].paramName, params[i].paramValue));
		} else newGate->setGUIParam( params[i].paramName, params[i].paramValue );
	}
	if( logicType.size() > 0 ) {
		// Loop through the hotspots and pass logic core hotspot settings:
		LibraryGate libGate;
		wxGetApp().libParser.getGate(type, libGate);
		for( unsigned int i = 0; i < libGate.hotspots.size(); i++ ) {

			// Send the isInverted message:
			if( libGate.hotspots[i].isInverted ) {
				if ( libGate.hotspots[i].isInput ) {
					gCanvas->getCircuit()->sendMessageToCore(new Message_SET_GATE_INPUT_PARAM(id, libGate.hotspots[i].name, "INVERTED", "TRUE"));
				} else {
					gCanvas->getCircuit()->sendMessageToCore(new Message_SET_GATE_OUTPUT_PARAM(id, libGate.hotspots[i].name, "INVERTED", "TRUE"));
				}
			}

			// Send the logicEInput message:
			if( libGate.hotspots[i].logicEInput != "" ) {
				if ( libGate.hotspots[i].isInput ) {
					gCanvas->getCircuit()->sendMessageToCore(new Message_SET_GATE_INPUT_PARAM(id, libGate.hotspots[i].name, "E_INPUT", libGate.hotspots[i].logicEInput));
				} else {
					gCanvas->getCircuit()->sendMessageToCore(new Message_SET_GATE_OUTPUT_PARAM(id, libGate.hotspots[i].name, "E_INPUT", libGate.hotspots[i].logicEInput));
				}
			}
		} // for( loop through the hotspots )
	} // if( logic type is non-null )


	// Connect inputs and outputs.
	GUICircuit *gCircuit = gCanvas->getCircuit();
	for (unsigned int i = 0; i < inputs.size(); i++) {

		guiWire *wire = gCircuit->createWire(inputs[i].wireIds);

		cmdConnectWire::sendMessagesToConnect(gCircuit, wire->getID(),
			newGate->getID(), inputs[i].connectionID, true);

		gCanvas->insertWire(wire);
	}
	for (unsigned int i = 0; i < outputs.size(); i++) {

		guiWire *wire = gCircuit->createWire(outputs[i].wireIds);

		cmdConnectWire::sendMessagesToConnect(gCircuit, wire->getID(),
			newGate->getID(), outputs[i].connectionID, true);

		gCanvas->insertWire(wire);
	}
}

//********************************
void CircuitParse::parseWireToSend( void ) {
	// If no library was loaded, then no gates were made for us
	if (wxGetApp().libraries.size() == 0) return;
	// Parse the wire right here, generate its map and set it
	char dump;
	// parse the ID
	string ID; vector<IDType> ids;
	map < long, wireSegment > wireShape;
	do { // while next tag is not close wire
		// tags in wire can be ID or shape
		string temp = mParse->readTag(); // get tag
		if (temp == "ID") { // get ID
			ID = mParse->readTagValue(temp);
			mParse->readCloseTag(); // >ID
			ostringstream oss;
			istringstream iss(ID);

			IDType tempId;
			while (iss >> tempId) {
				ids.push_back(tempId);
			}
			iss.clear();

		} else if (temp == "shape") { //read tree
			do {
				// tags in shape can be hsegment or vsegment; they are identical aside from orientation
				bool isVertical = false;
				long headSegmentID = -1; // hold the first segment's id.
				temp = mParse->readTag();
				if (temp == "vsegment") isVertical = true;
				wireSegment newSeg; newSeg.verticalSeg = isVertical;
				do {
					// Within segments you have ID, points, connection, and intersection tags
					temp = mParse->readTag();
					if (temp == "ID") {					
						istringstream iss( mParse->readTagValue("ID") );
						iss >> newSeg.id;
						if (headSegmentID == -1) headSegmentID = newSeg.id;
						mParse->readCloseTag();
					} else if (temp == "points") {
						// points are begin.x, begin.y, end.x, end.y; comma delimited
						GLPoint2f begin, end;
						istringstream iss( mParse->readTagValue("points") );
						iss >> begin.x >> dump >> begin.y >> dump >> end.x >> dump >> end.y;
						newSeg.begin = begin;
						newSeg.end = end;
						newSeg.calcBBox();
						mParse->readCloseTag();
					} else if (temp == "connection") {
						// connection tags contain GID tag and name tag, one of each
						unsigned long GID; string hsName;
						for (int ct = 0; ct < 2; ct++) {
							temp = mParse->readTag();
							if (temp == "GID") {
								istringstream iss( mParse->readTagValue("GID") );
								iss >> GID;
								mParse->readCloseTag();
							} else if (temp == "name") {
								hsName = mParse->readTagValue("name");
								mParse->readCloseTag();
							}
						}
						wireConnection nwc; nwc.gid = GID; nwc.connection = hsName;
						nwc.cGate = (*(gCanvas->getCircuit()->getGates()))[GID];
						newSeg.connections.push_back( nwc );
						mParse->readCloseTag();
					} else if (temp == "intersection") {
						// intersections have intersection point and id
						istringstream iss( mParse->readTagValue("intersection") );
						GLfloat isectPoint; long isectSegID;
						iss >> isectPoint >> isectSegID;
						newSeg.intersects[isectPoint].push_back( isectSegID );
						mParse->readCloseTag();
					}
				} while (!mParse->isCloseTag(mParse->getCurrentIndex())); // !closesegment
				mParse->readCloseTag(); // >segment
				wireShape[newSeg.id] = newSeg;
			} while (!mParse->isCloseTag(mParse->getCurrentIndex())); // !closeshape
			mParse->readCloseTag(); // >shape
		}
	} while (!mParse->isCloseTag(mParse->getCurrentIndex())); // !closewire
	mParse->readCloseTag(); // >wire

	// Check to make sure the wire exists before we do things to it
	if ((gCanvas->getCircuit()->getWires())->find(ids.front()) == (gCanvas->getCircuit()->getWires())->end()) return;

	(*(gCanvas->getCircuit()->getWires()))[ids.front()]->setIDs(ids);
	(*(gCanvas->getCircuit()->getWires()))[ids.front()]->setSegmentMap( wireShape );
}

void CircuitParse::saveCircuit(string filename, vector< GUICanvas* > glc, unsigned int currPage) {
	ostringstream* ossCircuit = new ostringstream();

	// This is a sentinal circuit definition that is ignored by Cedar Logic 2.0 and newer.
	// Older versions of Cedar Logic will read this instead of the actual Circuit data.
	// This circuit decribes two labels with error messages.
	// The second label has a link to the download for the latest version of Cedar Logic.
	// I acknowledge that this is a hack...
	// Versions of Cedar Logic 2.0 and newer have a <version> tag.
	*ossCircuit << R"===(
<circuit>
<CurrentPage>0</CurrentPage>
<page 0>
<PageViewport>-32.95,39.6893,61.95,-63.2229</PageViewport>
<gate>
<ID>2</ID>
<type>AA_LABEL</type>
<position>14.5,-13</position>
<gparam>LABEL_TEXT Go to https://cedar.to/vjyQw7 to download the latest version!</gparam>
<gparam>TEXT_HEIGHT 2</gparam>
<gparam>angle 0.0</gparam></gate>
<gate>
<ID>3</ID>
<type>AA_LABEL</type>
<position>14.5,-9.5</position>
<gparam>LABEL_TEXT Error: This file was made with a newer version of Cedar Logic!</gparam>
<gparam>TEXT_HEIGHT 2</gparam>
<gparam>angle 0.0</gparam></gate></page 0>
</circuit>
<throw_away></throw_away>

	)===";

	mParse = new XMLParser(ossCircuit);

	mParse->openTag("version");
	mParse->writeTag("version", VERSION_NUMBER_STRING());
	mParse->closeTag("version");

	mParse->openTag("circuit");
	unordered_map < unsigned long, guiGate* >* gateList;
	unordered_map < unsigned long, guiWire* >* wireList;

	// Save which page was current:
	//	NOTE: currently this tag is not implemented
	mParse->openTag("CurrentPage");
	ostringstream oss;
	oss << currPage;
	mParse->writeTag("CurrentPage", oss.str());
	mParse->closeTag("CurrentPage");

	for (unsigned int i = 0; i < glc.size(); i++) {
		ostringstream oss;
		oss << "page " << i;
		string pageNumber = oss.str();
		mParse->openTag(pageNumber);

		// Save the page's last viewport
		mParse->openTag("PageViewport");
		oss.str("");
		oss.clear();
		GLPoint2f topLeft, bottomRight;
		glc[i]->getViewport(topLeft, bottomRight);
		oss << topLeft.x << "," << topLeft.y << "," << bottomRight.x << "," << bottomRight.y;
		mParse->writeTag("PageViewport", oss.str());
		mParse->closeTag("PageViewport");

		gateList = glc[i]->getGateList();
		wireList = glc[i]->getWireList();
		unordered_map< unsigned long, guiGate* >::iterator thisGate = gateList->begin();
		while (thisGate != gateList->end()) {
			(thisGate->second)->saveGate(mParse);
			thisGate++;
		}
		
		unordered_map< unsigned long, guiWire* >::iterator thisWire = wireList->begin();
		while (thisWire != wireList->end()) {
			if (thisWire->second != nullptr) {
				(thisWire->second)->saveWire(mParse);
			}
			thisWire++;
		}
		
		mParse->closeTag(pageNumber);
	}
	
	mParse->closeTag("circuit");
	
	ofstream outfile(filename.c_str());
	outfile << ossCircuit->str();
	outfile.close();
}
