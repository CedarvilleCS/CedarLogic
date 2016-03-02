#include<iostream>
#include<iomanip>

#include"Z_80Registers.h"
#include"Z_80LogicGate.h"



using namespace std;

EightBitReg::EightBitReg(){
	z_80LogicGate = NULL;
}

//this gives the eightBitRegister a name so that when
//its value changes it can automatically tell the user that its value changed
void EightBitReg::setData( string newName, Z_80LogicGate* newZ_80LogicGate, bool newMute ){
	value = 0;
	name = newName;
	z_80LogicGate = newZ_80LogicGate;
	mute = newMute;
}


int EightBitReg::getValue(){
	return value;
}

string EightBitReg::getValueString(){
	ostringstream stream;
	stream << uppercase << hex << setfill('0') << setw(2) << getValue();
	return stream.str();
}

int EightBitReg::getValue( EightBitReg* pair ){
	return ( value | (pair->getValue() << BITS_IN_BYTE) );
}

string EightBitReg::getValueString( EightBitReg* pair ){
	ostringstream stream;
	stream << uppercase << hex << setfill('0') << setw(2) << getValue( pair );
	return stream.str();
}

void EightBitReg::setValue( int newData ){
	value = (newData & 0xFF);
	
	if( !mute && z_80LogicGate != NULL ){
		z_80LogicGate->notifyOfRegisterChange( name );
	}
}

void EightBitReg::setValueString( string newData ){
	istringstream stream( newData );
	int dataAsInt = 0;
	stream >> hex >> dataAsInt;
	setValue( dataAsInt );
}

void EightBitReg::setValue( EightBitReg* pair, int newData ){
	value = newData & 0xFF;
	pair->setValue( newData >> BITS_IN_BYTE );
	
	if( !mute && z_80LogicGate != NULL ){
		z_80LogicGate->notifyOfRegisterChange( name );
	}
}

void EightBitReg::setValueString( EightBitReg* pair, string newData ){
	istringstream stream( newData );
	int dataAsInt = 0;
	stream >> hex >> dataAsInt;
	setValue( pair, dataAsInt );
}
//----------------------------------------------------------------

SixteenBitReg::SixteenBitReg(){
	z_80LogicGate = NULL;
}

void SixteenBitReg::setData( string newName, Z_80LogicGate* newZ_80LogicGate ){
	msPart.setData( "msp", newZ_80LogicGate, true );
	lsPart.setData( "lsp", newZ_80LogicGate, true );
	name = newName;
	z_80LogicGate = newZ_80LogicGate;
}
int SixteenBitReg::getValue(){
	return lsPart.getValue( &msPart );
}
string SixteenBitReg::getValueString(){
	ostringstream stream;
	stream << uppercase << hex << setfill('0') << setw(4) << getValue();
	return stream.str();
}
	
void SixteenBitReg::setValue( int newData ){
	lsPart.setValue( &msPart, newData );
	if( z_80LogicGate != NULL ){
		z_80LogicGate->notifyOfRegisterChange( name );
	}
}

void SixteenBitReg::setValueString( string newData ){
	istringstream stream( newData );
	int dataAsInt = 0;
	stream >> hex >> dataAsInt;
	setValue( dataAsInt );
}

//----------------------------------------------------------------

Z_80Registers::Z_80Registers(){
	z_80LogicGate = NULL;
}

//The registers are told who they are so that they
//can automatically update properties in the z_80LogicGate
void Z_80Registers::initRegisters( Z_80LogicGate* z_80LogicGate ){
	//Originally this could not be done in the constructor because
	//we could not send properties up to the gui until we were being
	//simulated.  Now the save method is used instead, which auto
	//matically logs property changes until next time something
	//changes.
	this->z_80LogicGate = z_80LogicGate;
	
	singlets[aReg].setData( "A", z_80LogicGate );
	singlets[fReg].setData( "F", z_80LogicGate );
	singlets[bReg].setData( "B", z_80LogicGate );
	singlets[cReg].setData( "C", z_80LogicGate );
	singlets[dReg].setData( "D", z_80LogicGate );
	singlets[eReg].setData( "E", z_80LogicGate );
	singlets[hReg].setData( "H", z_80LogicGate );
	singlets[lReg].setData( "L", z_80LogicGate );
	I.setData( "I", z_80LogicGate );
	R.setData( "R", z_80LogicGate );
	
	singletsPrime[aReg].setData( "A'", z_80LogicGate );
	singletsPrime[fReg].setData( "F'", z_80LogicGate );
	singletsPrime[bReg].setData( "B'", z_80LogicGate );
	singletsPrime[cReg].setData( "C'", z_80LogicGate );
	singletsPrime[dReg].setData( "D'", z_80LogicGate );
	singletsPrime[eReg].setData( "E'", z_80LogicGate );
	singletsPrime[hReg].setData( "H'", z_80LogicGate );
	singletsPrime[lReg].setData( "L'", z_80LogicGate );
	
	SP.setData( "SP", z_80LogicGate );	
	PC.setData( "PC", z_80LogicGate );
	IX.setData( "IX", z_80LogicGate );	
	IY.setData( "IY", z_80LogicGate );
	
	setIntModeCode( IM_MODE_0_CODE );
	setIFF1( false );
	setIFF2( false );
}

