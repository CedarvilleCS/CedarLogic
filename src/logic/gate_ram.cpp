#include "gate_ram.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <cmath>

//for the ram gate **** by Joshua Lansford**********
#define DATA_RECORD_HEX 0x00
#define END_OF_FILE_HEX 0x01
//*************************************************

// ******************************** RAM GATE ***********************************


// Initialize the starting state and the output:
Gate_RAM::Gate_RAM( ) : Gate() {

	// Declare the stationary pins:
	declareInput( "write_clock", true );
	declareInput( "write_enable" );
	
	// NOTE: None of the other pins are declared in advance!
	// They are created in setParameter, because they depend on the RAM's size!

	// Set the RAM's default size:	
	setParameter( "ADDRESS_BITS", "0" );
	setParameter( "DATA_BITS", "0" );
	
	lastRead = (unsigned long)-1;
}


// Handle gate events:
void Gate_RAM::gateProcess( void ) {
	
	// Don't do the process unless there are address and data lines declared!
	if( (addressBits == 0) || (dataBits == 0) ) return;

	unsigned long address = bus_to_ulong( getInputBusState("ADDRESS") );
	unsigned long dataIn = bus_to_ulong( getInputBusState("DATA_IN") );

//***********************************************************************
//Edit by Joshua Lansford 12/31/06
//Purpose of edit:  When the logic gate loads from a file, it is not
//   possable to directly notify the gui by paramiters about the new
//   data.  Thus instead a flag is set and we update the gui now
    if( flushGuiMemory ){
    	flushGuiMemory = false;
		listChangedParam( "MemoryReset" );
		for( map< unsigned long, unsigned long >::iterator I = memory.begin();
		     I != memory.end();  ++I ){
		    ostringstream virtualPropertyName;
			virtualPropertyName << "Address:";
			virtualPropertyName << I->first; //we just list the address
			listChangedParam( virtualPropertyName.str() );
		}
    }
//End of Edit************************************************************

	if( getInputState("write_enable") == ONE ) {
		// HI_Z all of the data outputs:
		vector< StateType > allHI_Z( dataBits, HI_Z );
		setOutputBusState( "DATA_OUT", allHI_Z );
		
		if( isRisingEdge("write_clock") ) {
			// Write to the RAM.
			memory[address] = dataIn;
			ostringstream oss;
			oss << "Wroted to the memory thing: Address = " << address << ", data = " << dataIn;
			WARNING(oss.str());
//***********************************************************************
//Edit by Joshua Lansford 12/31/06
//Purpose of edit:  The Cedar-logic ram gate is being expanded to
//have a popup that shows the contents of the memory
//therefore it is necisary for the logic gate to tell the gui gate
//every time data changes in it.
				ostringstream virtualPropertyName;
				virtualPropertyName << "Address:";
				virtualPropertyName << address;
				listChangedParam( virtualPropertyName.str() );
//End of edit************************************************************
		}
	} else {
		// Read from the RAM, and write the data to the outputs.
		vector< StateType > ramReadData = ulong_to_bus( memory[address], dataBits );
		setOutputBusState( "DATA_OUT", ramReadData );
//***********************************************************************
//Edit by Joshua Lansford 4/22/06
//Purpose of edit:  This allerts the pop-up when ever an address has changed
		if( getInputState("ENABLE_0") == ONE ){
			lastRead = address;
			listChangedParam( "lastRead" );
		}
//End of edit******************************************************
	}
}


