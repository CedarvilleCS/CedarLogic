
#include "quoted.h"
#include <cctype>
#include <iostream>

namespace parse {

	void printQuotedAscii(std::ostream &out, const std::string &str) {

		out.put('"');

		for (const char c : str) {
			switch (c) {
			case '\n':
				out.put('\\');
				out.put('n');
				break;
			case '\\':
				out.put('\\');
				out.put('\\');
				break;
			case '\"':
				out.put('\\');
				out.put('"');
				break;
			case '\t':
				out.put('\\');
				out.put('t');
				break;
			default:
				out.put(c);
				break;
			}
		}

		out.put('"');
	}

	void parseQuotedAscii(std::istream &in, std::string &result) {

		result.clear();

		// Ignore leading spaces.
		char c;
		while (in.get(c)) {
			if (!isspace(c)) {
				break;
			}
		}

		// Parse with standard string input for no-quotes.
		if (c != '"') {
			in.unget();
			in >> result;
		}
		else {

			// Parse quoted strings.
			bool inEscape = false;
			while (in.get(c)) {

				if (c < ' ' || c > '~') {

					// Fail for illegal characters.
					in.clear(std::ios::failbit);
					break;
				}

				if (inEscape) {
					inEscape = false;

					// Parse escape sequences.
					char toAppend;
					if (c == 'n') {
						toAppend = '\n';
					}
					else if (c == '\\') {
						toAppend = '\\';
					}
					else if (c == '"') {
						toAppend = '"';
					}
					else if (c == 't') {
						toAppend = '\t';
					}
					else {

						// Fail for illegal escapes.
						in.clear(std::ios::failbit);
						break;
					}

					result.push_back(toAppend);
				}
				else {

					// Parse string contents.
					if (c == '"') {

						// Finish.
						break;
					}
					else if (c == '\\') {
						inEscape = true;
					}
					else {
						result.push_back(c);
					}
				}
			}
		}

		// Return empty string for failures.
		if (!in) {
			result.clear();
		}
	}
}