bool Z_80Registers::isPositive( int var, int varType ){
	if( (varType == BYTE_SIZE && (var&SIGN_8BIT_MASK)==SIGN_8BIT_MASK ) ||
	    (varType == WORD_SIZE && (var&SIGN_16BIT_MASK)==SIGN_16BIT_MASK ) ){
		return false; //if the sign bit is set, it is not positive
	}else{
		return true;
	}
}
int Z_80Registers::getMagnitude( int var, int varType ){
	if( isPositive( var, varType ) ){
		if( varType == BYTE_SIZE ){
			return var & 0xFF;
		}else{
			return var & 0xFFFF;
		}
	}else{
		return getNigation( var, varType );
	}
}
int Z_80Registers::getNigation( int var, int varType ){
	if( varType == BYTE_SIZE ){
		return (-var) & 0xFF;
	}else{
		return (-var) & 0xFFFF;
	}
}
int Z_80Registers::getSignExtend( int var, int varType ){
	int answer = 0;
	if( isPositive( var, varType ) ){
		answer = var;
	}else{
		if( varType == BYTE_SIZE ){
			answer = (-1-0xFF) | var;
		}else{
			answer = (-1-0xFFFF) | var;
		}
	}
	return answer;
}

int Z_80Registers::testSFlag( int result, int varType ){
	if( !isPositive( result, varType ) ){
		return S_FLAG;
	}else{
		return 0;
	}
}
int Z_80Registers::testZFlag( int result, int varType ){
	if( (varType == BYTE_SIZE && (result & 0xFF)== 0) ||
	    (varType == WORD_SIZE && (result & 0xFFFF)==0) ){
		return Z_FLAG;
	}else{
		return 0;
	}
}

int Z_80Registers::testHFlag( int lhs, int rhs, bool wasAdd, bool hasCarry ){
	if( testHFlag( lhs, rhs, wasAdd ) == H_FLAG ){
		return H_FLAG;
	}else if( hasCarry ){
		if( wasAdd ){
			return testHFlag( lhs+rhs, 1, true );
		}else{
			return testHFlag( lhs-rhs, -1, false );
		}
	}else{
		return 0;
	}
}

int Z_80Registers::testHFlag( int lhs, int rhs, bool wasAdd ){
	int lhsFirstNibble = lhs & 0x0F;
	int rhsFirstNibble = rhs & 0x0F;
	if( (wasAdd && (lhsFirstNibble + rhsFirstNibble > 0x0F )) ||
	   (!wasAdd && (lhsFirstNibble < rhsFirstNibble ) ) ){
		return H_FLAG;
	}else{
	 	return 0;
   	}
}

int Z_80Registers::testPFlag( int result, int varType ){
	//test for even parity
	bool isEven = true;
	int bitMask = 1;
	int numBits = (varType==BYTE_SIZE)?BITS_IN_BYTE:BITS_IN_WORD;
	//cycle through all bits and test if they
	//are set flipping the isEven flag each
	//time
	for( int i = 0; i < numBits; ++i ){
		if( (result & bitMask) == bitMask ){
			isEven = !isEven;
		}
		bitMask <<= 1;
	}
	if( isEven ){
		return P_V_FLAG;
	}else{
		return 0;
	}
}


int Z_80Registers::testVFlag( int lhs, int rhs, bool wasAdd, bool hasCarry, int varType ){
	if( testVFlag( lhs, rhs, wasAdd, varType ) == P_V_FLAG ){
		return P_V_FLAG;
	}else if( hasCarry ){
		if( wasAdd ){
			return testVFlag( lhs+rhs, 1, true, varType );
		}else{
			return testVFlag( lhs-rhs, -1, false, varType );
		}
	}else{
		return 0;
	}
}

