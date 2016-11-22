#ifndef Z_80REGISTERS_H_
#define Z_80REGISTERS_H_

#include<string>

#define S_FLAG	0x80
#define Z_FLAG	0x40
#define H_FLAG	0x10
#define P_V_FLAG	0x04
#define N_FLAG	0x02
#define C_FLAG	0x01

//these are for setFlags
#define HOLD 0
#define TEST 1
#define CLEAR 2
#define RESET 2 //same as clear
#define SET 3
#define DNTCARE 4
#define NOTUSED 4 //same as DNTCARE

#define BYTE_SIZE 0
#define WORD_SIZE 1
//
#define EIGHT_BIT_MAX_VALUE	0xFF
#define SIXTEEN_BIT_MAX_VALUE 0xFFFF
#define BITS_IN_NIBBLE		4
#define BITS_IN_BYTE 		8
#define BITS_IN_WORD		16
#define SIGN_8BIT_MASK	0x80
#define SIGN_16BIT_MASK 0x8000

const int IM_MODE_0_CODE = 0x0;
const int IM_MODE_1_CODE = 0x2;
const int IM_MODE_2_CODE = 0x3;

const int CC_NZ = 0x0;
const int CC_Z  = 0x1;
const int CC_NC = 0x2;
const int CC_C  = 0x3;
const int CC_PO = 0x4;
const int CC_PE = 0x5;
const int CC_P  = 0x6;
const int CC_M  = 0x7;
const int CC_T  = 0x8;

const int NUM_CCs = 8;

const int P_00H = 0x0;
const int P_08H = 0x1;
const int P_10H = 0x2;
const int P_18H = 0x3;
const int P_20H = 0x4;
const int P_28H = 0x5;
const int P_30H = 0x6;
const int P_38H = 0x7;

using namespace std;

class Z_80LogicGate;

#define NUM_REG_SINGLETS 8
enum RSinglet {
	aReg = 0,
	fReg,
	bReg,
	cReg,
	dReg,
	eReg,
	hReg,
	lReg,
	iReg,
	rReg
};

#define NUM_DOUBLETS 8
enum RDoublet{
	bcReg = 0,
	deReg,
	hlReg,
	spReg,
	pcReg,
	afReg,
	ixReg,
	iyReg
};

class EightBitReg{
private:
	string name;
	int value;
	bool mute;
	Z_80LogicGate* z_80LogicGate;
	
public:
    //constructor
	EightBitReg();

	//accessor methods.  Methods with a pair
	//make it so that you can change register values
	//in pairs such as HL
	//mute is set to true if this is being used as a sub part of a sixteen bit register
	void setData( string newName, Z_80LogicGate* newZ_80LogicGate, bool newMute = false );
	int getValue();
	string getValueString();
	int getValue( EightBitReg* pair );
	string getValueString( EightBitReg* pair );
	void setValue( int newData );
	void setValueString( string newData );
	void setValue( EightBitReg* pair, int newData );
	void setValueString( EightBitReg* pair, string newData );
};

class SixteenBitReg{
private:
	string name;
	EightBitReg msPart;
	EightBitReg lsPart;
	Z_80LogicGate* z_80LogicGate;
public:
	SixteenBitReg();
	void setData( string newName, Z_80LogicGate* newZ_80LogicGate );
	int getValue();
	string getValueString();
	void setValue( int newData );
	void setValueString( string newData );
};
	
	

class Z_80Registers{
private:
	
	//registers A, B, C, D ... and F
	EightBitReg singlets[ NUM_REG_SINGLETS ];
	
	EightBitReg I;
	EightBitReg R;

	//registers A', B', C' ...
	EightBitReg singletsPrime[ NUM_REG_SINGLETS ];
	
	//SP, PC ...
	SixteenBitReg SP;
	SixteenBitReg PC;
	SixteenBitReg IX;
	SixteenBitReg IY;
	
	//interrupt flip-flops status
	bool IFF1Set;
	bool IFF2Set;
	int IMF;
	
	//This pointer is so that
	//the IMF and IFF1/2 can
	//properly signal when the have changed.
	Z_80LogicGate* z_80LogicGate;

public:
	//these are helper functions for the flag functions
	//below
	bool isPositive( int var, int varType );
	int getMagnitude( int var, int varType );
	int getNigation( int var, int varType );
	int getSignExtend( int var, int varType );
	
private:
	//these are used to construct a new flags value
	//by oring their results together
	int testSFlag( int result, int varType );
	int testZFlag( int result, int varType );
	int testHFlag( int lhs, int rhs, bool wasAdd, bool hasCarry );
	int testHFlag( int lhs, int rhs, bool wasAdd );
	int testPFlag( int result, int varType );
	int testVFlag( int lhs, int rhs, bool wasAdd, bool hasCarry, int varType );
	int testVFlag( int lhs, int rhs, bool wasAdd, int varType );
	int testNFlag( bool wasAdd );
	int testCFlag( int lhs, int rhs, bool wasAdd, bool hasCarry, int varType );
	int testCFlag( int lhs, int rhs, bool wasAdd, int varType );
		
	//these test the current state of the flags in the F
	//registor.
	int testSFlag();
	int testZFlag();
	int testHFlag();
	int testPVFlag();
	int testNFlag();
	int testCFlag();
	
	//this tests a specified string condition against the flags
	bool conditionMet( string condition );
	
public:
	//constructor.
	Z_80Registers();
	
	//The registers are told who they are so that they
	//can automatically update properties in the z_80LogicGate
	void initRegisters( Z_80LogicGate* z_80LogicGate );
	

	//this method is a shortcut way for methods to set the flags
	void setFlags( int result, int varType, int lhs, int rhs, bool wasAdd, bool carryIn, int SF, int ZF, int HF, int PF, int VF, int NF, int CF );

	//This method takes care of the reset option which the reset pin does.
	//Note:  this does not clear all registers, instead it
	//initializes the CPU as follows: it resets the
	//interrupt enable flip-flop, clears the PC and
	//Registers I and R, and sets the interupt status
	//to Mode 0.
	void resetData( Z_80LogicGate* z_80LogicGate );
		
	//thase functions get and set the interupt mode code
	//0 stands for mode 0, 2 stands for mode 1 and 3
	//stands for mode 2.
	void setIntModeCode( int newModeCode );
	
	//sets the interupt mode code as axplained above
	int getIntModeCode();
		
	void inc8b( RSinglet reg );
	void inc16b( RDoublet reg );
	void dec8b( RSinglet reg );
	void dec16b( RDoublet reg );
	void add16b( RDoublet dest, RDoublet source, bool withCarry = false);
	void addImm8b( RSinglet dest, int rhs, bool withCarry = false );
	void sub8b( RSinglet source, bool withBarrow = false );
	void sbcHL( RDoublet rhs );
	void rlc8b( RSinglet reg );
	void exchangeAF();
	void exchangeBCDEHL();
	int get8b( RSinglet reg );
	string get8bName( RSinglet reg );
	int get16b( RDoublet reg );
	string get16bName( RDoublet reg );
	string getReg( string reg );
	bool getIFF1();
	bool getIFF2();
	void setIFF1( bool newValue );
	void setIFF2( bool newValue );
	void set8b( RSinglet reg, int newValue );
	void set16b( RDoublet reg, int newValue );
	void setReg( string reg, string newValue );
	void rrc8b( RSinglet reg );
	void rl8b( RSinglet reg );
	void rr8b( RSinglet reg );
	void BCDUpdate();
	void jr( int offset );
	void neg();
};


#endif /*Z_80REGISTERS_H_*/
