#ifndef INSTRUCTIONMANAGER_H_
#define INSTRUCTIONMANAGER_H_
#include<string>
#include<map>

//more ZAD constants ***************************************************
const int H_KEY_0 = 0x06;
const int H_KEY_1 = 0x05;
const int H_KEY_2 = 0x01;
const int H_KEY_3 = 0x08;
const int H_KEY_4 = 0x0C;
const int H_KEY_5 = 0x0B;
const int H_KEY_6 = 0x07;
const int H_KEY_7 = 0x09;
const int H_KEY_8 = 0x12;
const int H_KEY_9 = 0x17;
const int H_KEY_A = 0x02;
const int H_KEY_B = 0x03;
const int H_KEY_C = 0x18;
const int H_KEY_D = 0x11;
const int H_KEY_E = 0x04;
const int H_KEY_F = 0x0A;
const int H_KEY_NEXT = 0x22;
const int H_KEY_PREV = 0x1C;
const int H_KEY_GO = 0x1F;
const int H_KEY_STEP = 0x19;
const int H_KEY_DATA = 0x16;
const int H_KEY_SET_BRK_PT = 0x0E;
const int H_KEY_INSERT = 0x15;
const int H_KEY_DELETE = 0x1B;
const int H_KEY_PC = 0x13;
const int H_KEY_ADDR = 0x10;
const int H_KEY_CLR_BRK_PT = 0x14;
const int H_KEY_REG = 0x0D;
const int H_KEY_COPY = 0x21;
const int H_KEY_RELA = 0x0F;
const int H_KEY_DUMP = 0x20;
const int H_KEY_LOAD = 0x1A;
const int H_KEY_NONE = 0x00;

const int H_KEY_RESET = 0xFF;

const int S_KEY_NEXT = 0x10;
const int S_KEY_PREV = 0x11;
const int S_KEY_GO = 0x12;
const int S_KEY_STEP = 0x13;
const int S_KEY_DATA = 0x14;
const int S_KEY_SET_BRK_PT = 0x15;
const int S_KEY_INSERT = 0x16;
const int S_KEY_DELETE = 0x17;
const int S_KEY_PC = 0x18;
const int S_KEY_ADDR = 0x19;
const int S_KEY_CLR_BRK_PT = 0x1A;
const int S_KEY_REG = 0x1B;
const int S_KEY_COPY = 0x1C;
const int S_KEY_RELA = 0x1D;
const int S_KEY_DUMP = 0x1E;
const int S_KEY_LOAD = 0x1F;

#include"Z_80Registers.h"

enum InterruptCase{
	NMI_interrupt,
	INT_interrupt,
	DIDNT_interrupt
};

enum ZAD_MODE{
	ZAD_MODE_SMART,
	ZAD_MODE_OFF,
	ZAD_MODE_ON
};


#define OPCODE_TABLE_SIZE 65536
#define MAX_PROCEDURES 3

using namespace std;
class WaveformFSM;
class Z_80LogicGate;


class InstructionManager;

const int SCAN_KEY_TIME_OUT = 10000;

const int ADDRESS_ZERO = 0x0000;
const int KBRD_ADDRESS = 0x0FEB;
const int SCAN_ADDRESS = 0x0FEE;
const int HEXTO7_ADDRESS = 0x0FF1;
const int MESOUT_ADDRESS = 0x0FF4;
//const int ASCICV_ADDRESS = 0x0FF7;
const int TONE_ADDRESS = 0x0FFA;
const int DELAY_ADDRESS = 0x0FFD;

const int DISPV_ADDRESS = 0x1F12;

const int NUM_DISP_DIGITS = 6;
const int ZAD_START_ADDRESS = 0x1800;
const int ZAD_INT1_TARGET = 0x1F41;


//This is the typedef of a function pointer
//now funcPointer can be used to declare a
//function pointer
typedef void (InstructionManager::*funcPointer)(  Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );


//This class is mentioned in the Architectural Design section.  
//It is one of the levels in the  Z_80LogicGate.  
//It is responsible for directing the WaveformFSM 
//level on what waveform to do next.  This class 
//is where the instructions for the Z-80 are 
//implemented.  This class also sends changes 
//to the Z_80Registers level on what registers 
//to change.
class InstructionManager{
	
private:
	//The ZAD mode determines how the system will respond when the
	//PC reaches memory addresses which are known to be
	//system functions on the ZAD trainer boards.
	//SMART mode will only treat them as if they did contain
	//the ZAD system calls if there is a no-op in there locations.
	//ON mode will treat them as ZAD system calls wither or not
	//there is a no-op there.  OFF mode will case the system
	//calls to never happen even if there is a no-op in the location
	//and a subrutine calls that location.
	ZAD_MODE zadMode;
	
	//The auto1800 zad feature needs to be smart enough
	//so that it doesn't jump to 1800 when the program
	//is jumped up to 1800 inentionally.
	//auto1800 only makes sence right when the system
	//is starting up or when a reset happens.
	bool systemJustRestarted;

	//when this is set to true, the InstructionManager
	//should allert the Z_80LogicGate to pause
	//the simulation when the instructionManager is
	//inbetween instructions
	bool iStepModeEnabled;
	
	//This usually will always hold the first byte of the opcode
	unsigned long opcodeData;
	
	//tempData and tempData2 are used by functions which are called
	//in sequence by the function pointer that need to pass information
	//from one of them to the next
	int tempData;
	int tempData2;
	
	//this variable holds the current type of interupt
	InterruptCase interruptCase;

	//this class holds all the register data... hence the name
	//It is also reponsable for noteifying the gui side when
	//register changes are made.
	Z_80Registers z_80Registers;
	
	
	//This function is called by each implimentation
	//of a function to provide information to pass up
	//to the pop-up about the current executing instruction
	void describeInstruction( string oppName, string description, int n, Z_80LogicGate* z_80LogicGate );
	void describeInstruction( string oppName, string description, Z_80LogicGate* z_80LogicGate );
	