int Z_80Registers::testVFlag( int lhs, int rhs, bool wasAdd, int varType ){
	bool hasOverflow = false;
	int sum = 0;
	
	if( varType==BYTE_SIZE ){
		lhs &= 0xFF;
		rhs &= 0xFF;		
	}else{
		lhs &= 0xFFFF;
		rhs &= 0xFFFF;
	}
	
	if( !wasAdd ) rhs = getNigation( rhs, varType );
	sum = (lhs + rhs) & 0xFF;
	
  	if( isPositive( lhs, varType ) && isPositive( rhs, varType ) ){
  		if( !isPositive( sum, varType ) ){
  			hasOverflow = true;
  		}
  	}else if( !isPositive( lhs, varType ) && !isPositive( rhs, varType ) ){
  		if( isPositive( sum, varType ) ){
  			hasOverflow = true;
  		}
  	}else{
  		int posValue = lhs;
  		int negValue = rhs;
  		if( isPositive( rhs, varType ) ){
  			posValue = rhs;
  			negValue = lhs;
  		}
  		if( getNigation( negValue, varType ) > posValue ){
  			if( isPositive( sum, varType ) ){
  				hasOverflow = true;
  			}
  		}else{
  			if( !isPositive( sum, varType ) ){
  				hasOverflow = true;
  			}
  		}
  	}
  	return hasOverflow?P_V_FLAG:0;
}
int Z_80Registers::testNFlag( bool wasAdd ){
	return wasAdd?0:Z_FLAG;
}

int Z_80Registers::testCFlag( int lhs, int rhs, bool wasAdd, bool hasCarry, int varType ){
	if( testCFlag( lhs, rhs, wasAdd, varType ) == C_FLAG ){
		return C_FLAG;
	}else if( hasCarry ){
		if( wasAdd ){
			return testCFlag( lhs+rhs, 1, true, varType );
		}else{
			return testCFlag( lhs-rhs, -1, false, varType );
		}
	}else{
		return 0;
	}
}

int Z_80Registers::testCFlag( int lhs, int rhs, bool wasAdd, int varType ){
	//make sure there isn't any extra bits hanging around
	if( varType==BYTE_SIZE ){
		lhs &= 0xFF;
		rhs &= 0xFF;		
	}else{
		lhs &= 0xFFFF;
		rhs &= 0xFFFF;
	}
	
	if( wasAdd ){
		if( ( varType == BYTE_SIZE && lhs + rhs > EIGHT_BIT_MAX_VALUE ) ||
	    ( varType == WORD_SIZE && lhs + rhs > SIXTEEN_BIT_MAX_VALUE )) {
			return C_FLAG;
		}else{
			return 0;
		}
	}else{
		if( lhs < rhs ){
			return C_FLAG;
		}else{
			return 0;
		}
	}
}
//these test the current state of the flags in the F
//registor
int Z_80Registers::testSFlag(){
	return S_FLAG & singlets[fReg].getValue();
}
int Z_80Registers::testZFlag(){
	return Z_FLAG & singlets[fReg].getValue();
}
int Z_80Registers::testHFlag(){
	return H_FLAG & singlets[fReg].getValue();
}
int Z_80Registers::testPVFlag(){
	return P_V_FLAG & singlets[fReg].getValue();
}
int Z_80Registers::testNFlag(){
	return N_FLAG & singlets[fReg].getValue();
}
int Z_80Registers::testCFlag(){
	return C_FLAG & singlets[fReg].getValue();
}

//this method is a shortcut way for methods to set the flags
void Z_80Registers::setFlags( int result, int varType, int lhs, int rhs, bool wasAdd, bool carryIn, int SF, int ZF, int HF, int PF, int VF, int NF, int CF ){
	int newFlags = 0;
	if( SF == HOLD ){
		newFlags |= testSFlag();
	}else if( SF == TEST ){
		newFlags |= testSFlag( result, varType );
	}else if( SF == CLEAR || SF == DNTCARE ){
	}else if( SF == SET ){
		newFlags |= S_FLAG;
	}
	
	if( ZF == HOLD ){
		newFlags |= testZFlag();
	}else if( ZF == TEST ){
		newFlags |= testZFlag( result, varType );
	}else if( ZF == CLEAR || ZF == DNTCARE ){
	}else if( ZF == SET ){
		newFlags |= Z_FLAG;
	}
	
	if( HF == HOLD ){
		newFlags |= testHFlag();
	}else if( HF == TEST ){
		newFlags |= testHFlag( lhs, rhs, wasAdd, carryIn );
	}else if( HF == CLEAR || HF == DNTCARE ){
	}else if( HF == SET ){
		newFlags |= H_FLAG;
	}
	if( PF == HOLD ){
		newFlags |= testPVFlag();
	}else if( PF == TEST ){
		newFlags |= testPFlag( result, varType );
	}else if( PF == CLEAR || PF == DNTCARE ){
	}else if( PF == SET ){
		newFlags |= P_V_FLAG;
	}
	
	if( VF == HOLD ){
		newFlags |= testPVFlag();
	}else if( VF == TEST ){
		newFlags |= testVFlag( lhs, rhs, wasAdd, carryIn, varType );
	}else if( VF == CLEAR || VF == DNTCARE ){
	}else if( VF == SET ){
		newFlags |= P_V_FLAG;
	}
	
	if( NF == HOLD ){
		newFlags |= testNFlag();
	}else if( NF == TEST ){
		newFlags |= testNFlag( result );
	}else if( NF == CLEAR || NF == DNTCARE ){
	}else if( NF == SET ){
		newFlags |= N_FLAG;
	}
	
	if( CF == HOLD ){
		newFlags |= testCFlag();
	}else if( CF == TEST ){
		newFlags |= testCFlag( lhs, rhs, wasAdd, carryIn, varType );
	}else if( CF == CLEAR || CF == DNTCARE ){
	}else if( CF == SET ){
		newFlags |= C_FLAG;
	}
	
	set8b(fReg, newFlags );
}

