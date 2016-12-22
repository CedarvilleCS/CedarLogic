
#pragma once
#include <string>

// Versioning.
// Make sure that this gets updated with each release.

const static int VERSION_MAJOR = 2;

const static int VERSION_MINOR = 0;

const static int VERSION_NUMBER = VERSION_MAJOR * 1000 + VERSION_MINOR;

const static std::string VERSION_NUMBER_STRING =
	std::to_string(VERSION_MAJOR) + "." + std::to_string(VERSION_MINOR);

const static std::string VERSION_ABOUT_TEXT =
	"CEDAR Logic Simulator 1.5a\n"
	"Copyright 2007 Cedarville University, Matt Lewellyn, \n"
	"\tDavid Knierim, Ben Sprague, Joshua Lansford\n"
	"\tand Nathan Harro\n"
	"\n"
	"Font rendering thanks to GLFont library (created by Brad Fish, bhf5@email.byu.edu)\n"
	"\n"
	"All rights reserved\n"
	"See license.txt for details.";

const static std::string APP_TITLE =
	"Cedar Logic " + VERSION_NUMBER_STRING;