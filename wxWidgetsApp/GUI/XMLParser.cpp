/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   XMLParser: Scans and tokenizes an XML file
*****************************************************************************/

#include "XMLParser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// XMLParser is generated from an already open file stream.
//	The lines of the file are read into an STL vector of strings.
XMLParser::XMLParser(fstream* strIO, bool writing)
{
	mStream = strIO;
	if (writing) return;
	string temp;
	while (!mStream->eof()) {
	    getline(*mStream, temp, '\n');
	    lines.push_back(temp + '\n');
	}
  	lineIdx = 0;
  	linePtr = -1;
	
	// Go ahead and get the first token, so when getNextToken
	//	is called, a valid token is already scanned to return.
	nextToken = scanNextToken();
}

// XMLParser is generated from an already open file stream.
//	This constructor sets the output file stream for writing.
XMLParser::XMLParser(ostream* strIO) {
	writeStream = strIO;
}

// The destructor will simply remove the pointer.
XMLParser::~XMLParser()
{
	mStream = (fstream*)0;
	writeStream = (ofstream*)0;
}

// Return the current token, and scan for the next one
Token XMLParser::getNextToken() {
	Token returnToken = nextToken;
	if (nextToken.tokenType != XML_EOF)
		nextToken = scanNextToken();
	return returnToken;
}

// FSM to scan for next token
Token XMLParser::scanNextToken() {
	XMLTokenType tokenType = XML_EOF;
	string tokenData;
	bool done = false;
	char peekChar = 0, addChar = 0;
	int state = 0;
	
	while (!done) {
		peekChar = peekNextChar();
		// Five states we can go into:
		//	0: start, goto on any but newline
		//	1: tag value, continue to munch until # or <
		//	2: open tag, continue to munch until >
		//	3: comment, scan until newline and return to 0
		//	4: close tag, continue to munch until >
		switch (state) {
		case 0: // START
			// look for '<', which starts either a tag or a close tag
			if (peekChar == '<') {
				getNextChar(); // munch the < so I can look at the next one
				peekChar = peekNextChar();
				if (peekChar == '/') { // Is this a closing tag
					getNextChar(); // munch the closing indicator
					state = 4; // yup, closing tag
				}
				else state = 2; // nope, this is an open tag
			}
			else if (peekChar == '#') { // Comment?
				getNextChar(); // munch the #
				state = 3;
			}
			else if (peekChar == (char)-1) { // EOF?
				tokenType = XML_EOF;
				done = true;
			}
			else if (peekChar == '\n') getNextChar(); // simply munch an endline
			else state = 1; // Guess we're a tag value
			break;
		case 1: // TAG VAL
			addChar = peekChar;
			if (addChar == 0x07) addChar = '<'; // Check for substitute char because of scanning for '<'
			if (peekChar != '\n' && peekChar != '<' && peekChar != '#') tokenData += addChar; // don't hold endlines
			if (peekChar == '<' || peekChar == '#') { // apparently this is the end of the value
				tokenType = XML_VALUE;
				done = true;
			}
			else getNextChar(); // munch the next char so we can look at it next time around
			break;
		case 2: // TAG
			if (peekChar == '>') { // are we done?
				getNextChar(); // munch the closing bracket
				tokenType = XML_TAG;
				done = true;
			}
			else if (peekChar == '\n') getNextChar(); // ignore newline
			else tokenData += getNextChar(); // otherwise just munch it and go on
			break;
		case 3: // COMMENT
			if (peekChar == '\n') state = 0; // on newline goto state 0
			getNextChar(); // otherwise just munch
			break;
		case 4: // CLOSE TAG
			if (peekChar == '>') { // are we done?
				getNextChar(); // munch the closing bracket
				tokenType = XML_CTAG;
				done = true;
			}
			else if (peekChar == '\n') getNextChar(); // ignore newline
			else tokenData += getNextChar(); // otherwise munch it
			break;
		}
	}
	return Token ( tokenType, tokenData );
}

// Return the next char without munching it
//	A (char)-1 return is EOF
char XMLParser::peekNextChar() {
    string checkLine = lines[lineIdx];
    int tempLineIdx = lineIdx;
    int tempLinePtr = linePtr+1;
    if (tempLinePtr == (int)(checkLine.size())) {// We're at the end of the line
        tempLineIdx++;
        if (tempLineIdx == (int)(lines.size())) return (char)-1;
        checkLine = lines[tempLineIdx];
        tempLinePtr = 0;
    }
    return checkLine[tempLinePtr];
}

// Munch the next char
//	A (char)-1 return is EOF
char XMLParser::getNextChar() {
    string checkLine = lines[lineIdx];
    linePtr++;
    if (linePtr == (int)(checkLine.size())) {// We're at the end of the line
        lineIdx++;
        if (lineIdx == (int)(lines.size())) return (char)-1;
        checkLine = lines[lineIdx];
        linePtr = 0;
    }
    return checkLine[linePtr];
}

// openTag writes an opening tag
//		<tagName>
void XMLParser::openTag(string tagName) {
	if (!openTags.empty()) *writeStream << endl;
	*writeStream << "<" << tagName << ">";
	openTags.push(tagName);
}

// writeTag writes the value for the most open tag
// several values may be written before the tag is closed
//		<tagname>value
void XMLParser::writeTag(string tagName, string value) {
	if (openTags.top() == tagName) {
		long pos = value.find("<",0);
		while (pos != -1) {
			value[pos] = 0x07; // BEL char, we can't have <
			pos = value.find("<",pos+1);
		}
		*writeStream << value;
	}
	// else throw exception
}

// closeTag writes a closing tag
// the tag may not be given a value after it is closed
//		<tagname>value</tagname>
void XMLParser::closeTag(string tagName) {
	if (openTags.top() == tagName) {
		*writeStream << "</" << tagName << ">";
		openTags.pop();
	}
	// else throw exception
}

// getCurrentIndex returns the pointer to the location
//	in the vector of lines
long XMLParser::getCurrentIndex() {
	return lineIdx;
}

// isTag returns true iff the current token is a tag
bool XMLParser::isTag(long idx) {
	return (nextToken.tokenType == XML_TAG);
}

// isCloseTag returns true iff the line contains a closing tag
bool XMLParser::isCloseTag(long idx) {
	return (nextToken.tokenType == XML_CTAG);
}

// readTag reads and opens a tag for reading its value
//	returns null string if the token is a close tag
string XMLParser::readTag() {
	// Looking for either an open or a close tag
	while (nextToken.tokenType == XML_VALUE) {
		getNextToken();
	}
	if (nextToken.tokenType == XML_CTAG) return ""; // don't advance
	Token returnToken = getNextToken(); // otherwise advance it
	return returnToken.data;
}

// readTagValue reads the value of the most open tag
string XMLParser::readTagValue(string tagName) {
	// Is the current token a value?  If not then don't
	//	do anything so we don't lose tags
	if (nextToken.tokenType != XML_VALUE) return "";
	Token returnToken = getNextToken(); // make sure to munch it
	return returnToken.data;
}

// readCloseTag closes the most open tag
string XMLParser::readCloseTag() {
	// Look for a close tag and then munch it
	while (nextToken.tokenType != XML_CTAG) getNextToken();
	Token returnToken = getNextToken();
	return returnToken.data;
}

// Debug function used to print out the vector of lines
void XMLParser::printAllLines(ostream& oss) {
	for (unsigned int i = 0; i < lines.size(); i++)
		oss << lines[i] << endl;
}