//This method takes care of the reset option which the reset pin does.
//Note:  this does not clear all registers, instead it
//initializes the CPU as follows: it resets the
//interrupt enable flip-flop, clears the PC and
//Registers I and R, and sets the interupt status
//to Mode 0.
void Z_80Registers::resetData( Z_80LogicGate* z_80LogicGate ){
	setIFF1( false );
	setIFF2( false );
	set16b( pcReg, 0x0000 );
	set8b( iReg, 0x00 );
	set8b( rReg, 0x00 );
	setIntModeCode( IM_MODE_0_CODE );
}


//thase functions get and set the interupt mode code
//0 stands for mode 0, 2 stands for mode 1 and 3
//stands for mode 2.
void Z_80Registers::setIntModeCode( int newModeCode ){
	IMF = newModeCode;
	z_80LogicGate->notifyOfRegisterChange( "IMF" );
}

//sets the interupt mode code as axplained above
int Z_80Registers::getIntModeCode(){
	return IMF;
}





//this tests a specified string condition against the flags
bool Z_80Registers::conditionMet( string condition ){
	int flags = singlets[fReg].getValue();
	return 
	  ( ( condition == "NZ" && ((flags & Z_FLAG) != Z_FLAG) ) ||
	    ( condition ==  "Z" && ((flags & Z_FLAG) == Z_FLAG) ) ||
	    ( condition == "NC" && ((flags & C_FLAG) != C_FLAG) ) ||
	    ( condition ==  "C" && ((flags & C_FLAG) == C_FLAG) ) ||
	    ( condition == "PO" && ((flags&P_V_FLAG)!=P_V_FLAG) ) ||
	    ( condition == "PE" && ((flags&P_V_FLAG)==P_V_FLAG) ) ||
	    ( condition ==  "P" && ((flags & S_FLAG) != S_FLAG) ) ||
	    ( condition ==  "M" && ((flags & S_FLAG) == S_FLAG) ) ||
	    ( condition ==  "T" ) );
}

void Z_80Registers::inc8b( RSinglet reg ){
	int oldValue = get8b( reg );
	int newValue = oldValue + 1;
	set8b( reg, newValue );
	//only set the flags if we are modifying an 8bit value
	//16 bit values don't modify the flags
	setFlags( newValue, BYTE_SIZE, oldValue, 1, true, false, TEST, TEST, TEST, NOTUSED, TEST, RESET, HOLD );
}
void Z_80Registers::inc16b( RDoublet reg ){
	int oldValue = get16b( reg );
	int newValue = oldValue + 1;
	set16b( reg, newValue );
	//16 bit values don't modify the flags
}
void Z_80Registers::dec8b( RSinglet reg ){
	int oldValue = get8b( reg );
	int newValue = oldValue - 1;
	set8b( reg, newValue );
	//only set the flags if we are modifying an 8bit value
	//16 bit values don't modify the flags
	//we don't test with one because some 8bit specifications
	setFlags( newValue, BYTE_SIZE, oldValue, 1, false, false, TEST, TEST, TEST, DNTCARE, TEST, SET, HOLD );
}
void Z_80Registers::dec16b( RDoublet reg ){
	int oldValue = get16b( reg );
	int newValue = oldValue - 1;
	set16b( reg, newValue );
	//only set the flags if we are modifying an 8bit value
	//16 bit values don't modify the flags
	//we don't test with one because some 8bit specifications
}

void Z_80Registers::addImm8b( RSinglet dest, int rhs, bool withCarry ){
	int lhs = get8b( dest );
	int newValue = lhs + rhs;
	if( withCarry ) newValue++;
	set8b( dest, newValue );
	if( dest == aReg ){
		setFlags(newValue, BYTE_SIZE, lhs, rhs, true, withCarry, TEST, TEST, TEST, DNTCARE, TEST, CLEAR, TEST );
	}
}

