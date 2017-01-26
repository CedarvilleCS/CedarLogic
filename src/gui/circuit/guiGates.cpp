/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   guiGate: GUI representation of gate objects
*****************************************************************************/

#include "guiGates.h"
#include <iomanip>
#include "wx/wx.h"
#include "../MainApp.h"
#include "../klsCollisionChecker.h"
#include "../dialog/paramDialog.h"
#include "guiWire.h"

#include <float.h>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <string>
#include <fstream>
#include "wx/glcanvas.h"
#include "wx/docview.h"

#include "../../logic/logic_values.h"
#include "../XMLParser.h"
#include "../gl_text.h"
#include "../thread/Message.h"
#include "../dialog/RamPopupDialog.h"
#include "../dialog/Z80PopupDialog.h"
#include "../dialog/ADCPopupDialog.h"
#include "../gl_wrapper.h"
#include "../gl_defs.h"

using namespace std;

DECLARE_APP(MainApp)


#define SELECTED_LABEL_INTENSITY 0.50

// TO-FROM constants.
#define TO_FROM_TEXT_HEIGHT 1.5
#define TO_BUFFER 0.4
#define FROM_BUFFER 0.0
#define FROM_FIX_SHIFT 0.0
#define FLIPPED_OFFSET 0.5











// *********************** guiGateREGISTER *************************

guiGateREGISTER::guiGateREGISTER() {
	guiGate();
	// Default to 0 when creating:
	//NOTE: Does not send this to the core, just updates it
	// on the GUI side.
	renderInfo_numDigitsToShow = 1;
	setLogicParam( "CURRENT_VALUE", "0" );
	setLogicParam( "UNKNOWN_OUTPUTS", "false" );
}

void guiGateREGISTER::draw( bool color ) {
	// Draw the default lines:
	guiGate::draw(color);

	float diffx = renderInfo_diffx;
	float diffy = renderInfo_diffy;
	
	diffx /= (double)renderInfo_numDigitsToShow; // set width of each digit
	
	//Inner Square for value
	if (color) {
		// Display box
		glBegin( GL_LINE_LOOP );
			glVertex2f(renderInfo_valueBox.begin.x,renderInfo_valueBox.begin.y);
			glVertex2f(renderInfo_valueBox.begin.x,renderInfo_valueBox.end.y);
			glVertex2f(renderInfo_valueBox.end.x,renderInfo_valueBox.end.y);
			glVertex2f(renderInfo_valueBox.end.x,renderInfo_valueBox.begin.y);
		glEnd();
		
		// Draw the number in red (or blue if inputs are not all sane)
		if (renderInfo_drawBlue) glColor4f( 0.3f, 0.3f, 1.0, 1.0 );
		else glColor4f( 1.0, 0.0, 0.0, 1.0 );

		GLfloat lineWidthOld;
		glGetFloatv(GL_LINE_WIDTH, &lineWidthOld);
		glLineWidth(2.0);
        
		// THESE ARE ALL SEVEN SEGMENTS WITH DIFFERENTIAL COORDS.  USE THEM FOR EACH DIGIT VALUE FOR EACH DIGIT
		//		AND INCREMENT CURRENTDIGIT.  CURRENTDIGIT=0 IS MSB.
		glBegin( GL_LINES );
		for (unsigned int currentDigit = 0; currentDigit < renderInfo_currentValue.size(); currentDigit++) {
			char c = renderInfo_currentValue[currentDigit];
			if ( c != '1' && c != '4' && c != 'B' && c != 'D' ) {
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.1875),renderInfo_valueBox.begin.y+(diffy*0.88462)); // TOP
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.8125),renderInfo_valueBox.begin.y+(diffy*0.88462)); }
			if ( c != '0' && c != '1' && c != '7' && c != 'C' ) {
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.1875),renderInfo_valueBox.begin.y+(diffy*0.5)); // MID
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.8125),renderInfo_valueBox.begin.y+(diffy*0.5)); }
			if ( c != '1' && c != '4' && c != '7' && c != '9' && c != 'A' && c != 'F' ) {
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.1875),renderInfo_valueBox.begin.y+(diffy*0.11538)); // BOTTOM
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.8125),renderInfo_valueBox.begin.y+(diffy*0.11538)); }
			if ( c != '1' && c != '2' && c != '3' && c != '7' && c != 'D' ) {
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.1875),renderInfo_valueBox.begin.y+(diffy*0.88462)); // TL
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.1875),renderInfo_valueBox.begin.y+(diffy*0.5)); }
			if ( c != '5' && c != '6' && c != 'B' && c != 'C' && c != 'E' && c != 'F' ) {
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.8125),renderInfo_valueBox.begin.y+(diffy*0.88462)); // TR
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.8125),renderInfo_valueBox.begin.y+(diffy*0.5)); }
			if ( c != '1' && c != '3' && c != '4' && c != '5' && c != '7' && c != '9' ) {
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.1875),renderInfo_valueBox.begin.y+(diffy*0.11538)); // BL
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.1875),renderInfo_valueBox.begin.y+(diffy*0.5)); }
			if ( c != '2' && c != 'C' && c != 'E' && c != 'F' ) {
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.8125),renderInfo_valueBox.begin.y+(diffy*0.11538)); // BR
				glVertex2f(renderInfo_valueBox.begin.x+(diffx*currentDigit)+(diffx*0.8125),renderInfo_valueBox.begin.y+(diffy*0.5)); }
		}
		glEnd();
		glLineWidth(lineWidthOld);
		glColor4f( 0.0, 0.0, 0.0, 1.0 );
	}
}

