
#include "version.h"
#include <map>
using namespace std;

namespace {
	const int VERSION_MAJOR = 2;
	const int VERSION_MINOR = 0;
}

string VERSION_NUMBER_STRING() {

	// Date in format: "MMM DD YYYY"
	static string date = __DATE__;

	// Time in format: "HH:MM:SS"
	static string hms = __TIME__;

	static map<string, string> monthToDigits = {
		{ "Jan", "01" },
		{ "Feb", "02" },
		{ "Mar", "03" },
		{ "Apr", "04" },
		{ "May", "05" },
		{ "Jun", "06" },
		{ "Jul", "07" },
		{ "Aug", "08" },
		{ "Sep", "09" },
		{ "Oct", "10" },
		{ "Nov", "11" },
		{ "Dec", "12" }
	};

	string year = date.substr(7, 4);

	string month = date.substr(0, 3);
	month = monthToDigits[month];

	string day = date.substr(4, 2);

	// Version in format: "MAJOR.MINOR|YYYY-MM-DD HH:MM:SS"
	return std::to_string(VERSION_MAJOR) + "." +
		std::to_string(VERSION_MINOR) + " | " +
		year + "-" + month + "-" + day + " " + hms;
}

string VERSION_TITLE() {
	return "Cedar Logic " + VERSION_NUMBER_STRING();
}

string VERSION_ABOUT_TEXT() {

	return
		VERSION_TITLE() + "\n"
		"Copyright 2007 Cedarville University, Matt Lewellyn, \n"
		"\tDavid Knierim, Ben Sprague, Joshua Lansford\n"
		"\tand Nathan Harro\n"
		"\n"
		"Font rendering thanks to GLFont library (created by Brad Fish, bhf5@email.byu.edu)\n"
		"\n"
		"All rights reserved\n"
		"See license.txt for details.";
}