void Z_80Registers::add16b( RDoublet lhs, RDoublet rhs, bool withCarry ){
	int lhsValue = get16b( lhs );
	int rhsValue = get16b( rhs );
	int newValue = lhsValue + rhsValue;
	bool hadCarry = false;
	if( withCarry && (testCFlag() == C_FLAG) ){
		newValue++;
		hadCarry = true;
	}
	set16b( lhs, newValue );
	if( lhs == hlReg ){
		if( !withCarry ){
			setFlags( newValue, WORD_SIZE, lhsValue, rhsValue, true, hadCarry, HOLD, HOLD, DNTCARE, DNTCARE, HOLD, CLEAR, TEST );
		}else{
			setFlags( newValue, WORD_SIZE, lhsValue, rhsValue, true, hadCarry, TEST, TEST, DNTCARE, DNTCARE, TEST, CLEAR, TEST );
		}
	}else if( lhs == ixReg || lhs == iyReg ){
		setFlags( newValue, WORD_SIZE, lhsValue, rhsValue, hadCarry, withCarry, HOLD, HOLD, DNTCARE, DNTCARE, HOLD, CLEAR, TEST );
	}
}

void Z_80Registers::sbcHL( RDoublet rhs ){
	int lhsValue = get16b( hlReg );
	int rhsValue = get16b( rhs );
	int newValue = lhsValue - rhsValue;
	bool hadBarrow = false;
	if( testCFlag() == C_FLAG ){
		newValue--;
		hadBarrow = true;
	}
	set16b( hlReg, newValue );
	setFlags( newValue, WORD_SIZE, lhsValue, rhsValue, true, hadBarrow, TEST, TEST, DNTCARE, NOTUSED, TEST, SET, TEST );
}

void Z_80Registers::rlc8b( RSinglet reg ){
	int lastValue = get8b( reg );
	bool signBitWasSet = (lastValue & SIGN_8BIT_MASK) == SIGN_8BIT_MASK;
	int newValue = (lastValue << 1) & 0xFF;
	if( signBitWasSet ){
		newValue |= 1;
	}
	set8b( reg, newValue );
	
	int newFlags = 0;
	//the accumilator rlc sets different flags some reason
	if( reg == aReg ){
		newFlags |= testSFlag(); //hold S
		newFlags |= testZFlag(); //hold Z
								//clear H
		newFlags |= testPVFlag(); //hold P/V
		 						//clear N
	}else{
		newFlags |= testSFlag( newValue, BYTE_SIZE );
		newFlags |= testZFlag( newValue, BYTE_SIZE );
		newFlags |= testPFlag( newValue, BYTE_SIZE );
	}
	if( signBitWasSet ){
		newFlags |= C_FLAG; //Test C
	}
	set8b(fReg, newFlags );
}


void Z_80Registers::exchangeAF(){
	int temp = singlets[ aReg ].getValue();
	singlets[ aReg ].setValue( singletsPrime[ aReg ].getValue() );
	singletsPrime[ aReg ].setValue( temp );
	int temp2 = singlets[ fReg ].getValue();
	singlets[ fReg ].setValue( singletsPrime[ fReg ].getValue() );
	singletsPrime[ fReg ].setValue( temp2 );
	//doesn't affect flags
}

//preforms the exx command
void Z_80Registers::exchangeBCDEHL(){
	for( int regToSwap = bReg; regToSwap <= hReg; ++regToSwap ){
		int temp = singlets[ regToSwap ].getValue();
		singlets[ regToSwap ].setValue( singletsPrime[ regToSwap ].getValue() );
		singletsPrime[ regToSwap ].setValue( temp );
	}
	//doesn't affect flags
}

//returns an 8b register value
int Z_80Registers::get8b( RSinglet reg ){
	if( reg != iReg && reg != rReg ){
		return singlets[reg].getValue();
	}else if( reg == iReg ){
		return I.getValue();
	}else{
		return R.getValue();
	}
}

//retruns a name for the 8b register
string Z_80Registers::get8bName( RSinglet reg ){
	string answer;
	switch( reg ){	
		case aReg:
			answer = "A";
			break;
		case fReg:
			answer = "F";
			break;
		case bReg:
			answer = "B";
			break;
		case cReg:
			answer = "C";
			break;
		case dReg:
			answer = "D";
			break;
		case eReg:
			answer = "E";
			break;
		case hReg:
			answer = "H";
			break;
		case lReg:
			answer = "L";
			break;
		case iReg:
			answer = "I";
			break;
		case rReg:
			answer = "R";
			break;
	}
	return answer;
}

int Z_80Registers::get16b( RDoublet reg ){
	int answer = 0;
	switch( reg ){
		case bcReg:
			answer = singlets[cReg].getValue( &singlets[bReg] );
			break;
		case deReg:
			answer = singlets[eReg].getValue( &singlets[dReg] );
			break;
		case hlReg:
			answer = singlets[lReg].getValue( &singlets[hReg] );
			break;
		case spReg:
			answer = SP.getValue();
			break;
		case pcReg:
			answer = PC.getValue();
			break;
		case afReg:
			answer = singlets[fReg].getValue( &singlets[aReg] );
			break;
		case ixReg:
			answer = IX.getValue();
			break;
		case iyReg:
			answer = IY.getValue();
			break;
	}
	return answer;	
}

