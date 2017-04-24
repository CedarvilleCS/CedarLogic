/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   gl_text: Encapsulation of text for GUI objects
*****************************************************************************/

#pragma once

#include "OpenGL.h"
#include <string>

using namespace std;

struct GLbox {
	GLdouble top;
	GLdouble bottom;
	GLdouble left;
	GLdouble right;
};

class gl_text {
public:
	gl_text();
	
	// Load the font into a texture for the currently-selected GL context
	//	Call this for each context after initialization
	static void loadFont(string);
	
	// *************** Action methods *********************

	// Render using current settings on current canvas:
	void draw( void );

	// Return the bounding box of the text object (in local-space coordinates + scale and translation):
	GLbox getBoundingBox( void );

	// *************** Mutator methods ****************************
	void setColor( GLdouble c1, GLdouble c2, GLdouble c3, GLdouble c4 ) {
		color[0] = c1;
		color[1] = c2;
		color[2] = c3;
		color[3] = c4;
	};
	
	void getColor( GLdouble& c1, GLdouble& c2, GLdouble& c3, GLdouble& c4 ) {
		c1 = color[0];
		c2 = color[1];
		c3 = color[2];
		c4 = color[3];
	};

	// Set and get the position of the text object (Defaults to <0.0, 0.0>)
	void setPosition( GLdouble x, GLdouble y ) {
		translate[0] = x;
		translate[1] = y;
	};

	void getPosition( GLdouble& x, GLdouble& y ) {
		x = translate[0];
		y = translate[1];
	};
	
	// Set the scale factor by setting a text height and aspect ratio (w / h).
	// NOTE: You can't get these values back from this class, or any direct scale info.
	void setSize( GLdouble textHeight, GLdouble aspect = 1.0 );

	void setRotation(float rotation) {
		this->rotation = rotation;
	}
	
	// Get and set the text string.
	string getText( void ) { return textString; };
	void setText( string newString ) { textString = newString; };

private:
	// The text color:
	float color[4];
	
	// X and Y scale factors:
	GLdouble scale[2];
	
	// X and Y translation factors (Optional):
	GLdouble translate[2];

	// The text string to be displayed:	
	string textString;

	// Text rotation.
	float rotation;
	
	// The font loading initialization flag:
	static bool fontIsLoaded;
	
	// The counted reference object to tell when there are no more gl_text objects
	// that exist:
	static unsigned long numTextObjects;
};