// Set the parameters:
bool Gate_RAM::setParameter( string paramName, string value ) {
	istringstream iss(value);
	if( paramName == "ADDRESS_BITS" ) {
		iss >> addressBits;

		// Declare the address pins!		
		if( addressBits > 0 ) {
			declareInputBus( "ADDRESS", addressBits );
		}

		//NOTE: Don't return "true" from this or DATA_BITS, because
		// you shouldn't be setting this param during simulation while
		// anything is connected anyhow!
	} else if( paramName == "DATA_BITS" ) {
		iss >> dataBits;

		// Declare the data input and output pins!
		if( dataBits > 0 ) {
			declareInputBus( "DATA_IN", dataBits );
			declareOutputBus( "DATA_OUT", dataBits );
		}
	} else if( paramName == "WRITE_FILE" ) {
		outputMemoryFile(value);
	} else if( paramName == "READ_FILE" ) {
		if( value.substr( value.length() - 3 ) == "cdm" ){ 
			inputMemoryFile(value);
		//*********************************************
		//Edit by Joshua Lansford. 1/22/06
		//This extends the ram gate so that it can open
		//intel hex files as well.
		//the if that goes with this else was added as
		//well.
		}else{
			inputMemoryFileFromIntelHex( value );
		}
		
		//End of edit**********************************
			
		return true;
	//*******************************************
	//Edit by Joshua Lansford 4/22/07
	//This edit is so that the pop-up can send changes
	//down to the core by editing cells
	} else if( paramName.substr( 0, 8 ) == "Address:" ){
		istringstream addressExtractor( paramName.substr( 8 ) );
		unsigned long addressOfNewData = 0;
		addressExtractor >> addressOfNewData;
		
		unsigned long newData;
		iss >> newData;
		
		if( memory[ addressOfNewData ] != newData ){
			memory[ addressOfNewData ] = newData;
			//now we will re list the param so
			//that the change will bounce back up into
			//the pop-up.
			listChangedParam( paramName );
		}
		return true;
	//********************************************
	
	//********************************
	//Edit by Joshua Lansford 4/22/07
	//This idet is so that the pop-up can know
	//when ever there is a read.
	}else if( paramName == "lastRead" ){
		iss >> lastRead;
	//End of edit******************************
	}else{
		return Gate::setParameter( paramName, value );
	}
	return false;
}


// Set the parameters:
string Gate_RAM::getParameter( string paramName ) {
	ostringstream oss;
	if( paramName == "ADDRESS_BITS" ) {
		oss << addressBits;
		return oss.str();
	} else if( paramName == "DATA_BITS" ) {
		oss << dataBits;
		return oss.str();
//***********************************************************************
//Edit by Joshua Lansford 12/31/06
//Purpose of edit:  The Cedar-logic ram gate is being expanded to
//have a popup that shows the contents of the memory
//therefore it is necisary for the logic gate to tell the gui gate
//every time data changes in it.
	} else if( paramName.substr( 0, 8 ) == "Address:" ){
		istringstream iss( paramName.substr( 8 ) );
		unsigned long addressOfDataToReturn = 0;
		iss >> addressOfDataToReturn;
		unsigned long dataToReturn = memory[ addressOfDataToReturn ];
		ostringstream oss;
		oss << dataToReturn;
		
		
		return oss.str();
	}else if( paramName == "MemoryReset" ){
		return "true";
//End of edit************************************************************
	//********************************
	//Edit by Joshua Lansford 4/22/07
	//This idet is so that the pop-up can know
	//when ever there is a read.
	}else if( paramName == "lastRead" ){
		oss << lastRead;
		return oss.str();
	//End of edit******************************
	} else {
		return Gate::getParameter( paramName );
	}
}

// Write a file containing the memory data:
void Gate_RAM::outputMemoryFile( string fName ) {
	ofstream oFile( fName.c_str() );
	if(!oFile) {
		WARNING("Gate_RAM::outputMemoryFile() - Couldn't open the memory file for writing.");
		return;
	}
	
	oFile << "# CEDAR Logic RAM memory file." << endl;
	oFile << "# The file format is:" << endl;
	oFile << "# \"hex_address : hex_data\"" << endl;
	oFile << "#" << endl;
	oFile << "# For example, if the address \"0\" contained the decimal" << endl;
	oFile << "# number \"18\", then the line for that address would be:" << endl;
	oFile << "# \"0 : 12\"" << endl << "#" << endl;
	oFile << "# Note that if a memory location is not represented here," << endl;
	oFile << "# then it is assumed to contain the data value \"0\"" << endl << endl;
	
	// Loop through all of the memory locations that have been written or read and dump the memory data:
	map< unsigned long, unsigned long >::iterator memLoc = memory.begin();
	while( memLoc != memory.end() ) {
		// Format: "hex_address : hex_data"
		// (Do uppercase hex characters.)
		oFile.setf(ios::hex, ios::basefield);
		oFile.setf(ios::uppercase);
		oFile << hex << (memLoc->first) << " : " << hex << (memLoc->second) << endl;
		memLoc++;
	}
	
	oFile.close();
}