string Z_80Registers::get16bName( RDoublet reg ){
	string answer;
	switch( reg ){
		case bcReg:
			answer = "BC";
			break;
		case deReg:
			answer = "DE";
			break;
		case hlReg:
			answer = "HL";
			break;
		case spReg:
			answer = "SP";
			break;
		case pcReg:
			answer = "PC";
			break;
		case afReg:
			answer = "AF";
			break;
		case ixReg:
			answer = "IX";
			break;
		case iyReg:
			answer = "IY";
			break;
	}
	return answer;
}

string Z_80Registers::getReg( string reg ){
	string answer = "";
	if( reg == "A" ){
		answer = singlets[ aReg ].getValueString();
	}else if( reg == "F" ){
		answer = singlets[ fReg ].getValueString();
	}else if( reg == "B" ){
		answer = singlets[ bReg ].getValueString();
	}else if( reg == "C" ){
		answer = singlets[ cReg ].getValueString();
	}else if( reg == "D" ){
		answer = singlets[ dReg ].getValueString();
	}else if( reg == "E" ){
		answer = singlets[ eReg ].getValueString();
	}else if( reg == "H" ){
		answer = singlets[ hReg ].getValueString();
	}else if( reg == "L" ){
		answer = singlets[ lReg ].getValueString();
	}else if( reg == "I" ){
		answer = I.getValueString();
	}else if( reg == "R" ){
		answer = R.getValueString();
	}else if( reg == "A'" ){
		answer = singletsPrime[ aReg ].getValueString();
	}else if( reg == "F'" ){
		answer = singletsPrime[ fReg ].getValueString();
	}else if( reg == "B'" ){
		answer = singletsPrime[ bReg ].getValueString();
	}else if( reg == "C'" ){
		answer = singletsPrime[ cReg ].getValueString();
	}else if( reg == "D'" ){
		answer = singletsPrime[ dReg ].getValueString();
	}else if( reg == "E'" ){
		answer = singletsPrime[ eReg ].getValueString();
	}else if( reg == "H'" ){
		answer = singletsPrime[ hReg ].getValueString();
	}else if( reg == "L'" ){
		answer = singletsPrime[ lReg ].getValueString();
	}else if( reg == "BC" ){
		answer = singlets[ cReg ].getValueString( &singlets[ bReg ] );
	}else if( reg == "DE" ){
		answer = singlets[ eReg ].getValueString( &singlets[ dReg ] );
	}else if( reg == "HL" ){
		answer = singlets[ lReg ].getValueString( &singlets[ hReg ] );
	}else if( reg == "PC" ){
		answer = PC.getValueString();
	}else if( reg == "SP" ){
		answer = SP.getValueString();
	}else if( reg == "IX" ){
		answer = IX.getValueString();
	}else if( reg == "IY" ){
		answer = IY.getValueString();
	}else if( reg == "IFF1" ){
		answer = getIFF1()?"1":"0";
	}else if( reg == "IFF2" ){
		answer = getIFF2()?"1":"0";
	}else if( reg == "IMF" ){
		ostringstream stream;
		stream << hex << getIntModeCode();
		answer = stream.str();
	}
	return answer;
}

//returns an interupt flip flop value
bool Z_80Registers::getIFF1(){
	return IFF1Set;
}
bool Z_80Registers::getIFF2(){
	return IFF2Set;
}
void Z_80Registers::setIFF1( bool newValue){
	IFF1Set = newValue;
	z_80LogicGate->notifyOfRegisterChange( "IFF1" );
}
void Z_80Registers::setIFF2( bool newValue ){
	IFF2Set = newValue;
	z_80LogicGate->notifyOfRegisterChange( "IFF2" );
}


void Z_80Registers::set8b( RSinglet reg, int newValue ){
	if( reg != iReg && reg != rReg ){
		singlets[ reg ].setValue( newValue );
	}else if( reg == iReg ){
		I.setValue( newValue );
	}else{
		R.setValue( newValue );
	}
}

void Z_80Registers::set16b( RDoublet reg, int newValue ){
	switch( reg ){
		case bcReg:
			singlets[ cReg ].setValue( &singlets[ bReg ], newValue );
			break;
		case deReg:
			singlets[ eReg ].setValue( &singlets[ dReg ], newValue );
			break;
		case hlReg:
			singlets[ lReg ].setValue( &singlets[ hReg ], newValue );
			break;
		case spReg:
			SP.setValue( newValue );
			break;
		case pcReg:
			PC.setValue( newValue );
			break;
		case afReg:
			singlets[ fReg ].setValue( &singlets[ aReg ], newValue );
			break;
		case ixReg:
			IX.setValue( newValue );
			break;
		case iyReg:
			IY.setValue( newValue );
			break;
		default:
			cerr << "unknown register reference: " << newValue << endl;
			break;
	}
}