	//Because we can not always execute an instruction on just one fetch
	//from memory, we need to keep track of what to do next.
	//this is acomplished by this function pointer which points
	//at the next thing to do when more data is ready from the
	//WaveformFSM
	void (InstructionManager::*nextPart)(  Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//Instead of having a large switch statement that is based off of the opcode
	//we have a pointer array to functions.  Thus to preform an opcode, it is just
	//used to index the array and then the function is called.  This array has two dimentions
	//so that the instruction can use generic rutines which then refer to the next function
	//in the list
	map< unsigned long, funcPointer > opcodeHash;
	
	//some of opcodes have an aditional opcode byte which
	//comes after an argument to the instruction.  Because
	//we do not want to fit every permutation of the opcode
	//into the opcodeHash, this aditional hash is created.
	//A function which will recognize that we need to do
	//an extra lookup will be placed in opcodeHash so no
	//extra consideratoins need to be taken and this
	//hash will be referenced automatically.
	map< unsigned long, funcPointer > postfixedOpcodeHash;
	                     
	
	//This is called by the constructor to populate the opcodeTable
	void initOpcodeTable();
	
	//This is a method used by initOpcodeTable to construct the opcode from pieces
	unsigned long putPartIn( unsigned long rest, int part, int bytePosition );
	
	//This method takes a multi byte opcode and puts it in the opcodeHash several
	//times as a one byte opcode as a two byte opcode and as the three byte opcode
	//so that each time that only part of the opcode is looked up, the code
	//will know to fetch more opcode
	void hashTwoByteOpcode( unsigned long opcode, funcPointer targetFunction );

	//This function is stashed at opcode locations in the opcodeHash that are partial opcodes.
	//It stashes the current opcode and requests another
	void fetchMoreOpcode( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void fetchMoreOpcode_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );


	//This function takes an opcode that has a postfix to it
	//and hashes it correctly to the opcodeHash and the
	//postfixedOpcodeHash.
	void hashOpcodeWithPostFix( unsigned long twoBytePrefix, unsigned long postfix, funcPointer targetFunction );
	
	//This function is hashed in the opcodeHash hash by hashOpcodeWithPostFix so that the postfixedOpcodeHash
	//will be accessed.
	void fetchArgumentAndPostFix( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void fetchArgumentAndPostFix_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void fetchArgumentAndPostFix_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

   /**
   * This will extract from data a range of bits and return it as an integer
   * @param data the data containing the field
   * @param bitNum the index at which the right most bit resides.  Byte zerro is
   * the least significant (right most) byte of the data.
   * @param numBits the number of bits that pertain to the field.
   * @return the extracted field
   */
   //it is used by the functions below to extract fields out of the opcode
    int extractField( int data, int bitLocation, int bitLength );
    
    //This function is used to translate the way registers are refered to in the opcode
    //to how they are refered to in the Z_80Registers layer
    RSinglet translateReg8b( int regCode );
    
    //This function is used to to translate 16b values.  There are two different ways, the dd way
    //and the qq way
    RDoublet translateReg16bDD( int regCode );
    RDoublet translateReg16bQQ( int regCode );
    RDoublet translateReg16bSS( int regCode );
	RDoublet translateReg16bPP( int regCode );
	RDoublet translateReg16bRR( int regCode );
	
	//This function queries the waveformFSM to see if an interupt
	//has accured during the last instruction. This method is called
	//by the InstructionManager itself in the FetchInstruction method
	//and in the Halt method.
	//If this instruction returns true, then this instruction has
	//handled sending the waveformFSM its next command and has set
	//the nextPart pointer because it recieved an interupt.
	//if it returns false then there wasn't an interupt and the
	//calling function, (FetchInstruction or Halt) should
	//do thease things for themselves.
	bool checkInterupts( WaveformFSM* waveformFSM, Z_80LogicGate* z_80LogicGate );
	void checkInterupts_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void checkInterupts_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void checkInterupts_4( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void checkInterupts_5( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void checkInterupts_6( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
    
	//When an emulated instruction finnishes it will do one of two things.
	//If it has already ordered the waveformFSM to do something, it will set the next pointer
	//to this function.
	//If it hasn't, then it will call this function directly.
	void InstructionFetch( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void InstructionFetch_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//This takes the data passed in newData and interperates it as an opcode.
	//It then branches to an apropriate function acording to the opcode by looking
	//up the function pointer in the opcodeHash
	void InstructionDecode( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	
	void FetchOpcode( WaveformFSM* waveformFSM );
	
	//ZAD function emulating functions****************************************************
	char segmentVectorCArray[ NUM_DISP_DIGITS + 1 ]; //6 digits and a null
	
	//This function is the braking point which breakes out on the
	//special ZAD subrutines to emulate them in code.
	//There are two stipulations for the brake out to accure,
	//the pc must have just been incremented past the
	//special address and the opcode fetched from that location
	//must be a NOP.  This makes it so that someone can
	//create a genuine function at that location that doesn't
	//get short circuited
	void checkForZADCode( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//This scans the keyboard and refreshes the display.
	//it will not return until a keypad key is pressed
	void KBRD_EM( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void KBRD_EM_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
		
	//the ZAD trainer board expects programs to start at address 1800h.
	//This function reads in 0x0000 and if it is a no-op, it will
	//asume that the program is realy starting at 1800h and redirect the
	//program there.
	void AUTO_1800( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
			
	//This will scan the keyboard and refreshes the display exactly once,
	//and then returns to the caller immediaetly
	//the return value is placed in the A reg
	void SCAN_EM( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	
	//HEXTO7 Address: 0FF1H  Hex data found in the display
	//vector is converted to 7-segment numberic codes, and
	//copied intot the segment vector.
	
	//the first thing we need to do is grab the bytes out of
	//memory at the display vector
	void HEXTO7_EM( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void HEXTO7_EM_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void HEXTO7_EM_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void HEXTO7_EM_4( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//Prepares a 6-character ASCII message for display
	//IX should point to the first characters immediately
	//following the first
	void MESOUT_EM( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void MESOUT_EM_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	void TONE_EM( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//This will delay the number of 'millisecions' in HL
	//This will execute instruction fetches from its own memory
	//address to stall time.
	//It executes aproxamatly the same number of instructions
	//as the real one would have
	void DELAY_EM( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void DELAY_EM_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//The 'operating system' has code at this location to
	//jump to the address specified by the memory locations
	//1F41H and 1F42H
	void INT_MODE1_EM( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void INT_MODE1_EM_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void INT_MODE1_EM_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//Opcode functions***********************************
	
	//moves a 8bit value from one register to another
	void LD_r_rp( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//loads an 8bit immediate into a register
	void LD_r_n( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void LD_r_n_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//load from memory referenced by HL into target r
	void LD_r_IHLI( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData );
	void LD_r_IHLI_2( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData );
	
	//loads from memory dereferenced from offset from IX or IY register
	void LD_r_IIXYpdI( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData );
	void LD_r_IIXYpdI_2( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData );
	void LD_r_IIXYpdI_3( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData );

	//save to memory index at HL from register
	void LD_IHLI_r( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData );

	//save register into memory indexed by IX or by IY and an offset
	void LD_IIXYpdI_r( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData );
	void LD_IIXYpdI_r_2( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData );

	//load constant into location index by HL
	void LD_IHLI_n( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void LD_IHLI_n_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//saves immediate into memory indexed by (IX+d) or (IY+d) 
	void LD_IIXYpDI_n( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void LD_IIXYpDI_n_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void LD_IIXYpDI_n_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//loads into A memory referenced by BC or DE
	void LD_A_IBCDEI( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData );
	void LD_X_IBCDEI_2( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData );

	//load 8 bit reg A from memory indexed location
	void LD_A_InnI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void LD_A_InnI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void LD_A_InnI_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void LD_A_InnI_4( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//save to memory index at BC or DE from A
	void LD_IBCDEI_A( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData );

	//saves 8 bit reg A  to index memory location
	void LD_InnI_A( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void LD_InnI_A_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void LD_InnI_A_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//loads A with I
	void LD_A_I( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//loads A with R
	void LD_A_R( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//loads I with A
	void LD_I_A( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//loads R with A
	void LD_R_A( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	
	//16-Bit Load Group *****************************************
	RDoublet workingRegisterPair;
	
	//load a constent 16b literal into a register pair identified by 
	//the member variable workingRegisterPair
	void LD_XX_nn( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void LD_XX_nn_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void LD_XX_nn_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//LD dd,nn
	void LD_dd_nn( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//LD IX,nn
	//LD IY,nn
	void LD_IXY_nn( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//loads a 16b value in memory into a sixteen bit register pair specified by
	//the workingRegisterPair mem
	void LD_XX_InnI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void LD_XX_InnI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void LD_XX_InnI_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void LD_XX_InnI_4( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void LD_XX_InnI_5( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//LD HL,(nn)
	void LD_HL_InnI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//LD dd,(nn)
	void LD_dd_InnI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//LD IX,(nn)
	//LD IY,(nn)
	void LD_IXY_InnI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	void LD_InnI_XX( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void LD_InnI_XX_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void LD_InnI_XX_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void LD_InnI_XX_4( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	

	//LD (nn),HL
	void LD_InnI_HL( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//LD (nn),dd
	void LD_InnI_dd( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//LD (nn),IX
	//LD (nn),IY
	void LD_InnI_IXY( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//LD SP,HL
	void LD_SP_HL( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//LD SP,IX
	//LD SP,IY
	void LD_SP_IXY( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//pushes the the workingRegisterPair onto the stack
	void PUSH_XX( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void PUSH_XX_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//PUSH qq
	void PUSH_qq( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//PUSH IX
	//PUSH IY
	void PUSH_IXY( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//This pops a 16b value off of the stack and places it in the workingRegisterPair
	void POP_XX( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void POP_XX_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void POP_XX_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//POP qq
	void POP_qq( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//POP IX
	//POP IY
	void POP_IXY( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	
	//Exchange, Block Transfer, Block Search Groups ******************
	//EX DE,HL
	void EX_DE_HL( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
		
	//EX AF,AF'
	void EX_AF_AFP( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//EXX
	void EXX( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//exchanges the contents of the workingRegisterPair with the location indexed by the stack pointer
	void EX_ISPI_XX( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void EX_ISPI_XX_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void EX_ISPI_XX_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void EX_ISPI_XX_4( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
		
	//EX (SP),HL
	void EX_ISPI_HL( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
		
	//EX (SP),IX
	//EX (SP),IY
	void EX_ISPI_IXY( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//LDI
	//LDIR
	void LDIIRDDR( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void LDIIRDDR_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//CPI
	//CPIR
	//CPD
	//CPDR
	void CPIIRDDR( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void CPIIRDDR_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	
	
	//8-Bit Arithmetic and Logical Group******************************
	//This function takes the integer code for a particular type of arithmatic
	//operation and returns the name as a string.  This is used as part of 
	//the reporting mechanizem which the software tells the user what each
	//instruction is doing.
	string operationToString( int operation );

	//This funciton returns a portion of the description for different arithmatic
	//operations as strings.
	string operationToDescription( int operation );
		
	//preforms an 8b arithmatic operation on register a with the data
	//that comes in the newData argument
	void XXX_A_X( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	//ADD A,r
	//ADC A,r
	//SUB A,r
	//...
	void XXX_A_r( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
		
		
	//XXX A,n
	void XXX_A_n( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void XXX_A_n_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//XXX A,(HL)
	void XXX_A_IHLI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void XXX_A_IHLI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
		
	//XXX A,(IX+d)
	//XXX A,(IY+d)
	void XXX_A_IIXYPDI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void XXX_A_IIXYPDI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void XXX_A_IIXYPDI_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
		
	//INC r
	//DEC r
	void INCDEC_R( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//This function will increment or decriment the value addressed in memory by newData
	void INCDEC_IXXI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void INCDEC_IXXI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//INC (HL)
	//DEC (HL)	
	void INCDEC_IHLI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//INC (IX+d)
	//DEC (IX+d)
	//INC (IY+d)
	//DEC (IX+d)
	void INCDEC_IIXYPDI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void INCDEC_IIXYPDI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );	
	
	//General Purpose Arithmetic and CPU Control Groups***************
	
	//DDA
	void DDA( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//CPL
	void CPL( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//NEG
	void NEG( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//CCF
	void CFF( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//SCF
	void SCF( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//NOP
	void NOP( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//HALT
	void HALT( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void HALT_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//DI
	void DI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//EI
	void EI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//IM 0
	//IM 1
	//IM 2
	void IM_0_1_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//16-Bit Arithmetic Group ****************************************
	//ADD HL,ss
	void ADD_HL_ss( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//ADC HL,ss	
	void ADC_HL_ss( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//SBC HL,ss
	void SBC_HL_ss( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
		
	//ADD IX,pp
	//ADD IY,rr
	void ADD_IXY_pprr( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	
	//INC ss
	//DEC ss
	void INCDEC_ss( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	// INC IX
	// DEC IX
	// INC IY
	// DEC IY
	void INCDEC_IXY( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//Rotate and Shift Group********************************
	
	//returns (IX+d) or (IY+d) formated as a string with d as a number
	//is used by functions to print their description
	string printIXYpd( int d, bool hasPostFix );
	
	//RLCA
	void RLCA( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//RLA
	void RLA( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
		
	//RRCA
	void RRCA( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
		
	//RRA
	void RRA( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//This function takes a value rotates it according to the rotateMove.
	//This function also sets the flags
	int rotateize( int oldValue, int rotationAction );
	 
	//RLC r
	//RL r
	//RRC r
	//RR r
	//SLA r
	//SRA r
	//SRL r
	void ROTATE_R( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	 
	//RLC (HL)
	//RL (HL)
	//RRC (HL)
	//RR (HL)
	//SLA (HL)
	//SRA (HL)
	//SRL (HL)
	void ROTATE_IHLI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void ROTATE_IHLI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
		
	//RLC (IX+d)
	//RL (IX+d)
	//RRC (IX+d)
	//RR (IX+d)
	//SLA (IX+d)
	//SRA (IX+d)
	//SRL (IX+d)
	//RLC (IX+d)
	//RL (IY+d)
	//RRC (IY+d) 
	//RR (IY+d)
	//SLA (IY+d)
	//SRA (IY+d)
	//SRL (IY+d)
	void ROTATE_IIXYpDI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void ROTATE_IIXYpDI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//RLD
	void RLD( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void RLD_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
		
	//RRD
	void RRD( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void RRD_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//Bit Set, Reset and Test Group**************************
	//This function is used by the below bit functions
	//to do the testing of the bit and then setting the flags
	void BIT_TESTER( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//This determinds the ix+d or iy+d value by examining the current opcode
	int getIXYpDAddress();
	
	//BIT b,r
	void BIT_b_r( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//BIT b,(HL)
	void BIT_b_IHLI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//BIT b,(IX+d)
	//BIT b,(IY+d)
	void BIT_B_IIXYpDI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	int changeBitTo( int newBitValue, int oldValue, int bitPos );
	
	//SET b,r
	//RES b,r
	void SETRES_B_R( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );	
		
	//This function will do a bit set or reset on the value
	//passed in newData, and then will save it back to
	//the address saved in tempData;
	void SETRES_B_IXXI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );	
	
	//SET b,(HL)  
	//RES b,(HL)
	void SETRES_B_IHLI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	
	//SET b,(IX+d)
	//SET b,(IY+d)
	//RES b,(IX+d)
	//RES b,(IY+d)
	void SETRES_B_IIXYpDI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );

	//Jump Group*********************************************
	//This returns a string representation of a condition
	//code
	string conditionToString( int condition );
	
	//This returns a string containing a description of a condition
	string conditionToDescription( int condition );
	
	//JP nn
	void JP_nn( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void JP_nn_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void JP_nn_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	
	//JP cc,nn
	void JP_cc_nn( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void JP_cc_nn_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void JP_cc_nn_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//JR e
	void JR_e( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void JR_e_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );	
	
	//JR C,e
	void JR_C_e( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void JR_C_e_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//JR NC,e
	void JR_NC_e( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void JR_NC_e_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
		
	//JR Z,e
	void JR_Z_e( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void JR_Z_e_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	
	//JR NZ, e
	void JR_NZ_e( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void JR_NZ_e_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	
	//JP (HL)
	void JP_IHLI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
		
	//JP (IX)
	//JP (IY)
	void JP_IIXYI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//DJNZ, e
	void DJNZ_e( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void DJNZ_e_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );	



	//Call and Return Group**********************************
	//This function is used by CALL_nn and RET_cc to test
	//if their condition is true against the flags
	bool testCondition( int conditionToTest );
	
	//This function will call depending on the CC code in
	//the tempData reg.
	void CALL_XX_nn( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void CALL_XX_nn_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void CALL_XX_nn_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void CALL_XX_nn_4( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	
	//CALL nn
	void CALL_nn( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//CALL cc,nn
	void CALL_cc_nn( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//RET
	void RET( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void RET_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void RET_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//RET cc
	void RET_cc( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//RETI
	//RETN
	void RETIN( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//RST p
	void RST_p( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void RST_p_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//Input and Output Group*********************************
	//IN A,(n)
	void IN_A_INI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void IN_A_INI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void IN_A_INI_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	
	//IN F,(C)
	void IN_F_ICI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void IN_F_ICI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//IN r,(C)
	void IN_R_ICI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void IN_R_ICI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//INI
	void INI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void INI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
		
	//INIR
	void INIR( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void INIR_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void INIR_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//IND
	void IND( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void IND_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//INDR
	void INDR( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void INDR_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void INDR_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//OUT (n),A
	void OUT_INI_A( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void OUT_INI_A_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//OUT (C),r
	void OUT_ICI_R( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
		
	//OUTI
	void OUTI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void OUTI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//OTIR
	void OTIR( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void OTIR_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void OTIR_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//OUTD
	void OUTD( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void OUTD_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//OTDR
	void OTDR( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void OTDR_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	void OTDR_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	
public:
	InstructionManager( Z_80LogicGate* z_80LogicGate );

	//this function is used by the WaveformFSM so that the
	//instrucitonManager can instruct it on what waveform to
	//produce next.  newData is all the different types of
	//data that the waveformFSM can harvice from the data lines
	//this includes opcodes, memory reads, port reads and
	//interupt reads.  The data is interperated by what the
	//instruction manager last asked for.
	void requestInstruction( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData );
	
	//this function returns the current value of a named
	//register.  This function is called by WaveformFSM::getReg
	string getReg( string name );
	
	//this method is so that the user can manipulate the values of the
	//registers.  It is called by waveformFSM:setReg
	void setReg( string regName, string newValue );
	
	//this method tells the instruction manager wither or not it
	//should tell the Z_80LogicGate to block at the end of each instruction
	//this method is called by the WaveformFSM::setRunMode 
	void setIStepMode( bool shouldStep );
	
	//this method is called by the waveformFSM when it has been
	//told by the Z_80LogicGate it has been time to reset
	void doReset( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM );
	
	//sets the current zad mode.  See the definition over the zadMode
	//private member variable
	void setZADMode( ZAD_MODE newZadMode, Z_80LogicGate* z_80LogicGate );
	
	//returns the current ZAD_MODE. See the definition over the zadMode
	//private member variable
	ZAD_MODE getZADMode();
};

#endif /*INSTRUCTIONMANAGER_H_*/
