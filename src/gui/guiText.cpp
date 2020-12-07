/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   guiText: Encapsulation of text for GUI objects
*****************************************************************************/

//I think they are using http://www.forexseek.com/glf/ glf type fonts...

#include "guiText.h"

// Include the glf font library:
#include "MainApp.h"
#include "GLFont/glfont2.h"

#define TEXT_SCALE_FACTOR 0.050
#define FONT_TEXTURE_ID 255

DECLARE_APP(MainApp)

static glfont::GLFont fontFace;

guiText::guiText() {
	
	// The text color (Default = black):
	color[0] = 0.2f;
	color[1] = 0.2f;
	color[2] = 0.2f;
	color[3] = 1.0f;
	
	// X and Y scale factors (Default = none):
	scale[0] = 1.0;
	scale[1] = 1.0;
	
	// X and Y translation factors (Default = none):
	translate[0] = 0.0;
	translate[1] = 0.0;

	// The text string to be displayed:	
	textString = "Text";
}

guiText::~guiText() {
}


// *************** Action methods *********************

// Render using current settings on current canvas:
void guiText::draw( void ) {

	// Store the old color to restore after we've drawn:
	GLfloat oldColor[4];
	glGetFloatv( GL_CURRENT_COLOR, oldColor );
	glColor4f( color[0], color[1], color[2], color[3] );

	// Isolate our matrix changes by adding to the current
	// matrix and then reverting back to it afterward:
	glPushMatrix();
		
		// Set the translation and scaling:
		glTranslatef( translate[0], translate[1], 0.0 );
		glScalef(scale[0], scale[1], 1);

		// Draw the text:
		glEnable(GL_TEXTURE_2D);
		fontFace.Begin();
		fontFace.DrawString(textString, 0., 0.);
		glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	// Set the color back to the old color:
	glColor4f( oldColor[0], oldColor[1], oldColor[2], oldColor[3] );
	
} // draw()

// Return the bounding box of the text object (in local-space coordinates + scale and translation):
GLbox guiText::getBoundingBox( void ) {
	GLbox tempBox;

	std::pair<int, int> size;
	fontFace.GetStringSize( textString, &size );
	tempBox.left = 0;
	tempBox.right = size.first*scale[0];
	tempBox.top = -size.second*0.1667*scale[1];
	tempBox.bottom = -size.second*0.8333*scale[1];
	return tempBox;
}

// *************** Mutator methods ****************************
	
// Set the scale factor by setting a text height and aspect ratio (w / h).
// NOTE: You can't get these values back from this class, or any direct scale info.
void guiText::setSize( GLdouble textHeight, GLdouble aspect ) {
	// Height:
	scale[1] = textHeight * TEXT_SCALE_FACTOR;
	
	// Width:
	scale[0] = scale[1] * aspect;
}

// **************** STATIC METHODS ****************************

// loadFont - call this for each context after initialization
void guiText::loadFont(string fontpath) {
	// Pedro Casanova (casanova@ujaen.es) 2020/04-11
	if (fontpath == "res")
	{
		// Font in Resources
		HANDLE hResource = FindResource(NULL, "ARIAL.GLF", "BIN");
		DWORD nLen = SizeofResource(NULL, (HRSRC)hResource);
		hResource = LoadResource(NULL, (HRSRC)hResource);
		void* lpResData = LockResource(hResource);
		fontFace.Create(lpResData, FONT_TEXTURE_ID);
	}
	else
	{
		fontFace.Create(fontpath.c_str(), FONT_TEXTURE_ID);
	}
}