void guiGateREGISTER::setLogicParam(const std::string &paramName, const std::string &value) {
	int intVal;
	if (paramName == "INPUT_BITS") {
		// How many digits should I show? (min of 1)
		istringstream iss( value );
		iss >> renderInfo_numDigitsToShow;
		renderInfo_numDigitsToShow = (int) ceil( ((double) renderInfo_numDigitsToShow) / 4.0 );
		if (renderInfo_numDigitsToShow == 0) renderInfo_numDigitsToShow = 1;
		if (getLogicParam("CURRENT_VALUE") != "") {
			iss.clear(); iss.str( getLogicParam("CURRENT_VALUE") );
			iss >> intVal;		
	        ostringstream ossVal;
	        for (int i=2*sizeof(int) - 1; i>=0; i--) {
	            ossVal << "0123456789ABCDEF"[((intVal >> i*4) & 0xF)];
	        }
	        renderInfo_currentValue = ossVal.str().substr(ossVal.str().size()-renderInfo_numDigitsToShow,renderInfo_numDigitsToShow);
		}
	} else if (paramName == "UNKNOWN_OUTPUTS") {
		renderInfo_drawBlue = (value == "true");
	} else if (paramName == "CURRENT_VALUE") {
		istringstream iss( value );
		iss >> intVal;		
        ostringstream ossVal;
        for (int i=2*sizeof(int) - 1; i>=0; i--) {
            ossVal << "0123456789ABCDEF"[((intVal >> i*4) & 0xF)];
        }
        renderInfo_currentValue = ossVal.str().substr(ossVal.str().size()-renderInfo_numDigitsToShow,renderInfo_numDigitsToShow);
	}
	guiGate::setLogicParam(paramName, value);
}

void guiGateREGISTER::setGUIParam(const std::string &paramName, const std::string &value) {
	if (paramName == "VALUE_BOX") {
		istringstream iss(value);
		char dump;
		iss >> renderInfo_valueBox.begin.x >> dump >> renderInfo_valueBox.begin.y >>
			dump >> renderInfo_valueBox.end.x >> dump >> renderInfo_valueBox.end.y;
		renderInfo_diffx = renderInfo_valueBox.end.x - renderInfo_valueBox.begin.x;
		renderInfo_diffy = renderInfo_valueBox.end.y - renderInfo_valueBox.begin.y;
	}
	guiGate::setGUIParam(paramName, value);
}

