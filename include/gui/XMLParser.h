/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   XMLParser: Scans and tokenizes an XML file
*****************************************************************************/

#ifndef XMLPARSER_H_
#define XMLPARSER_H_

#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>

using namespace std;

enum XMLTokenType {
	XML_TAG,
	XML_CTAG,
	XML_VALUE,
	XML_EOF
};

// Class Token:
//	holds information about a scanned token in an XML file.
//	tokenType describes the function of data.
class Token {
public:
	XMLTokenType tokenType;
	string data;
	// constructor:
	Token( XMLTokenType a, string& strData ) : tokenType(a), data(strData) {};
	Token( const Token& x ) { tokenType = x.tokenType; data = x.data; };
	Token() {};
};

// Class XMLParser:
//	This parser is finite-automata driven, scanning char by char.  Files are loaded
//	into memory as a series of lines, through which pointers are kept.  readTag returns
//	the next tag in the file, ignoring unread tag values and returning a null string if
//	a close tag is found first.  readCloseTag also ignores unread tag values and returns
//	the first close tag found.  readTagValue returns all data up to the beginning of a 
//	tag or a comment.
//	For writing, the second constructor is used, giving an output file.  Use the
//	openTag, writeTag, and closeTag functions to write to the file.
class XMLParser {
public:
	XMLParser(fstream*, bool writing = false);
	XMLParser(ostream*);
	virtual ~XMLParser();
	
	void openTag(string tagName);
	void writeTag(string tagName, string value);
	void closeTag(string tagName);

	string readTag();
	string readTagValue(string tagName);
	string readCloseTag();
	long getCurrentIndex();
	bool isTag(long);
	bool isCloseTag(long);
	
	void printAllLines(ostream&);
	
private:
	// Returns nextToken and advances the token (in that order).
	Token getNextToken();
	// Runs the FSM to get the next token in the file (EOF if end).
	Token scanNextToken();
	// Look at the next char of the file without munching it.
	//	Returns (char)-1 if EOF.
	char peekNextChar();
	// Munch the next char of the file.
	//	Returns (char)-1 if EOF.
	char getNextChar();
	
	// Keep track of position in file
	Token nextToken;
	int lineIdx; // Current line, defaults to 0
	int linePtr; // Current position in line, defaults to -1
	
	fstream* mStream;
	ostream* writeStream;
	stack < string > openTags;
	vector < string > lines;
};

#endif /*XMLPARSER_H_*/