// Read a file and load the memory data:
void Gate_RAM::inputMemoryFile( string fName ) {
	ifstream iFile( fName.c_str() );
	if(!iFile) {
		WARNING("Gate_RAM::inputMemoryFile() - Couldn't open the memory file for reading.");
		return;
	}

	// Clear the old memory before loading the new one:
	memory.clear();


//********************************************************
//Edit by Joshua Lansford 12/31/06
//Purpose of edit: Let the gui reset its copy of the 
//     memory too
	flushGuiMemory = true;
//End of edit*********************************************

	string temp;
	istringstream theLine;
	unsigned long address = 0, data = 0;
	char dump;
	while (!iFile.eof()) {
	    getline(iFile, temp, '\n');
	    if (temp[0] != '#') {
	    	// This line is a legitimate parseable line - not a comment line.
	    	
	    	// Try to parse the line:
	    	theLine.clear();

	    	
	    	theLine.str(temp);
			theLine.setf(ios::hex, ios::basefield); // The input data is in hex format
	    	theLine >> address >> dump >> data;
	    	if(!theLine.fail()) {
	    		// If the line parsed correctly, then set the memory value:
	    		memory[address] = data;
	    	}
	    }
	}
}

//*************************************************
//Edit by Joshua Lansford 1/22/06
//This will make it so that the logic gate can
//also load Intel Hex files.  This is a format
//which is exported by the zad assembler.
void Gate_RAM::inputMemoryFileFromIntelHex( string fName ){
	ifstream fin( fName.c_str(), ios::in );
	
	bool endOfFile = false;
	char temp = '0';
	int byteCount = 0;
	int nextByte = 0;
	int addressPointer = 0;
	int recordType = 0;
	
	if( fin ){
		//check to make sure the file actually exists before
		//we blow our last data
		memory.clear();
		flushGuiMemory = true;
	}else{
		//TODO: This needs to be able to become visible to the
		//user somehow.  An idea would be to make an error property
		//that gets displayed in an allert box when it changes.
		cout << "Error reading file.  Empty or non-existant?" << endl;
	}
	
	while( !endOfFile && fin ){
		//here we will process a record
		//first we make sure that the first character is a ":"
		fin >> temp;
		if( temp == ':' ){
			byteCount = readInHex( &fin, 2 );
			addressPointer = readInHex( &fin, 4 );
			recordType = readInHex( &fin, 2 );
			
			switch( recordType ){
				case DATA_RECORD_HEX:
					for( int byteNum = 0; byteNum < byteCount; ++byteNum ){
						nextByte = readInHex( &fin, 2 );
						memory[ addressPointer + byteNum ] = nextByte;
					}
				
					break;
				case END_OF_FILE_HEX:
					endOfFile = true;
					break;
				default:
					//TODO: cout doesn't cut the cheeze
					cout << "Unexpected record type: " << recordType << endl;
					endOfFile = true;
					break;
			}
			
			//dump checksum
			readInHex( &fin, 2 );
		}else{
			//TODO: cout doesn't cut the cheeze
			cout << "Error reading file: Expected ':'" << endl;
			endOfFile = true;
		}
	}
	
	fin.close();
}

//helper function that allows reading in hex files
int Gate_RAM::readInHex( ifstream* fin, int numChars ){
	int result = 0;
	char nextChar = '0';
	int charValue = 0;
	for( int i = 0; i < numChars; ++i ){
		(*fin) >> nextChar;
		if( nextChar >= '0' && nextChar <= '9' ){
			charValue = nextChar - '0';
		}else if( nextChar >= 'A' && nextChar <= 'F' ){
			charValue = nextChar - 'A' + 10;
		}else if( nextChar >= 'a' && nextChar <= 'f' ){
			charValue = nextChar - 'a' + 10;
		}else{
			//TODO: cout doesn't cut the cheeze
			cout << "non hex character in stream: " << nextChar << endl;
			charValue = 0;
		}
		result <<= 4;
		result |= charValue;
	}
	return result;
}
	
//End of edit**************************************


// **************************** END RAM GATE ***********************************