// ******************** END guiGateREGISTER **********************













// *********************** guiGatePULSE *************************

guiGatePULSE::guiGatePULSE() : guiGate() {

	// Set the default CLICK box:
	// Format is: "minx miny maxx maxy"
	setGUIParam("CLICK_BOX", "-0.76,-0.76,0.76,0.76");

	// Default to single pulse width:
	setGUIParam("PULSE_WIDTH", "1");
};

// Send a pulse message to the logic core whenever the gate is
// clicked on:
Message_SET_GATE_PARAM* guiGatePULSE::checkClick( GLfloat x, GLfloat y ) {
	klsBBox toggleButton;

	// Get the size of the CLICK square from the parameters:
	string clickBox = getGUIParam( "CLICK_BOX" );
	istringstream iss(clickBox);
	GLdouble minx = -0.5;
	GLdouble miny = -0.5;
	GLdouble maxx = 0.5;
	GLdouble maxy = 0.5;
	char dump;
	iss >> minx >> dump >> miny >> dump >> maxx >> dump >> maxy;

	toggleButton.addPoint( modelToWorld( GLPoint2f( minx, miny ) ) );
	toggleButton.addPoint( modelToWorld( GLPoint2f( minx, maxy ) ) );
	toggleButton.addPoint( modelToWorld( GLPoint2f( maxx, miny ) ) );
	toggleButton.addPoint( modelToWorld( GLPoint2f( maxx, maxy ) ) );

	if (toggleButton.contains( GLPoint2f( x, y ) )) {
/*		ostringstream oss;
		oss << "SET GATE ID " << getID() << " PARAMETER PULSE " << getGUIParam("PULSE_WIDTH"); */
		return new Message_SET_GATE_PARAM(getID(), "PULSE", getGUIParam("PULSE_WIDTH"));
	} else return NULL;
}











guiGateLED::guiGateLED() {
	guiGate();
	
	// Set the default LED box:
	// Format is: "minx miny maxx maxy"
	setGUIParam( "LED_BOX", "-0.76,-0.76,0.76,0.76" );
}

void guiGateLED::draw( bool color ) {
	StateType outputState = HI_Z;
	
	// Draw the default lines:
	guiGate::draw(color);
	
	// Get the first connected input in the LED's library description:
	// map i/o name to wire id
	map< string, guiWire* >::iterator theCnk = connections.begin();
	if( theCnk != connections.end() ) {
		outputState = (theCnk->second)->getState()[0];
	}

	switch( outputState ) {
	case ZERO:
		glColor4f( 0.0, 0.0, 0.0, 1.0 );
		break;
	case ONE:
		glColor4f( 1.0, 0.0, 0.0, 1.0 );
		break;
	case HI_Z:
		glColor4f( 0.0, 0.78f, 0.0, 1.0 );
		break;
	case UNKNOWN:
		glColor4f( 0.3f, 0.3f, 1.0, 1.0 );
		break;
	case CONFLICT:
		glColor4f( 0.0, 1.0, 1.0, 1.0 );
		break;
	}

	//Inner Square
	if (color) glRectd  ( renderInfo_ledBox.begin.x, renderInfo_ledBox.begin.y, 
			renderInfo_ledBox.end.x, renderInfo_ledBox.end.y ) ;

	// Set the color back to black:
	glColor4f( 0.0, 0.0, 0.0, 1.0 );
}

void guiGateLED::setGUIParam(const std::string &paramName, const std::string &value) {
	if (paramName == "LED_BOX") {
		istringstream iss(value);
		char dump;
		iss >> renderInfo_ledBox.begin.x >> dump >> renderInfo_ledBox.begin.y >>
			dump >> renderInfo_ledBox.end.x >> dump >> renderInfo_ledBox.end.y;
	}
	guiGate::setGUIParam(paramName, value);
}









guiLabel::guiLabel() {
	guiGate();
	// Set default parameters:
	setGUIParam( "LABEL_TEXT", "BLANK" );
	setGUIParam( "TEXT_HEIGHT", "2.0" );
}

