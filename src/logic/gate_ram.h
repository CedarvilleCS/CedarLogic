#pragma once

#include "logic_gate.h"

// ***************** n-bit by n-bit RAM Gate *******************
class Gate_RAM : public Gate
{
public:
	// Initialize the ram:
	Gate_RAM();

	// Handle gate events:
	void gateProcess( void );

	// Set the parameters:
	bool setParameter( string paramName, string value );

	// Get the parameters:
	string getParameter( string paramName );

	// Write a file containing the memory data:
	void outputMemoryFile( string fName );

	// Read a file and load the memory data:
	void inputMemoryFile( string fName );
	
//*************************************************
//Edit by Joshua Lansford 1/22/06
//This will make it so that the logic gate can
//also load Intel Hex files.  This is a format
//which is exported by the zad assembler.
	void inputMemoryFileFromIntelHex( string fName );
	
	//a helper function
	int readInHex( ifstream* fin, int numChars );
//End of edit**************************************

protected:
	unsigned long dataBits;
	unsigned long addressBits;

	map< unsigned long, unsigned long > memory;
	
	//This is the last location that a read has
	//taken place from.
	unsigned long lastRead;
};