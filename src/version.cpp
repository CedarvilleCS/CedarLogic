
#include "CedarLogic.h"
#include "version.h"
#include <map>
using namespace std;

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
	return std::to_string(CedarLogic_VERSION_MAJOR) + "." +
		std::to_string(CedarLogic_VERSION_MINOR) + "." +
		std::to_string(CedarLogic_VERSION_PATCH) + " | " +
		year + "-" + month + "-" + day + " " + hms;
}

string VERSION_TITLE() {
	return "Cedar Logic " + VERSION_NUMBER_STRING();
}

string VERSION_ABOUT_TEXT() {

	return
		VERSION_TITLE() + "\n"
		R"===(
Protected under the GNU General Public License v3.0.
See https://github.com/CedarvilleCS/CedarLogic for more information.
)===";
}