void guiLabel::draw( bool color ) {
	// Position the gate at its x and y coordinates:
	glLoadMatrixd(mModel);
	
	// Pick the color for the text:
	if( selected && color ) {
		GLdouble c = 1.0 - SELECTED_LABEL_INTENSITY;
		theText.setColor( 1.0, c / 4, c / 4, SELECTED_LABEL_INTENSITY );
	} else {
		theText.setColor( 0.0, 0.0, 0.0, 1.0 );
	}
	
	// Draw the text:
	theText.draw();
}

GLdouble guiLabel::getTextHeight() {

	istringstream iss(gparams["TEXT_HEIGHT"]);
	GLdouble textHeight = 1.0;
	iss >> textHeight;

	return textHeight;
};

// A custom setParam function is required because
// the object must resize it's bounding box 
// each time the LABEL_TEXT or TEXT_HEIGHT parameter is set.
void guiLabel::setGUIParam(const std::string &paramName, const std::string &value) {
	if( (paramName == "LABEL_TEXT") || (paramName == "TEXT_HEIGHT")  ) {

		std::string setValue = value;

		if( paramName == "TEXT_HEIGHT" ) {
			// Make the text parameter safe:
			istringstream iss(value);
			GLdouble textHeight = 1.0;
			iss >> textHeight;

			if( textHeight < 0 ) textHeight = -textHeight;
			if( textHeight < 0.01 ) textHeight = 0.01;
			
			ostringstream oss;
			oss << textHeight;
			setValue = oss.str();
		}
	
		guiGate::setGUIParam( paramName, setValue);

		string labelText = getGUIParam("LABEL_TEXT");
		GLdouble height = getTextHeight();
		theText.setSize( height );
		theText.setText( labelText );

		//Sets bounding box size
		this->calcBBox();
	} else {
		guiGate::setGUIParam( paramName, value );
	}
}

void guiLabel::calcBBox( void ) {
	GLbox textBBox = theText.getBoundingBox();
	float dx = fabs(textBBox.right-textBBox.left)/2.;
	float dy = fabs(textBBox.top-textBBox.bottom)/2.;
	double currentX, currentY; theText.getPosition(currentX, currentY);
	theText.setPosition(-dx, +dy);
	modelBBox.reset();
	modelBBox.addPoint( GLPoint2f(textBBox.left-dx, textBBox.bottom+dy) );
	modelBBox.addPoint( GLPoint2f(textBBox.right-dx, textBBox.top+dy) );

	// Recalculate the world-space bbox:
	updateBBoxes();
}










guiTO_FROM::guiTO_FROM() {
	// Note that I don't set the JUNCTION_ID parameter yet, because
	// that would call setParam() and that would call calcBBox()
	// and that wants to know that the gate's type is, which we don't know yet.
	
	guiGate();
	
	// Initialize the text object:
	theText.setSize( TO_FROM_TEXT_HEIGHT );
}

void guiTO_FROM::draw( bool color ) {
	// Draw the lines for this gate:
	guiGate::draw();

	// Position the gate at its x and y coordinates:
	glLoadMatrixd(mModel);
	
	// Pick the color for the text:
	if( selected && color ) {
		GLdouble c = 1.0 - SELECTED_LABEL_INTENSITY;
		theText.setColor( 1.0, c / 4, c / 4, SELECTED_LABEL_INTENSITY );
	} else {
		theText.setColor( 0.0, 0.0, 0.0, 1.0 );
	}
	
	//********************************
	//Edit by Joshua Lansford 04/04/07
	//Upside down text on tos and froms
	//isn't that exciting.
	//This will rotate the text around
	//before it is printed
	if( this->getGUIParam( "angle" ) == "180" ||
	    this->getGUIParam( "angle" ) ==  "90" ){
		
		//scoot the label over
		GLbox textBBox = theText.getBoundingBox();
		GLdouble textWidth = textBBox.right - textBBox.left;
		int direction = 0;
		if( getGUIType() == "TO" ) {
			direction = +1;
		} else if (getGUIType() == "FROM") {
			direction = -1;
		}
		glTranslatef( direction * (textWidth + FLIPPED_OFFSET), 0, 0 );
		
		//and spin it around
		glRotatef( 180, 0.0, 0.0, 1.0);
	}
	//End of Edit*********************
	
	// Draw the text:
	theText.draw();
}