void Z_80Registers::setReg( string reg, string newValue ){
	if( reg == "A" ){
		singlets[ aReg ].setValueString( newValue );
	}else if( reg == "F" ){
		singlets[ fReg ].setValueString( newValue );
	}else if( reg == "B" ){
		singlets[ bReg ].setValueString( newValue );
	}else if( reg == "C" ){
		singlets[ cReg ].setValueString( newValue );
	}else if( reg == "D" ){
		singlets[ dReg ].setValueString( newValue );
	}else if( reg == "E" ){
		singlets[ eReg ].setValueString( newValue );
	}else if( reg == "H" ){
		singlets[ hReg ].setValueString( newValue );
	}else if( reg == "L" ){
		singlets[ lReg ].setValueString( newValue );
	}else if( reg == "I" ){
		I.setValueString( newValue );
	}else if( reg == "R" ){
		R.setValueString( newValue );
	}else if( reg == "A'" ){
		singletsPrime[ aReg ].setValueString( newValue );
	}else if( reg == "F'" ){
		singletsPrime[ fReg ].setValueString( newValue );
	}else if( reg == "B'" ){
		singletsPrime[ bReg ].setValueString( newValue );
	}else if( reg == "C'" ){
		singletsPrime[ cReg ].setValueString( newValue );
	}else if( reg == "D'" ){
		singletsPrime[ dReg ].setValueString( newValue );
	}else if( reg == "E'" ){
		singletsPrime[ eReg ].setValueString( newValue );
	}else if( reg == "H'" ){
		singletsPrime[ hReg ].setValueString( newValue );
	}else if( reg == "L'" ){
		singletsPrime[ lReg ].setValueString( newValue );
	}else if( reg == "BC" ){
		singlets[ cReg ].setValueString( &singlets[ bReg ], newValue );
	}else if( reg == "DE" ){
		singlets[ eReg ].setValueString( &singlets[ dReg ], newValue );
	}else if( reg == "HL" ){
		singlets[ lReg ].setValueString( &singlets[ hReg ], newValue );
	}else if( reg == "PC" ){
		PC.setValueString( newValue );
	}else if( reg == "SP" ){
		SP.setValueString( newValue );
	}else if( reg == "IX" ){
		IX.setValueString( newValue );
	}else if( reg == "IY" ){
		IY.setValueString( newValue );
	}else if( reg == "IFF1" ){
		setIFF1( newValue == "1" );
	}else if( reg == "IFF2" ){
		setIFF2( newValue == "1" );
	}else if( reg == "IMF" ){
		istringstream stream( newValue );
		int val;
		stream >> val;
		setIntModeCode( val );
	}
}
void Z_80Registers::rrc8b( RSinglet reg ){
	int lastValue = get8b( reg );
	bool lspWasSet = (lastValue & 0x01) == 0x01;
	int newValue = (lastValue >> 1) & 0xFF;
	if( lspWasSet ){
		newValue |= SIGN_8BIT_MASK;
	}
	set8b( reg, newValue );
	
	int newFlags = 0;
	//the accumilator rlc sets different flags some reason
	if( reg == aReg ){
		newFlags |= testSFlag(); //hold S
		newFlags |= testZFlag(); //hold Z
								//clear H
		newFlags |= testPVFlag(); //hold P/V
		 						//clear N
	}else{
		newFlags |= testSFlag( newValue, BYTE_SIZE );
		newFlags |= testZFlag( newValue, BYTE_SIZE );
		newFlags |= testPFlag( newValue, BYTE_SIZE );
	}
	if( lspWasSet ){
		newFlags |= C_FLAG; //Test C
	}
	set8b(fReg, newFlags );
}
void Z_80Registers::rl8b( RSinglet reg ){
	int lastValue = get8b( reg );
	bool signBitWasSet = (lastValue & SIGN_8BIT_MASK) == SIGN_8BIT_MASK;
	int newValue = (lastValue << 1) & 0xFF;
	if( testCFlag() == C_FLAG ){
		newValue |= 1;
	}
	set8b( reg, newValue );
	
	int newFlags = 0;
	//the accumilator rlc sets different flags some reason
	if( reg == aReg ){
		newFlags |= testSFlag(); //hold S
		newFlags |= testZFlag(); //hold Z
								//clear H
		newFlags |= testPVFlag(); //hold P/V
		 						//clear N
	}else{
		newFlags |= testSFlag( newValue, BYTE_SIZE );
		newFlags |= testZFlag( newValue, BYTE_SIZE );
		newFlags |= testPFlag( newValue, BYTE_SIZE );
	}
	if( signBitWasSet ){
		newFlags |= C_FLAG; //Test C
	}
	set8b(fReg, newFlags );
}
void Z_80Registers::rr8b( RSinglet reg ){
	int lastValue = get8b( reg );
	bool lspWasSet = (lastValue & 0x01) == 0x01;
	int newValue = (lastValue >> 1) & 0xFF;
	if( testCFlag() == C_FLAG ){
		newValue |= SIGN_8BIT_MASK;
	}
	set8b( reg, newValue );
	
	int newFlags = 0;
	//the accumilator rlc sets different flags some reason
	if( reg == aReg ){
		newFlags |= testSFlag(); //hold S
		newFlags |= testZFlag(); //hold Z
								//clear H
		newFlags |= testPVFlag(); //hold P/V
		 						//clear N
	}else{
		newFlags |= testSFlag( newValue, BYTE_SIZE );
		newFlags |= testZFlag( newValue, BYTE_SIZE );
		newFlags |= testPFlag( newValue, BYTE_SIZE );
	}
	if( lspWasSet ){
		newFlags |= C_FLAG; //Test C
	}
	set8b( fReg, newFlags );
}

