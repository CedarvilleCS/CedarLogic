
#include "version.h"
#include <map>
using namespace std;

namespace {
	const int VERSION_MAJOR = 2;
	const int VERSION_MINOR = 3;
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
		R"===(
Copyright (c) 2007, Cedarville University, Benjamin Sprague, Matthew Lewellyn, David Knierim, Joshua Lansford, Nathan Harro.
Copyright (c) 2017, Cedarville University, Tyler Drake, Julian Pernia, Jackson Vaugn, Colin Broberg.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of the Cedarville University, Benjamin Sprague, Matthew Lewellyn, David Knierim, nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
)===";
}