// A custom setParam function is required because
// the object must resize it's bounding box 
// each time the JUNCTION_ID parameter is set.
void guiTO_FROM::setLogicParam(const std::string &paramName, const std::string &value) {
	if( paramName == "JUNCTION_ID" ) {
		guiGate::setLogicParam( paramName, value );

		string labelText = getLogicParam("JUNCTION_ID");
		theText.setText( labelText );
		theText.setSize( TO_FROM_TEXT_HEIGHT );

		//Sets bounding box size
		this->calcBBox();
	} else {
		guiGate::setLogicParam( paramName, value );
	}
}

void guiTO_FROM::calcBBox() {
	
	// Set the gate's bounding box based on the lines:
	guiGate::calcBBox();

	// Get the text's bounding box:	
	GLbox textBBox = theText.getBoundingBox();

	// Adjust the bounding box based on the text's bbox:
	GLdouble textWidth = textBBox.right - textBBox.left;
	if( getGUIType() == "TO" ) {
		GLPoint2f bR = modelBBox.getBottomRight();
		bR.x += textWidth;
		modelBBox.addPoint( bR );
		theText.setPosition( TO_BUFFER, TO_FROM_TEXT_HEIGHT/2+0.30 );
	} else if (getGUIType() == "FROM") {
		GLPoint2f tL = modelBBox.getTopLeft();
		tL.x -= (textWidth + FROM_BUFFER);
		modelBBox.addPoint( tL );
		theText.setPosition( tL.x + FROM_FIX_SHIFT, TO_FROM_TEXT_HEIGHT/2+0.30 );
	}

	// Recalculate the world-space bbox:
	updateBBoxes();
}







//*************************************************
//Edit by Joshua Lansford 12/25/2006
//I am creating a guiGate for the RAM so that
//the ram can have its own special pop-up window
guiGateRAM::guiGateRAM(){
	guiGate();
	ramPopupDialog = NULL;
}

guiGateRAM::~guiGateRAM(){	
	//Destroy is how you 'delete' wxwidget objects
	if( ramPopupDialog != NULL ){
		ramPopupDialog->Destroy();
		ramPopupDialog = NULL;
	}
}


void guiGateRAM::doParamsDialog(GUICircuit *gc, wxCommandProcessor *wxcmd) {
	if( ramPopupDialog == NULL ){
		ramPopupDialog = new RamPopupDialog( this, addressBits, (GUICircuit*)gc );
		ramPopupDialog->updateGridDisplay();
	}
	ramPopupDialog->Show( true );
}

//Saves the ram contents to the circuit file
//when the circuit saves
void guiGateRAM::saveGateTypeSpecifics( XMLParser* xparse ){
	for( map< unsigned long, unsigned long >::iterator I = memory.begin();
	     	I != memory.end();  ++I ){
	     if( I->second != 0 ){
			xparse->openTag("lparam");
		    ostringstream memoryValue;
			memoryValue << "Address:" << I->first << " " << I->second; 
			xparse->writeTag("lparam", memoryValue.str());
			xparse->closeTag("lparam");
	     }
	}
}