void Z_80Registers::BCDUpdate(){
	//heres the plan.  First we will undo the incorrect type of borrowing
	//or carrying  and then we will do the BCD type of borrowing or carrying
	int flags = singlets[fReg].getValue();
	int msNibble = (singlets[aReg].getValue() & 0xF0) >> 4;
	int lsNibble = singlets[aReg].getValue() & 0x0F;
	//first test to see if there was an incorrect type of borrowing or
	//carrying
	if( (flags & H_FLAG) == H_FLAG ){
		if( (flags & N_FLAG) == N_FLAG ){
			//this means that the prevoise operation was a subtract
			//thus the incorect manuver was a barrow
			//we will fix it.
			msNibble++;
			lsNibble -= 0x10;  //this will make our lsNibble negative
			                   //we will take care of it in a bit.
		}else{
			//this means that the previouse operation was an add
			//thus the incorrect manuver was a carry
			msNibble--;
			lsNibble += 0x10;
		}
	}
	
	//we should also test for a carry out.
	if( (flags & C_FLAG) == C_FLAG ){
		if( (flags & N_FLAG) == N_FLAG ){
			//recovering from a subtract
			msNibble -= 0x0F;
		}else{
			//recovering from an add
			msNibble += 0x0F;
		}
	}
	
	
	//we will keep track of what we did with thease
	//so we can set the flags correctly when we are done
	bool didHalfCarry = false;
	bool didCarryOut = false;
	
	//now we need to make the recovered state into a valid BCD number
	//first we will take care of negative numbers
	if( (flags & N_FLAG) == N_FLAG ){
		//half barrow
		while( lsNibble < 0 ){
			lsNibble += 10;
			msNibble--;
			didHalfCarry = true;
		}
		//barrow out
		while( msNibble < 0 ){
			msNibble += 10;
			didCarryOut = true;
		}
	}else{
		//half carry
		while( lsNibble > 9 ){
			lsNibble -= 10;
			msNibble++;
			didHalfCarry = true;
		}
		//carry out
		while( msNibble > 9 ){
			msNibble -= 10;
			didCarryOut = true;
		}
	}
	
	//finnaly we save our value
	int newValue = ((msNibble << 4 ) | lsNibble);
	set8b( aReg,  newValue  );
	
	//now it is time to set the flags
	int newFlags = 0;
	//test S
	newFlags |= testSFlag( newValue, BYTE_SIZE );
	//test Z
	newFlags |= testZFlag( newValue, BYTE_SIZE );
	//test H
	if( didHalfCarry ) newFlags |= H_FLAG;
	//test P
	newFlags |= testPFlag( newValue, BYTE_SIZE );
	//hold N
	if( (flags & N_FLAG) == N_FLAG ) newFlags |= N_FLAG;
	//test C
	if( didCarryOut ) newFlags |= C_FLAG;
	
	set8b( fReg, newFlags );	
}
void Z_80Registers::jr(  int offset ){
	offset = getSignExtend( offset, BYTE_SIZE );
	set16b( pcReg, get16b( pcReg ) + offset );
	//no flags to set
}

void Z_80Registers::neg(){
	int a = get8b( aReg );
	int ap = getNigation( a, BYTE_SIZE );
	set8b( aReg, ap );
	setFlags( ap, BYTE_SIZE, 0, a, false, false, TEST, TEST, TEST, NOTUSED, TEST, SET, TEST );
}