//Because the ram gui will be passed lots of data
//from the ram logic, we don't want it all going
//into the default hash of changed paramiters.
//Thus we catch it here
void guiGateRAM::setLogicParam(const std::string &paramName, const std::string &value) {
	
	if( paramName.substr( 0, 8 ) == "lastRead" ){	
		//this makes it so that the pop-up will green things
		//that have been just read
		istringstream addressiss( value );
		unsigned long address = 0;
		addressiss >> address;
		lastRead = address;
		if( ramPopupDialog != NULL )
			ramPopupDialog->updateGridDisplay();
	}else if( paramName.substr( 0, 8 ) == "Address:" ){
		istringstream addressiss( paramName.substr( 8 ) );
		unsigned long address = 0;
		addressiss >> address;
		istringstream dataiss( value );
		unsigned long data = 0;
		dataiss >> data;
		memory[ address ] = data;
		if( ramPopupDialog != NULL )
			ramPopupDialog->updateGridDisplay();
		lastWritten = address;
	}else if( paramName == "MemoryReset" ){
		memory.clear();
		if( ramPopupDialog != NULL )
		    ramPopupDialog->notifyAllChanged();
	}else if( paramName == "ADDRESS_BITS" ) {
		istringstream dataiss( value );
		dataiss >> addressBits;
		guiGate::setLogicParam( paramName, value );
		// Declare the address pins!		
	} else if( paramName == "DATA_BITS" ) {
		istringstream dataiss( value );
		dataiss >> dataBits;
		guiGate::setLogicParam( paramName, value );
	}else{ 
		guiGate::setLogicParam( paramName, value );
	}
}

//This method is used by the RamPopupDialog to
//learn what values are at different addresses
//in memory.
unsigned long guiGateRAM::getValueAt( unsigned long address ){
	//we want to refrain from creating an entry for an item
	//if it does not already exist.  Therefore we will not
	//use the [] operator
	map<unsigned long, unsigned long>::iterator finder = memory.find( address );
	if( finder != memory.end() )
		return finder->second;
	return 0;
}

//These is used by the pop-up to determine
//what was the last value read and written
long guiGateRAM::getLastWritten(){
	return lastWritten;
}
long guiGateRAM::getLastRead(){
	return lastRead;
}
//End of edit
//*************************************************









// ************************ z80 gate ****************************

//*************************************************
//Edit by us 1/26/2006
//I am creating a guiGate for the z80 so that
//the Z80 can have its own special pop-up window
guiGateZ80::guiGateZ80(){
	guiGate();
	z80PopupDialog = NULL;
}

guiGateZ80::~guiGateZ80(){	
	//Destroy is how you 'delete' wxwidget objects
	if( z80PopupDialog != NULL ){
		z80PopupDialog->Destroy();
		z80PopupDialog = NULL;
	}
}


void guiGateZ80::doParamsDialog(GUICircuit *gc, wxCommandProcessor *wxcmd) {
	if( z80PopupDialog == NULL ){
		z80PopupDialog = new Z80PopupDialog( this, (GUICircuit*)gc );
	}
	z80PopupDialog->Show( true );
}

void guiGateZ80::setLogicParam(const std::string &paramName, const std::string &value) {
	

	guiGate::setLogicParam( paramName, value );
	
	if (z80PopupDialog != NULL) {
		z80PopupDialog->NotifyOfRegChange();
	}
	
}
//End of edit
//*************************************************









// ************************ ADC gate ****************************

//*************************************************
//Edit by Joshua Lansford 05/10/2007
//I am creating a guiGate for the ADC so that
//the ADC can have its own special pop-up window
guiGateADC::guiGateADC(){
	guiGate();
	aDCPopupDialog = NULL;
}

guiGateADC::~guiGateADC(){	
	//Destroy is how you 'delete' wxwidget objects
	if( aDCPopupDialog != NULL ){
		aDCPopupDialog->Destroy();
		aDCPopupDialog = NULL;
	}
}


void guiGateADC::doParamsDialog(GUICircuit *gc, wxCommandProcessor *wxcmd) {
	if( aDCPopupDialog == NULL ){
		aDCPopupDialog = new ADCPopupDialog( this, (GUICircuit*)gc );
		aDCPopupDialog->notifyValueChanged();
	}
	aDCPopupDialog->Show( true );
}

void guiGateADC::setLogicParam(const std::string &paramName, const std::string &value) {
	guiGate::setLogicParam( paramName, value );
	
	if( paramName == "VALUE" && aDCPopupDialog != NULL )
		aDCPopupDialog->notifyValueChanged();
}

//End of edit
//*************************************************
