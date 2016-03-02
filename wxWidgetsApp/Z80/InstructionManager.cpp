#include<iostream>
#include<iomanip>

#include"InstructionManager.h"
#include"Z_80LogicGate.h"
#include"Z_80Registers.h"

using namespace std;

const int NUMBER_OF_R_REGISTERS = 7;
const int B_REG = 0;
const int C_REG = 1;
const int D_REG = 2;
const int E_REG = 3;
const int H_REG = 4;
const int L_REG = 5;
const int A_REG = 7; //6  skipped intensionally
const int SINGLE_RS[NUMBER_OF_R_REGISTERS] = { B_REG, C_REG, D_REG, E_REG, H_REG, L_REG, A_REG };

const int SINGLE_BIT_LENGTH = 3;

const int NMI_RESTART_LOCATION = 0x0066;
const int INT_MODE_1_RESTART_LOCATION = 0x0038;

//LD r,r'
const int LD_R_RP_R_POS = 3;
const int LD_R_RP_RP_POS = 0;
const int LD_R_RP_BASE_OPP = 0x40;

//LD r,n
const int LD_R_N_BASE_OPP = 0x06;
const int LD_R_N_R_POS = 3;

//LD r,(HL)
const int LD_R_IHLI_BASE_OPP = 0x46;
const int LD_R_IHLI_R_POS = 3;

//LD r,(IX + d)
//LD r,(IY + d)
const int LD_R_IIXYpDI_BASE_OPP = 0xDD46;
const int LD_R_IIXYpDI_R_POS = 3;
          
//LD (HL),r
const int LD_IHLI_R_BASE_OPP = 0x70;
const int LD_IHLI_R_R_POS = 0;

//LD (IX+d),r
//LD (IY+d),r  
const int LD_IIXYpDI_R_BASE_OPP = 0xDD70;
const int LD_IIXYpDI_R_R_POS = 0;
  
//LD (HL),n
const int LD_IHLI_N_BASE_OPP = 0x36;

//LD (IX+d),n
//LD (IY+d),n
const int LD_IIXYpDI_N_BASE_OPP = 0xDD36;

//LD A,(BC)
const int LD_A_IBCI_BASE_OPP = 0x0A;

//LD A,(DE)
const int LD_A_IDEI_BASE_OPP = 0x1A;

//LD A,(nn)
const int LD_A_INNI_BASE_OPP = 0x3A;

//LD (BC),A
//LD (DE),A
const int LD_IBCDEI_A_BASE_OPP = 0x02;
const unsigned int DE_MASK = 16;

const int LD_IDEI_A_BASE_OPP = 0x12;

//LD (nn),A
const int LD_INNI_A_BASE_OPP = 0x32;

//LD A,I
const int LD_A_I_BASE_OPP = 0xED57;

//LD A,R
const int LD_A_R_BASE_OPP = 0xED5F;

//LD I,A
const int LD_I_A_BASE_OPP = 0xED47;

//LD R,A
const int LD_R_A_BASE_OPP = 0xED4F;

//16-Bit Load Group *****************************************
const unsigned int IY_MASK = 0x2000;
const int DD_POS = 4;
const int DD_LENGTH = 2;
const int QQ_POS = 4;
const int QQ_LENGTH = 2;

//LD dd,nn
const int LD_DD_NN_BASE_OPP = 0x01;

//LD IX,nn
//LD IY,nn
const int LD_IXY_NN_BASE_OPP = 0xDD21;

//LD HL,(nn)
const int LD_HL_INNI_BASE_OPP = 0x2A;

//LD dd,(nn)
const int LD_DD_INNI_BASE_OPP = 0xED4B;

//LD IX,(nn)
//LD IY,(nn)
const int LD_IXY_INNI_BASE_OPP = 0xDD2A;

//LD (nn),HL
const int LD_INNI_HL_BASE_OPP = 0x22;

//LD (nn),dd
const int LD_INNI_DD_BASE_OPP = 0xED43;

//LD (nn),IX
//LD (nn),IY
const int LD_INNI_IXY_BASE_OPP = 0xDD22;

//LD SP,HL
const int LD_SP_HL_BASE_OPP = 0xF9;

//LD SP,IX
//LD SP,IY
const int LD_SP_IXY_BASE_OPP = 0xDDF9;

//PUSH qq
const int PUSH_QQ_BASE_OPP = 0xC5;

//PUSH IX
//PUSH IY
const int PUSH_IXY_BASE_OPP = 0xDDE5;

//POP qq
const int POP_QQ_BASE_OPP = 0xC1;

//POP IX
//POP IY
const int POP_IXY_BASE_OPP = 0xDDE1;


//Exchange, Block Transfer, Block Search Groups ******************

//EX DE,HL
const int EX_DE_HL_BASE_OPP = 0xEB;

//EX AF,AF'
const int EX_AF_AFP_BASE_OPP = 0x08;

//EXX
const int EXX_BASE_OPP = 0xD9;

//EX (SP),HL
const int EX_ISPI_HL_BASE_OPP = 0xE3;

//EX (SP),IX
//EX (SP),IY
const int EX_ISPI_IXY_BASE_OPP = 0xDDE3;

//LDI
const unsigned int LDI_BASE_OPP = 0xEDA0;

//LDIR
const unsigned int LDIR_BASE_OPP = 0xEDB0;

//LDD
const unsigned int LDD_BASE_OPP = 0xEDA8;

//LDDR
const unsigned int LDDR_BASE_OPP = 0xEDB8;

//CPI
const unsigned int CPI_BASE_OPP = 0xEDA1;

//CPIR
const unsigned int CPIR_BASE_OPP = 0xEDB1;

//CPD
const unsigned int CPD_BASE_OPP = 0xEDA9;

//CPDR
const unsigned int CPDR_BASE_OPP = 0xEDB9;


//8-Bit Arithmetic and Logical Group******************************
const int NUMBER_OF_ROPPS = 8;
const int ADD_ROPP = 0;
const int ADC_ROPP = 1;
const int SUB_ROPP = 2;
const int SBC_ROPP = 3;
const int AND_ROPP = 4;
const int OR_ROPP = 6; //early 6 is intentional
const int XOR_ROPP = 5; //5 intentional here too
const int CP_ROPP = 7;
const int ROPP_ARRAY[NUMBER_OF_ROPPS] = { ADD_ROPP, ADC_ROPP, SUB_ROPP, SBC_ROPP, AND_ROPP, OR_ROPP, XOR_ROPP, CP_ROPP };
const int ROPP_BIT_LENGTH = 3;
const int ROPP_LOCATION = 3;

const unsigned int DEC_MASK = 0x01;


//ADD A,r
//ADC A,r
//SUB A,r
//...
const int XXX_A_R_BASE_OPP = 0x80; 
const int XXX_A_R_R_POS = 0x00;

//XXX A,n
const int XXX_A_N_BASE_OPP = 0xC6; 

//XXX A,(HL)
const int XXX_A_IHLI_BASE_OPP = 0x86; 

//XXX A,(IX+d)
//XXX A,(IY+d)
const int XXX_A_IIXYPDI_BASE_OPP = 0xDD86;

//INC r
//DEC r
const int INCDEC_R_BASE_OPP = 0x04;
const int INCDEC_R_R_POS = 0x03;

//INC (HL)
//DEC (HL)
const int INCDEC_IHLI_BASE_OPP = 0x34;

//INC (IX+d)
//DEC (IX+d)
//INC (IY+d)
//DEC (IX+d)
const int INCDEC_IIXYPDI_BASE_OPP = 0xDD34;

//General Purpose Arithmetic and CPU Control Groups***************
//DDA
const int DDA_BASE_OPP = 0x27;

//CPL
const int CPL_BASE_OPP = 0x2F;

//NEG
const int NEG_BASE_OPP = 0xED44;

//CCF
const int CFF_BASE_OPP = 0x3F;

//SCF
const int SCF_BASE_OPP = 0x37;

//NOP
const unsigned int NOP_BASE_OPP = 0x00;

//HALT
const int HALT_BASE_OPP = 0x76;

//DI
const unsigned int DI_BASE_OPP = 0xF3;

//EI
const unsigned int EI_BASE_OPP = 0xFB;

//IM 0
//IM 1
//IM 2
const int IM_0_1_2_BASE_OPP = 0xED46;
const int IM_NUM_LOCATION = 3;
const int IM_NUM_LENGTH = 2;



//16-Bit Arithmetic Group ****************************************
const int SS_POS = 4;
const int SS_LENGTH = 2;
const int PP_POS = 4;
const int PP_LENGTH = 2;
const int RR_POS = 4;
const int RR_LENGTH = 2;
const unsigned int DEC_MASK_16b = 0x08;

//ADD HL,ss
const int ADD_HL_SS_BASE_OPP = 0x09;

//ADC HL,ss
const int ADC_HL_SS_BASE_OPP = 0xED4A;

//SBC HL,ss
const int SBC_HL_SS_BASE_OPP = 0xED42;

//ADD IX,pp
//ADD IY,rr
const int ADD_IXY_PPRR_BASE_OPP = 0xDD09;

//INC ss
//DEC ss
const int INCDEC_SS_BASE_OPP = 0x03;

// INC IX
// INC IY
// DEC IX
// DEC IY
const int INCDEC_IXY_BASE_OPP = 0xDD23;

//Rotate and Shift Group********************************
 const int ROTATER_RLC = 0;
 const int ROTATER_RL  = 2;
 const int ROTATER_RRC = 1;
 const int ROTATER_RR  = 3;
 const int ROTATER_SLA = 4;
 const int ROTATER_SRA = 5;
 const int ROTATER_SRL = 7;
 const int ROTATERS[] = { ROTATER_RLC, ROTATER_RL, ROTATER_RRC, ROTATER_RR, ROTATER_SLA, ROTATER_SRA, ROTATER_SRL };
 const int NUM_ROTATERS = 7;
 const int ROTATER_LOCATION = 3;
 const int ROTATER_LENGTH = 3;

 //RLCA
 const int RLCA_BASE_OPP = 0x07;
 
 //RLA
 const int RLA_BASE_OPP = 0x17;
 
 //RRCA
 const int RRCA_BASE_OPP = 0x0F;
 
 //RRA
 const int RRA_BASE_OPP = 0x1F;
 
 //RLC r
 //RL r
 //RRC r
 //RR r
 //SLA r
 //SRA r
 //SRL r
 const int ROTATE_R_BASE_OPP = 0xCB00;
 const int ROTATER_R_LOCATION = 0;
 
 //RLC (HL)
 //RL (HL)
 //RRC (HL)
 //RR (HL)
 //SLA (HL)
 //SRA (HL)
 //SRL (HL)
 const int ROTATE_IHLI_BASE_OPP = 0xCB06;
 
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
 const int ROTATE_IIXYpDI_PRE_OPP = 0xDDCB;
 const int ROTATE_IIXYpDI_POST_OPP = 0x06;

 //RLD
 const int RLD_BASE_OPP = 0xED6F;
 
 //RRD
 const int RRD_BASE_OPP = 0xED67;
 
 //TODO: make sure that register contents that are being
 //sent down as hex are interpreted as hex
 
 
//Bit Set, Reset and Test Group**************************
unsigned const int SET_MASK = 0x40;
unsigned const int NUM_BITS = 8;
unsigned const int BITS_LOCATION = 3;
unsigned const int BITS_LENGTH = 3;

//BIT b,r
const int BIT_B_R_BASE_OPP = 0xCB40;
const int BIT_B_R_R_LOCATION = 0;
const int BIT_B_R_B_LOCATION = 3;

//BIT b,(HL)
const int BIT_B_IHLI_BASE_OPP = 0xCB46;

//BIT b,(IX+d)
//BIT b,(IY+d)
const int BIT_B_IIXYpDI_PREFIX = 0xDDCB;
const int BIT_B_IIXYpDI_POSTFIX = 0x46;

//SET b,r
//RES b,r
const int SETRES_B_R_BASE_OPP = 0xCB80;
const int SETRES_B_R_R_LOCATION = 0;

//SET b,(HL)  
//RES b,(HL)
const int SETRES_B_IHLI_BASE_OPP = 0xCB86;

//SET b,(IX+d)
//SET b,(IY+d)
//RES b,(IX+d)
//RES b,(IY+d)
const int SETRES_B_IIXYpDI_PREFIX = 0xDDCB;
const int SETRES_B_IIXYpDI_POSTFIX = 0x86;


//Jump Group*********************************************

//JP nn
const int JP_NN_BASE_OPP = 0xC3;

//JP cc,nn
const int JP_CC_NN_BASE_OPP = 0xC2;
//const int CC_LOCATION = 3;
//const int CC_LENGTH = 3;

//JR e
const int JR_E_BASE_OPP = 0x18;

//JR C,e
const int JR_C_E_BASE_OPP = 0x38;

//JR NC,e
const int JR_NC_E_BASE_OPP = 0x30;

//JR Z,e
const int JR_Z_E_BASE_OPP = 0x28;

//JR NZ, e
const int JR_NZ_E_BASE_OPP = 0x20;

//JP (HL)
const int JP_IHLI_BASE_OPP = 0xE9;

//JP (IX)
//JP (IY)
const int JP_IIXYI_BASE_OPP = 0xDDE9;

//DJNZ, e
const int DJNZ_E_BASE_OPP = 0x10;




//Call and Return Group**********************************
//CALL nn
const int CALL_NN_BASE_OPP = 0xCD;

//CALL cc,nn
const int CALL_CC_NN_BASE_OPP = 0xC4;
const int CC_LOCATION = 3;
const int CC_LENGTH = 3;

//RET
const unsigned int RET_BASE_OPP = 0xC9;

//RET cc
const int RET_CC_BASE_OPP = 0xC0;

//RETI
//RETN
const int RETIN_BASE_OPP = 0xED45;
unsigned const int I_MASK = 0x08;

//RST p
const int RST_P_BASE_OPP = 0xC7;
const int P_LOCATION = 3;
const int P_LENGTH = 3;


//Input and Output Group*********************************
//IN A,(n)
const int IN_A_INI_BASE_OPP = 0xDB;

//IN F,(C)
const int IN_F_ICI_BASE_OPP = 0xED70;

//IN r,(C)
const int IN_R_ICI_BASE_OPP = 0xED40;
const int IN_R_ICI_R_LOCATION = 3;

//INI
const int INI_BASE_OPP = 0xEDA2;

//INIR
const int INIR_BASE_OPP = 0xEDB2;

//IND
const int IND_BASE_OPP = 0xEDAA;

//INDR
const int INDR_BASE_OPP = 0xEDBA;

//OUT (n),A
const int OUT_INI_A_BASE_OPP = 0xD3;

//OUT (C),r
const int OUT_ICI_R_BASE_OPP = 0xED41;
const int OUT_ICI_R_R_LOCATION = 3;

//OUTI
const int OUTI_BASE_OPP = 0xEDA3;

//OTIR
const int OTIR_BASE_OPP = 0xEDB3;

//OUTD
const int OUTD_BASE_OPP = 0xEDAB;

//OTDR
const int OTDR_BASE_OPP = 0xEDBB;

//constructor sets the pointer to the correct function
InstructionManager::InstructionManager( Z_80LogicGate* z_80LogicGate ){
	for( int i = 0; i < NUM_DISP_DIGITS; ++i ){
		segmentVectorCArray[ i ] = ' ';
	}
	//char strings must be null terminated
	segmentVectorCArray[ NUM_DISP_DIGITS ] = (char)NULL;
	
	
	z_80Registers.initRegisters( z_80LogicGate );
	setZADMode( ZAD_MODE_SMART, z_80LogicGate );
	nextPart = &InstructionManager::InstructionFetch;
	z_80LogicGate->breakSimulation();
	
	iStepModeEnabled = false;
	systemJustRestarted = true;
	interruptCase = DIDNT_interrupt;
	initOpcodeTable();
}

//This is called by the constructor to populate the opcodeTable
void InstructionManager::initOpcodeTable(){
	unsigned long opcode = 0;
	

	//8-Bit Load Group *****************************************
	//LD r, r'
	for( int r = 0; r < NUMBER_OF_R_REGISTERS; ++r ){
		for( int rp = 0; rp < NUMBER_OF_R_REGISTERS; ++rp ){
			//setting bit 6
			opcode = LD_R_RP_BASE_OPP;
			opcode = putPartIn( opcode, SINGLE_RS[r], LD_R_RP_R_POS );
			opcode = putPartIn( opcode, SINGLE_RS[rp], LD_R_RP_RP_POS );
			opcodeHash[ opcode ] = &InstructionManager::LD_r_rp;		
		}
	}
	
	//LD r, n
	for( int r = 0; r < NUMBER_OF_R_REGISTERS; ++r ){
		opcode = LD_R_N_BASE_OPP;
		opcode = putPartIn( opcode, SINGLE_RS[r], LD_R_N_R_POS );
		opcodeHash[ opcode ] = &InstructionManager::LD_r_n;
	}
	
	//LD r, (HL)
	for( int r  = 0; r < NUMBER_OF_R_REGISTERS; ++r ){
		opcode = LD_R_IHLI_BASE_OPP;
		opcode = putPartIn( opcode, SINGLE_RS[r], LD_R_IHLI_R_POS );
		opcodeHash[ opcode ] = &InstructionManager::LD_r_IHLI;
	}
	
	//LD r, (IX + d)
	for( int r  = 0; r < NUMBER_OF_R_REGISTERS; ++r ){
		opcode = LD_R_IIXYpDI_BASE_OPP;
		opcode = putPartIn( opcode, SINGLE_RS[r], LD_R_IIXYpDI_R_POS );
		hashTwoByteOpcode( opcode, &InstructionManager::LD_r_IIXYpdI );
		hashTwoByteOpcode( opcode,  &InstructionManager::LD_r_IIXYpdI );
	}
	
	//LD r, (IY + d)
	for( int r  = 0; r < NUMBER_OF_R_REGISTERS; ++r ){
		opcode = LD_R_IIXYpDI_BASE_OPP | IY_MASK;
		opcode = putPartIn( opcode, SINGLE_RS[r], LD_R_IIXYpDI_R_POS );
		hashTwoByteOpcode( opcode, &InstructionManager::LD_r_IIXYpdI );
	}
	
	//LD (HL), r
	for( int r  = 0; r < NUMBER_OF_R_REGISTERS; ++r ){
		opcode = LD_IHLI_R_BASE_OPP;
		opcode = putPartIn( opcode, SINGLE_RS[r], LD_IHLI_R_R_POS );
		opcodeHash[ opcode ] = &InstructionManager::LD_IHLI_r;
	}
	
	//LD(IX + d), r
	for( int r  = 0; r < NUMBER_OF_R_REGISTERS; ++r ){
		opcode = LD_IIXYpDI_R_BASE_OPP;
		opcode = putPartIn( opcode, SINGLE_RS[r], LD_IIXYpDI_R_R_POS );
		hashTwoByteOpcode( opcode, &InstructionManager::LD_IIXYpdI_r );
	}
	
	//LD(IY + d),r
	for( int r  = 0; r < NUMBER_OF_R_REGISTERS; ++r ){
		opcode = LD_IIXYpDI_R_BASE_OPP | IY_MASK;
		opcode = putPartIn( opcode, SINGLE_RS[r], LD_IIXYpDI_R_R_POS );
		hashTwoByteOpcode( opcode, &InstructionManager::LD_IIXYpdI_r );
	}
	
	//LD (HL), n
	opcodeHash[ LD_IHLI_N_BASE_OPP ] = &InstructionManager::LD_IHLI_n;
	
	//LD(IX + d), n
	hashTwoByteOpcode( LD_IIXYpDI_N_BASE_OPP, &InstructionManager::LD_IIXYpDI_n );
	
	//LD(IY + d),n
	hashTwoByteOpcode( LD_IIXYpDI_N_BASE_OPP | IY_MASK, &InstructionManager::LD_IIXYpDI_n );
	
	//LD A, (BC)
	opcodeHash[ LD_A_IBCI_BASE_OPP ] = &InstructionManager::LD_A_IBCDEI;
	
	//LD A, (DE)
	opcodeHash[ LD_A_IDEI_BASE_OPP ] = &InstructionManager::LD_A_IBCDEI;
	
	//LD A, (nn)
	opcodeHash[ LD_A_INNI_BASE_OPP ] = &InstructionManager::LD_A_InnI;
	
	//LD (BC),A
	opcodeHash[ LD_IBCDEI_A_BASE_OPP ] = &InstructionManager::LD_IBCDEI_A;

	//LD (DE),A
	opcodeHash[ LD_IBCDEI_A_BASE_OPP | DE_MASK ] = &InstructionManager::LD_IBCDEI_A;

	//LD (nn),A
	opcodeHash[ LD_INNI_A_BASE_OPP ] = &InstructionManager::LD_InnI_A;
	
	//LD A, I
	hashTwoByteOpcode( LD_A_I_BASE_OPP, &InstructionManager::LD_A_I );
	
	//LD A, R
	hashTwoByteOpcode( LD_A_R_BASE_OPP, &InstructionManager::LD_A_R );
	
	//LD I, A
	hashTwoByteOpcode( LD_I_A_BASE_OPP, &InstructionManager::LD_I_A );
	
	//LD R, A
	hashTwoByteOpcode( LD_R_A_BASE_OPP, &InstructionManager::LD_R_A );
	
	
	//16-Bit Load Group *****************************************
	const int NUM_DDs = 4;
	const int NUM_QQs = 4;
	
	//LD dd,nn
	for( int dd = 0; dd < NUM_DDs; ++dd ){
		opcode = LD_DD_NN_BASE_OPP;
		opcode = putPartIn( opcode, dd, DD_POS );
		opcodeHash[ opcode ] = &InstructionManager::LD_dd_nn;
	}
	
	//LD IX,nn
	hashTwoByteOpcode( LD_IXY_NN_BASE_OPP, &InstructionManager::LD_IXY_nn );
	//LD IY,nn
	hashTwoByteOpcode( (LD_IXY_NN_BASE_OPP | IY_MASK), &InstructionManager::LD_IXY_nn );

	//LD HL,(nn)
	opcodeHash[ LD_HL_INNI_BASE_OPP ] = &InstructionManager::LD_HL_InnI;
	
	//LD dd,(nn)
	for( int dd = 0; dd < NUM_DDs; ++dd ){
		opcode = LD_DD_INNI_BASE_OPP;
		opcode = putPartIn( opcode, dd, DD_POS );
		hashTwoByteOpcode( opcode, &InstructionManager::LD_dd_InnI );
	}

	//LD IX,(nn)
	hashTwoByteOpcode( LD_IXY_INNI_BASE_OPP, &InstructionManager::LD_IXY_InnI );
	//LD IY,(nn)
	hashTwoByteOpcode( (LD_IXY_INNI_BASE_OPP | IY_MASK), &InstructionManager::LD_IXY_InnI );

    //LD (nn),HL
	opcodeHash[ LD_INNI_HL_BASE_OPP ] = &InstructionManager::LD_InnI_HL;

	//LD (nn),dd
	for( int dd = 0; dd < NUM_DDs; ++dd ){
		opcode = LD_INNI_DD_BASE_OPP;
		opcode = putPartIn( opcode, dd, DD_POS );
		hashTwoByteOpcode( opcode, &InstructionManager::LD_InnI_dd );
	}

    //LD (nn),IX
	hashTwoByteOpcode( LD_INNI_IXY_BASE_OPP, &InstructionManager::LD_InnI_IXY );
    //LD (nn),IY
	hashTwoByteOpcode( (LD_INNI_IXY_BASE_OPP | IY_MASK), &InstructionManager::LD_InnI_IXY );

	//LD SP,HL
	opcodeHash[ LD_SP_HL_BASE_OPP ] = &InstructionManager::LD_SP_HL;

	//LD SP,IX
	hashTwoByteOpcode( LD_SP_IXY_BASE_OPP, &InstructionManager::LD_SP_IXY );
	//LD SP,IY
	hashTwoByteOpcode( (LD_SP_IXY_BASE_OPP | IY_MASK), &InstructionManager::LD_SP_IXY );

	//PUSH qq
	for( int qq = 0; qq < NUM_QQs; ++qq ){
		opcode = PUSH_QQ_BASE_OPP;
		opcode = putPartIn( opcode, qq, QQ_POS );
		opcodeHash[ opcode ] = &InstructionManager::PUSH_qq;
	}

	//PUSH IX
	hashTwoByteOpcode( PUSH_IXY_BASE_OPP, &InstructionManager::PUSH_IXY );
	//PUSH IY
	hashTwoByteOpcode( (PUSH_IXY_BASE_OPP | IY_MASK), &InstructionManager::PUSH_IXY );

	//POP qq
	for( int qq = 0; qq < NUM_QQs; ++qq ){
		opcode = POP_QQ_BASE_OPP;
		opcode = putPartIn( opcode, qq, QQ_POS );
		opcodeHash[ opcode ] = &InstructionManager::POP_qq;
	}

	//POP IX
	hashTwoByteOpcode( POP_IXY_BASE_OPP, &InstructionManager::POP_IXY );
	//POP IY
	hashTwoByteOpcode( (POP_IXY_BASE_OPP | IY_MASK), &InstructionManager::POP_IXY );
	
	//Exchange, Block Transfer, Block Search Groups ******************
	
	

	//EX DE,HL
	opcodeHash[ EX_DE_HL_BASE_OPP ] = &InstructionManager::EX_DE_HL;
	
	//EX AF,AF'
	opcodeHash[ EX_AF_AFP_BASE_OPP ] = &InstructionManager::EX_AF_AFP;
	
	//EXX
	opcodeHash[ EXX_BASE_OPP ] = &InstructionManager::EXX;
	
	//EX (SP),HL
	opcodeHash[ EX_ISPI_HL_BASE_OPP ] = &InstructionManager::EX_ISPI_HL;
	
	//EX (SP),IX
	hashTwoByteOpcode( EX_ISPI_IXY_BASE_OPP, &InstructionManager::EX_ISPI_IXY );
	
	//EX (SP),IY
	hashTwoByteOpcode( EX_ISPI_IXY_BASE_OPP | IY_MASK, &InstructionManager::EX_ISPI_IXY );
	
	//LDI
	hashTwoByteOpcode( LDI_BASE_OPP, &InstructionManager::LDIIRDDR );
	
	//LDIR
	hashTwoByteOpcode( LDIR_BASE_OPP, &InstructionManager::LDIIRDDR );
	
	//LDD
	hashTwoByteOpcode( LDD_BASE_OPP, &InstructionManager::LDIIRDDR );
	
	//LDDR
	hashTwoByteOpcode( LDDR_BASE_OPP, &InstructionManager::LDIIRDDR );
	
	//CPI
	hashTwoByteOpcode( CPI_BASE_OPP, &InstructionManager::CPIIRDDR );
	
	//CPIR
	hashTwoByteOpcode( CPIR_BASE_OPP, &InstructionManager::CPIIRDDR );
	
	//CPD
	hashTwoByteOpcode( CPD_BASE_OPP, &InstructionManager::CPIIRDDR );
	
	//CPDR
	hashTwoByteOpcode( CPDR_BASE_OPP, &InstructionManager::CPIIRDDR );
	
    //8-Bit Arithmetic and Logical Group******************************
	
	//ADD A,r
	//ADC A,r
	//SUB A,r
	//...
	for( int r  = 0; r < NUMBER_OF_R_REGISTERS; ++r ){
		for( int opp = 0; opp < NUMBER_OF_ROPPS; ++opp ){
			opcode = XXX_A_R_BASE_OPP;
			opcode = putPartIn( opcode, SINGLE_RS[r], XXX_A_R_R_POS );
			opcode = putPartIn( opcode, ROPP_ARRAY[opp], ROPP_LOCATION );
			opcodeHash[ opcode ] = &InstructionManager::XXX_A_r;
		}
	}
	
	//XXX A,n
	for( int opp = 0; opp < NUMBER_OF_ROPPS; ++opp ){
		opcode = XXX_A_N_BASE_OPP;
		opcode = putPartIn( opcode, ROPP_ARRAY[opp], ROPP_LOCATION );
		opcodeHash[ opcode ] = &InstructionManager::XXX_A_n;
	}
	
	//XXX A,(HL)
	for( int opp = 0; opp < NUMBER_OF_ROPPS; ++opp ){
		opcode = XXX_A_IHLI_BASE_OPP;
		opcode = putPartIn( opcode, ROPP_ARRAY[opp], ROPP_LOCATION );
		opcodeHash[ opcode ] = &InstructionManager::XXX_A_IHLI;
	}
	
	//XXX A,(IX+d)
	//XXX A,(IY+d)
	for( int opp = 0; opp < NUMBER_OF_ROPPS; ++opp ){
		opcode = XXX_A_IIXYPDI_BASE_OPP;
		opcode = putPartIn( opcode, ROPP_ARRAY[opp], ROPP_LOCATION );
		opcodeHash[ opcode ] = &InstructionManager::XXX_A_IIXYPDI;
		opcodeHash[ opcode | DEC_MASK ] = &InstructionManager::XXX_A_IIXYPDI;
	}
	
	//INC r
	//DEC r
	for( int r  = 0; r < NUMBER_OF_R_REGISTERS; ++r ){
		opcode = INCDEC_R_BASE_OPP;
		opcode = putPartIn( opcode, SINGLE_RS[r], INCDEC_R_R_POS );
		opcodeHash[ opcode ] = &InstructionManager::INCDEC_R;
		opcodeHash[ opcode | DEC_MASK ] = &InstructionManager::INCDEC_R;
	}
	
	//INC (HL)
	//DEC (HL)	
	for( int r  = 0; r < NUMBER_OF_R_REGISTERS; ++r ){
		opcode = INCDEC_IHLI_BASE_OPP;
		opcode = putPartIn( opcode, SINGLE_RS[r], INCDEC_R_R_POS );
		opcodeHash[ opcode ] = &InstructionManager::INCDEC_R;
		opcodeHash[ opcode | DEC_MASK ] = &InstructionManager::INCDEC_R;
	}
	opcodeHash[ INCDEC_IHLI_BASE_OPP ] = &InstructionManager::INCDEC_IHLI;
	opcodeHash[ INCDEC_IHLI_BASE_OPP | DEC_MASK ] = &InstructionManager::INCDEC_IHLI;
	
	//INC (IX+d)
	//DEC (IX+d)
	//INC (IY+d)
	//DEC (IX+d)
	hashTwoByteOpcode( INCDEC_IIXYPDI_BASE_OPP, &InstructionManager::INCDEC_IIXYPDI );
	hashTwoByteOpcode( INCDEC_IIXYPDI_BASE_OPP | DEC_MASK, &InstructionManager::INCDEC_IIXYPDI );
	hashTwoByteOpcode( INCDEC_IIXYPDI_BASE_OPP | IY_MASK, &InstructionManager::INCDEC_IIXYPDI );
	hashTwoByteOpcode( INCDEC_IIXYPDI_BASE_OPP | DEC_MASK | IY_MASK, &InstructionManager::INCDEC_IIXYPDI );

	//General Purpose Arithmetic and CPU Control Groups***************
	
	//DDA
	opcodeHash[ DDA_BASE_OPP ] = &InstructionManager::DDA;

	//CPL
	opcodeHash[ CPL_BASE_OPP ] = &InstructionManager::CPL;

	//NEG
	hashTwoByteOpcode( NEG_BASE_OPP, &InstructionManager::NEG );
	
	//CCF
	opcodeHash[ CFF_BASE_OPP ] = &InstructionManager::CFF;
	
	//SCF
	opcodeHash[ SCF_BASE_OPP ] = &InstructionManager::SCF;
	
	//NOP
	opcodeHash[ NOP_BASE_OPP ] = &InstructionManager::NOP;
	
	//HALT
	opcodeHash[ HALT_BASE_OPP ] = &InstructionManager::HALT;
	
	//DI
	opcodeHash[ DI_BASE_OPP ] = &InstructionManager::DI;
	
	//EI
	opcodeHash[ EI_BASE_OPP ] = &InstructionManager::EI;
	
	//IM 0
	//IM 1
	//IM 2
	opcode = putPartIn( IM_0_1_2_BASE_OPP, IM_MODE_0_CODE, IM_NUM_LOCATION );
	hashTwoByteOpcode( opcode, &InstructionManager::IM_0_1_2 );
	opcode = putPartIn( IM_0_1_2_BASE_OPP, IM_MODE_1_CODE, IM_NUM_LOCATION );
	hashTwoByteOpcode( opcode, &InstructionManager::IM_0_1_2 );
	opcode = putPartIn( IM_0_1_2_BASE_OPP, IM_MODE_2_CODE, IM_NUM_LOCATION );
	hashTwoByteOpcode( opcode, &InstructionManager::IM_0_1_2 );
	
	//16-Bit Arithmetic Group ****************************************
	const int NUM_SSs = 4;
	const int NUM_PPs = 4;
	//const int NUM_RRs = 4;
	
	//ADD HL,ss
	for( int i = 0; i < NUM_SSs; ++i ){
		opcode = ADD_HL_SS_BASE_OPP;
		opcode = putPartIn( opcode, i, SS_POS );
		opcodeHash[ opcode ] = &InstructionManager::ADD_HL_ss;
	}

	//ADC HL,ss	
	for( int i = 0; i < NUM_SSs; ++i ){
		opcode = ADC_HL_SS_BASE_OPP;
		opcode = putPartIn( opcode, i, SS_POS );
		hashTwoByteOpcode( opcode, &InstructionManager::ADC_HL_ss );
	}

	//SBC HL,ss
	for( int i = 0; i < NUM_SSs; ++i ){
		opcode = SBC_HL_SS_BASE_OPP;
		opcode = putPartIn( opcode, i, SS_POS );
		hashTwoByteOpcode( opcode, &InstructionManager::SBC_HL_ss );
	}

	//ADD IX,pp
	//ADD IY,rr
	for( int i = 0; i < NUM_PPs; ++i ){
		opcode = ADD_IXY_PPRR_BASE_OPP;
		opcode = putPartIn( opcode, i, PP_POS );
		hashTwoByteOpcode( opcode, &InstructionManager::ADD_IXY_pprr );
		hashTwoByteOpcode( (opcode|IY_MASK), &InstructionManager::ADD_IXY_pprr );
	}

	//INC ss
	//DEC ss
	for( int i = 0; i < NUM_SSs; ++i ){
		opcode = INCDEC_SS_BASE_OPP;
		opcode = putPartIn( opcode, i, SS_POS );
		opcodeHash[ opcode ] = &InstructionManager::INCDEC_ss;
		opcodeHash[ opcode | DEC_MASK_16b ] = &InstructionManager::INCDEC_ss;
	}

	// INC IX
	hashTwoByteOpcode( INCDEC_IXY_BASE_OPP, &InstructionManager::INCDEC_IXY );
	
	// DEC IX
	hashTwoByteOpcode( (INCDEC_IXY_BASE_OPP | DEC_MASK_16b), &InstructionManager::INCDEC_IXY );
	
	// INC IY
	hashTwoByteOpcode( (INCDEC_IXY_BASE_OPP | IY_MASK), &InstructionManager::INCDEC_IXY );
	
	// DEC IY
	hashTwoByteOpcode( (INCDEC_IXY_BASE_OPP | IY_MASK | DEC_MASK_16b), &InstructionManager::INCDEC_IXY );
	
	//Rotate and Shift Group********************************
 	//RLCA
 	opcodeHash[ RLCA_BASE_OPP ] = &InstructionManager::RLCA;
 
 	//RLA
 	opcodeHash[ RLA_BASE_OPP ] = &InstructionManager::RLA;
 	
	//RRCA
	opcodeHash[ RRCA_BASE_OPP ] = &InstructionManager::RRCA;
 
 	//RRA
 	opcodeHash[ RRA_BASE_OPP ] = &InstructionManager::RRA;
 
	 //RLC r
	 //RL r
	 //RRC r
	 //RR r
	 //SLA r
	 //SRA r
	 //SRL r
	 for( int rotaterNum = 0; rotaterNum < NUM_ROTATERS; ++rotaterNum ){
	 	for( int r = 0; r < NUMBER_OF_R_REGISTERS; ++r ){
		 	opcode = ROTATE_R_BASE_OPP;
		 	opcode = putPartIn( opcode, ROTATERS[rotaterNum], ROTATER_LOCATION );
		 	opcode = putPartIn( opcode, SINGLE_RS[r], ROTATER_R_LOCATION );
		 	hashTwoByteOpcode( opcode, &InstructionManager::ROTATE_R );
	 	}
	 }
 
	 //RLC (HL)
	 //RL (HL)
	 //RRC (HL)
	 //RR (HL)
	 //SLA (HL)
	 //SRA (HL)
	 //SRL (HL)
	 for( int rotaterNum = 0; rotaterNum < NUM_ROTATERS; ++rotaterNum ){
		 opcode = putPartIn( ROTATE_IHLI_BASE_OPP, ROTATERS[rotaterNum], ROTATER_LOCATION );
		 hashTwoByteOpcode( opcode, &InstructionManager::ROTATE_IHLI );
	 }
 
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
	 for( int rotaterNum = 0; rotaterNum < NUM_ROTATERS; ++rotaterNum ){
	 	int postFix = putPartIn( ROTATE_IIXYpDI_POST_OPP, ROTATERS[rotaterNum], ROTATER_LOCATION);
	 	hashOpcodeWithPostFix( ROTATE_IIXYpDI_PRE_OPP, postFix, &InstructionManager::ROTATE_IIXYpDI );
	 	hashOpcodeWithPostFix( ROTATE_IIXYpDI_PRE_OPP | IY_MASK, postFix, &InstructionManager::ROTATE_IIXYpDI );
	 }

	 //RLD
	 hashTwoByteOpcode( RLD_BASE_OPP, &InstructionManager::RLD );
 
 	//RRD
 	hashTwoByteOpcode( RRD_BASE_OPP, &InstructionManager::RRD );
	
	//Bit Set, Reset and Test Group**************************
	//BIT b,r
	for( unsigned int b = 0; b < NUM_BITS; ++b ){	 	
		for( int r = 0; r < NUMBER_OF_R_REGISTERS; ++r ){
		 	opcode = BIT_B_R_BASE_OPP;
		 	opcode = putPartIn( opcode, b, BITS_LOCATION );
		 	opcode = putPartIn( opcode, SINGLE_RS[r], BIT_B_R_R_LOCATION );
		 	hashTwoByteOpcode( opcode, &InstructionManager::BIT_b_r );
	 	}
	}
	
	//BIT b,(HL)
	for( unsigned int b = 0; b < NUM_BITS; ++b ){
		hashTwoByteOpcode( putPartIn( BIT_B_IHLI_BASE_OPP, b, BITS_LOCATION ), &InstructionManager::BIT_b_IHLI );
	}
	
	//BIT b,(IX+d)
	//BIT b,(IY+d)
	for( unsigned int b = 0; b < NUM_BITS; ++b ){
		int postFix = putPartIn( BIT_B_IIXYpDI_POSTFIX, b, BITS_LOCATION );
		hashOpcodeWithPostFix( BIT_B_IIXYpDI_PREFIX, postFix, &InstructionManager::BIT_B_IIXYpDI );
		hashOpcodeWithPostFix( (BIT_B_IIXYpDI_PREFIX | IY_MASK), postFix, &InstructionManager::BIT_B_IIXYpDI );
	}
	
	//SET b,r
	//RES b,r
	for( unsigned int b = 0; b < NUM_BITS; ++b ){	 	
		for( int r = 0; r < NUMBER_OF_R_REGISTERS; ++r ){
			opcode = SETRES_B_R_BASE_OPP;
			opcode = putPartIn( opcode, b, BITS_LOCATION );
			opcode = putPartIn( opcode, SINGLE_RS[r], SETRES_B_R_R_LOCATION );
			hashTwoByteOpcode( opcode, &InstructionManager::SETRES_B_R );
			hashTwoByteOpcode( (opcode | SET_MASK), &InstructionManager::SETRES_B_R );
	 	}
	}
	
	//SET b,(HL)  
	//RES b,(HL)
	for( unsigned int b = 0; b < NUM_BITS; ++b ){
		opcode = putPartIn( SETRES_B_IHLI_BASE_OPP, b, BITS_LOCATION );
		hashTwoByteOpcode( opcode, &InstructionManager::SETRES_B_IHLI );
		hashTwoByteOpcode( (opcode | SET_MASK), &InstructionManager::SETRES_B_IHLI );
	}
	
	//SET b,(IX+d)
	//SET b,(IY+d)
	//RES b,(IX+d)
	//RES b,(IY+d)
	for( unsigned int b = 0; b < NUM_BITS; ++b ){
		int postFix = putPartIn( SETRES_B_IIXYpDI_POSTFIX, b, BITS_LOCATION );
		hashOpcodeWithPostFix( SETRES_B_IIXYpDI_PREFIX, postFix, &InstructionManager::SETRES_B_IIXYpDI );
		hashOpcodeWithPostFix( (SETRES_B_IIXYpDI_PREFIX | IY_MASK), postFix, &InstructionManager::SETRES_B_IIXYpDI );
		hashOpcodeWithPostFix( SETRES_B_IIXYpDI_PREFIX, (postFix | SET_MASK), &InstructionManager::SETRES_B_IIXYpDI );
		hashOpcodeWithPostFix( (SETRES_B_IIXYpDI_PREFIX | IY_MASK), (postFix | SET_MASK), &InstructionManager::SETRES_B_IIXYpDI );
	}
		
	
	//Jump Group*********************************************

	//JP nn
	opcodeHash[ JP_NN_BASE_OPP ] = &InstructionManager::JP_nn;
	
	//JP cc,nn
	for( int cc = 0; cc < NUM_CCs; ++cc ){
		opcodeHash[ putPartIn( JP_CC_NN_BASE_OPP, cc, CC_LOCATION ) ] = &InstructionManager::JP_cc_nn;
	}
	
	//JR e
	opcodeHash[ JR_E_BASE_OPP ] = &InstructionManager::JR_e;
	
	//JR C,e
	opcodeHash[ JR_C_E_BASE_OPP ] = &InstructionManager::JR_C_e;
	
	//JR NC,e
	opcodeHash[ JR_NC_E_BASE_OPP ] = &InstructionManager::JR_NC_e;
	
	//JR Z,e
	opcodeHash[ JR_Z_E_BASE_OPP ] = &InstructionManager::JR_Z_e;
	
	//JR NZ, e
	opcodeHash[ JR_NZ_E_BASE_OPP ] = &InstructionManager::JR_NZ_e;
	
	//JP (HL)
	opcodeHash[ JP_IHLI_BASE_OPP ] = &InstructionManager::JP_IHLI;
	
	//JP (IX)
	hashTwoByteOpcode( JP_IIXYI_BASE_OPP, &InstructionManager::JP_IIXYI );
	
	//JP (IY)
	hashTwoByteOpcode( (JP_IIXYI_BASE_OPP | IY_MASK), &InstructionManager::JP_IIXYI );
	
	//DJNZ, e
	opcodeHash[ DJNZ_E_BASE_OPP ] = &InstructionManager::DJNZ_e;

	
	//Call and Return Group**********************************
	//CALL nn
	opcodeHash[ CALL_NN_BASE_OPP ] = &InstructionManager::CALL_nn;
	
	//CALL cc,nn
	for( int cc = 0; cc <= 7; ++cc ){
		opcodeHash[ putPartIn( CALL_CC_NN_BASE_OPP, cc, CC_LOCATION ) ] 
		    = &InstructionManager::CALL_cc_nn;
	}
	
	//RET
	opcodeHash[ RET_BASE_OPP ] = &InstructionManager::RET;
	
	//RET cc
	for( int cc = 0; cc <= 7; ++cc ){
		opcodeHash[ putPartIn( RET_CC_BASE_OPP, cc, CC_LOCATION ) ]
		    = &InstructionManager::RET_cc;
	}
	
	//RETI
	hashTwoByteOpcode( RETIN_BASE_OPP | I_MASK, &InstructionManager::RETIN );
	//RETN
	hashTwoByteOpcode( RETIN_BASE_OPP, &InstructionManager::RETIN );
	
	//RST p
	for( int p = 0; p <= 7; ++p ){
		opcodeHash[ putPartIn( RST_P_BASE_OPP, p, P_LOCATION ) ]
		    = &InstructionManager::RST_p;
	}
	
	
	//Input and Output Group*********************************
	//IN A,(n)
	opcodeHash[ IN_A_INI_BASE_OPP ] = &InstructionManager::IN_A_INI;
	
	//IN F,(C)
	hashTwoByteOpcode( IN_F_ICI_BASE_OPP, &InstructionManager::IN_F_ICI );
	
	//IN r,(C)
	for( int r = 0; r < NUMBER_OF_R_REGISTERS; ++r ){
	 	opcode = putPartIn( IN_R_ICI_BASE_OPP, SINGLE_RS[r], IN_R_ICI_R_LOCATION );
	 	hashTwoByteOpcode( opcode, &InstructionManager::IN_R_ICI );
	}
	
	//INI
	hashTwoByteOpcode( INI_BASE_OPP, &InstructionManager::INI );
	
	//INIR
	hashTwoByteOpcode( INIR_BASE_OPP, &InstructionManager::INIR );
	
	//IND
	hashTwoByteOpcode( IND_BASE_OPP, &InstructionManager::IND );

	//INDR
	hashTwoByteOpcode( INDR_BASE_OPP, &InstructionManager::INDR );
	
	//OUT (n),A
	opcodeHash[ OUT_INI_A_BASE_OPP ] = &InstructionManager::OUT_INI_A;

	//OUT (C),r
	for( int r = 0; r < NUMBER_OF_R_REGISTERS; ++r ){
	 	opcode = putPartIn( OUT_ICI_R_BASE_OPP, SINGLE_RS[r], OUT_ICI_R_R_LOCATION );
	 	hashTwoByteOpcode( opcode, &InstructionManager::OUT_ICI_R );
	}
	
	//OUTI
	hashTwoByteOpcode( OUTI_BASE_OPP, &InstructionManager::OUTI );
	
	//OTIR
	hashTwoByteOpcode( OTIR_BASE_OPP, &InstructionManager::OTIR );
	
	//OUTD
	hashTwoByteOpcode( OUTD_BASE_OPP, &InstructionManager::OUTD );
	
	//OTDR
	hashTwoByteOpcode( OTDR_BASE_OPP, &InstructionManager::OTDR );
}

//This is a method used by initOpcodeTable to construct the opcode from pieces
unsigned long InstructionManager::putPartIn( unsigned long rest, int part, int bytePosition ){
    part <<= bytePosition;
    rest |= part;
    return rest;
}

//This method takes a two byte opcode and puts it in the opcodeHash once
//times as a one byte opcode as a then as a two byte opcode
//so that each time that only part of the opcode is looked up, the code
//will know to fetch more opcode
void InstructionManager::hashTwoByteOpcode( unsigned long opcode, funcPointer targetFunction ){
	//first we hash the fully formed code
	//to the indicated function
	opcodeHash[ opcode ] = targetFunction;
	opcodeHash[ opcode >> BITS_IN_BYTE ] = &InstructionManager::fetchMoreOpcode;
}

//This function is stashed at opcode locations in the opcodeHash that are partial opcodes.
//It stashes the current opcode and requests another
void InstructionManager::fetchMoreOpcode( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	FetchOpcode( waveformFSM ); //get aditional opcode
	nextPart = &InstructionManager::fetchMoreOpcode_2;
}
void InstructionManager::fetchMoreOpcode_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	opcodeData <<= 8; //move the old opcode over one byte
	opcodeData |= newData; //or in new part.
	
	//now recall the hash with new new opcode
	
	if( this->opcodeHash[ opcodeData ] != NULL ){
		(this->*opcodeHash[ opcodeData ])( z_80LogicGate, waveformFSM, newData );
	}else{
		//bad opcodes will be treated as no-ops
		InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
	}		
}

//This function takes an opcode that has a postfix to it
//and hashes it correctly to the opcodeHash and the
//postfixedOpcodeHash.
void InstructionManager::hashOpcodeWithPostFix( unsigned long twoBytePrefix, unsigned long postfix, funcPointer targetFunction ){
	hashTwoByteOpcode( twoBytePrefix, &InstructionManager::fetchArgumentAndPostFix );
	postfixedOpcodeHash[ ((twoBytePrefix << BITS_IN_BYTE) | postfix ) ] = targetFunction;
}

//This function is hashed in the opcodeHash hash by hashOpcodeWithPostFix so that the postfixedOpcodeHash
//will be accessed.
void InstructionManager::fetchArgumentAndPostFix( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//we need to get a d
	FetchOpcode( waveformFSM );
	nextPart = &InstructionManager::fetchArgumentAndPostFix_2;
}
void InstructionManager::fetchArgumentAndPostFix_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData = newData;
	//and now the postfix
	FetchOpcode( waveformFSM );
	nextPart = &InstructionManager::fetchArgumentAndPostFix_3;
}
void InstructionManager::fetchArgumentAndPostFix_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int postfix = newData;
	int d = tempData;
	int prefix = opcodeData;
	
	//set the opcodeData correctly
	opcodeData = ( prefix << ( BITS_IN_BYTE * 2 ) ) | ( d << BITS_IN_BYTE ) | postfix;
	
	int postfixedOpcodeHashKey = (prefix << BITS_IN_BYTE) | postfix;
	
	//and then call the post fixedified function
	//now call it
	(this->*postfixedOpcodeHash[ postfixedOpcodeHashKey ])( z_80LogicGate, waveformFSM, newData );
}

//This function is called by each implementation
//of a function to provide information to pass up
//to the pop-up about the current executing instruction
void InstructionManager::describeInstruction( string oppName, string description, int n, Z_80LogicGate* z_80LogicGate ){
	if( iStepModeEnabled ){
		ostringstream outer;
		outer << "opp: " << oppName << " n: 0x" << hex << uppercase << n << dec << "(" << n << ")" << endl;
		outer << endl;
		outer << "description: " << description << endl;
		z_80LogicGate->setInfoString( outer.str() );
	}
}
void InstructionManager::describeInstruction( string oppName, string description, Z_80LogicGate* z_80LogicGate ){
	if( iStepModeEnabled ){
		ostringstream outer;
		outer << "opp: " << oppName << endl;
		outer << endl;
		outer << "description: " << description << endl;
		z_80LogicGate->setInfoString( outer.str() );
	}
}


 
// This will extract from data a range of bits and return it as an integer
// @param data the data containing the field
// @param bitNum the index at which the right most bit resides.  Byte zerro is
// the least significant (right most) byte of the data.
// @param numBits the number of bits that pertain to the field.
// @return the extracted field 
int InstructionManager::extractField( int data, int bitLocation, int bitLength ){
  data >>= bitLocation;
  int mask = (1<<bitLength)-1;
  return (int)(data & mask);
}

  //This function is used to translate the way registers are refered to in the opcode
//to how they are refered to in the Z_80Registers layer
RSinglet InstructionManager::translateReg8b( int regCode ){
	RSinglet answer = aReg;
	switch( regCode ){
		case B_REG:
			answer = bReg;
			break;
		case C_REG:
			answer = cReg;
			break;
		case D_REG:
			answer = dReg;
			break;
		case E_REG:
			answer = eReg;
			break;
		case H_REG:
			answer = hReg;
			break;
		case L_REG:
			answer = lReg;
			break;
		case A_REG:
			answer = aReg;
			break;
	}
	return answer;
}

//This function is used to to translate 16b values.  There are two different ways, the dd way
//and the qq way
RDoublet InstructionManager::translateReg16bDD( int regCode ){
	const int BCcode = 0;
	const int DEcode = 1;
	const int HLcode = 2;
	const int SPcode = 3;
	RDoublet answer = bcReg;
	switch( regCode ){
		case BCcode:
			answer = bcReg;
			break;
		case DEcode:
			answer = deReg;
			break;
		case HLcode:
			answer = hlReg;
			break;
		case SPcode:
			answer = spReg;
			break;
	}
	return answer;
}
RDoublet InstructionManager::translateReg16bQQ( int regCode ){
	const int BCcode = 0;
	const int DEcode = 1;
	const int HLcode = 2;
	const int AFcode = 3;
	RDoublet answer = bcReg;
	switch( regCode ){
		case BCcode:
			answer = bcReg;
			break;
		case DEcode:
			answer = deReg;
			break;
		case HLcode:
			answer = hlReg;
			break;
		case AFcode:
			answer = afReg;
			break;
	}
	return answer;
}
RDoublet InstructionManager::translateReg16bSS( int regCode ){
	return translateReg16bDD( regCode );
}
RDoublet InstructionManager::translateReg16bPP( int regCode ){
	const int BCcode = 0;
	const int DEcode = 1;
	const int IXcode = 2;
	const int SPcode = 3;
	RDoublet answer = bcReg;
	switch( regCode ){
		case BCcode:
			answer = bcReg;
			break;
		case DEcode:
			answer = deReg;
			break;
		case IXcode:
			answer = ixReg;
			break;
		case SPcode:
			answer = spReg;
			break;
	}
	return answer;
}
RDoublet InstructionManager::translateReg16bRR( int regCode ){
	const int BCcode = 0;
	const int DEcode = 1;
	const int IYcode = 2;
	const int SPcode = 3;
	RDoublet answer = bcReg;
	switch( regCode ){
		case BCcode:
			answer = bcReg;
			break;
		case DEcode:
			answer = deReg;
			break;
		case IYcode:
			answer = iyReg;
			break;
		case SPcode:
			answer = spReg;
			break;
	}
	return answer;
}

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
bool InstructionManager::checkInterupts( WaveformFSM* waveformFSM, Z_80LogicGate* z_80LogicGate ){
	interruptCase = waveformFSM->checkInteruptState();
	
	bool answer = false;
	
	//check to see if an interupt that we need to handle has happened
	//special note:  We don't interupt if the last instruction was an EI or DI
	//This is so that we can execute a RETI next before another interupt happens
	if( interruptCase == NMI_interrupt || 
	   ( interruptCase == INT_interrupt && z_80Registers.getIFF1() && 
	     opcodeData != EI_BASE_OPP && opcodeData != DI_BASE_OPP )){
	     	
		//request the apropreate responce
		tempData = 0;
		if( interruptCase == NMI_interrupt ){
			waveformFSM->preformNMIRequestCycle( z_80Registers.get16b( pcReg ) );
			z_80Registers.setIFF2( z_80Registers.getIFF1() );
		}else{
			waveformFSM->preformINTRequestCycle( z_80Registers.get16b( pcReg ) );
			z_80Registers.setIFF2( 0 );
		}
		z_80Registers.setIFF1( 0 );
		//and set up the interuptHandler function serise
		if( interruptCase != INT_interrupt || z_80Registers.getIntModeCode() != IM_MODE_0_CODE ){
			nextPart = &InstructionManager::checkInterupts_2;
		}else{
			//interupt mode 1 assumes that the byte supplied
			//by the io devices is an opcode.
			nextPart = &InstructionManager::InstructionDecode;
			
			//TODO: modify fetch opcode so that that if the
			//byte fetched by the opcode is a multibyte opcode,
			//more then one byte will be attempted to be
			//fetched from the io device and we wont try to
			//fetch the aditional opcodes from the pc
			//http://www.gaby.de/z80/zip/z80cpu_um.pdf
			//sugests that an opcode suplied by the hardware
			//may have more then one byte in it.
		}
		answer = true;
	}else{
		answer = false;
	}
	return answer;
}

//Thease serise of functions make it so that the instruction manager
//can take in interupts
void InstructionManager::checkInterupts_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//we save this data for use later
	tempData = newData;
	
	//we now need to save the pc to the stack
	int pcValue = tempData2 = z_80Registers.get16b( pcReg );
	int spValue = z_80Registers.get16b( spReg );
	//save the high order byte first (just a guess)
	waveformFSM->preformMemoryOperation( spValue - 1, true, ((pcValue & 0xFF00) >> BITS_IN_BYTE) );
	
	nextPart = &InstructionManager::checkInterupts_3;
}
void InstructionManager::checkInterupts_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int pcValue = tempData2;
	int spValue = z_80Registers.get16b( spReg );
	//and then the low order byte
	waveformFSM->preformMemoryOperation( spValue - 2, true, (pcValue & 0x00FF) );
	//move the stack pointer
	z_80Registers.set16b( spReg, spValue - 2 );
	nextPart = &InstructionManager::checkInterupts_4;
}
void InstructionManager::checkInterupts_4( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int modeCode = z_80Registers.getIntModeCode();
	//now it is time to do different things depending on what type of interupt
	//we are experianceing
	if( interruptCase == NMI_interrupt ){
		z_80Registers.set16b( pcReg, NMI_RESTART_LOCATION );
		interruptCase = DIDNT_interrupt;
		//start executing interupt function
		InstructionFetch( z_80LogicGate, waveformFSM, newData );
	
	//IM_MODE_0_CODE is handled in the checkInterupts first method
	}else if( modeCode == IM_MODE_1_CODE ){
		z_80Registers.set16b( pcReg, INT_MODE_1_RESTART_LOCATION );
		interruptCase = DIDNT_interrupt;
		//start executing interupt function
		InstructionFetch( z_80LogicGate, waveformFSM, newData );
		
	}else if( modeCode == IM_MODE_2_CODE ){
		//im mode 2 must do some research before it can jump to
		//its interupt function
		int iValue = z_80Registers.get8b( iReg );
		int valueFromIO = tempData;
		int vectorAddress = tempData2 = ( (iValue << BITS_IN_BYTE) | valueFromIO );
		
		//we now need to fetch two bytes from the vector table because
		//memory addresses are 16 bits long.
		//we will now arbitrarily fetch fetch the lower ordered byte first
		waveformFSM->preformMemoryOperation( vectorAddress, false );
		nextPart = &InstructionManager::checkInterupts_5;
	}else{
		//if we get here we have a bad error.  However we will set the nextPart pointer
		//just so the program doesn't compleately blow up
		nextPart = &InstructionManager::InstructionFetch;
	}
}
void InstructionManager::checkInterupts_5( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//only interupt mode 2 gets this far
	tempData = newData; //this is the lower side of the byte
	int vectorAddress = tempData2;
	
	//we now need to fetch the higher order of the byte to put together with the lower order so
	//that we can construct the location of the interupt service rutine from the vector
	//interupt table
	waveformFSM->preformMemoryOperation( vectorAddress + 1, false );
	nextPart = &InstructionManager::checkInterupts_6;
}
void InstructionManager::checkInterupts_6( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int serviceRutineAddress = (newData << BITS_IN_BYTE) | tempData;
	
	//reset flags
	interruptCase = DIDNT_interrupt;
	
	//now that we know the address of the serviceRutine we should jump to it
	z_80Registers.set16b( pcReg, serviceRutineAddress );
	//start executing interupt function
	InstructionFetch( z_80LogicGate, waveformFSM, newData );
}
		

//acording to Dr. Sangregory (A microprocessor instructor),
//after we resume from halts, we should execute the next instruction
//that comes after the halt.  Thus when we return from an interupt
//handler, we do not need to take care to continure executing the
//halt, because that is not the right thing to do.

	

//Nongeneric functions that get hashed in the opcodeHash

//moves a 8bit value from one register to another
void InstructionManager::LD_r_rp( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//( int data, int bitLocation, int bitLength )
	int rp = extractField( opcodeData, LD_R_RP_RP_POS, SINGLE_BIT_LENGTH );
	int r = extractField( opcodeData, LD_R_RP_R_POS, SINGLE_BIT_LENGTH );
	int temp = z_80Registers.get8b( translateReg8b( rp ) );
	z_80Registers.set8b( translateReg8b( r ), temp );
	
	ostringstream cater;
	cater << "LD " << z_80Registers.get8bName( translateReg8b( rp ) ) << ", " << z_80Registers.get8bName( translateReg8b( r ) );
	describeInstruction( cater.str(), "Copies data from one register to another", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

//loads an 8bit immediate into a register
void InstructionManager::LD_r_n( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	FetchOpcode( waveformFSM ); //need to get an n
	nextPart = &InstructionManager::LD_r_n_2;	
}
void InstructionManager::LD_r_n_2( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData ){
	int r = extractField( opcodeData, LD_R_N_R_POS, SINGLE_BIT_LENGTH ); 
	z_80Registers.set8b( translateReg8b( r ), newData );
	
	ostringstream cater;
	cater << "LD " << z_80Registers.get8bName( translateReg8b( r ) ) << ",n";
	ostringstream description;
	description << "Loads n into register " << z_80Registers.get8bName( translateReg8b( r ) );
	describeInstruction( cater.str(), description.str(), newData, z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

//load from memory referenced by HL into target r
//LD r, (HL)
void InstructionManager::LD_r_IHLI( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData ){
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( hlReg ), false );
	nextPart = &InstructionManager::LD_r_IHLI_2;
}
void InstructionManager::LD_r_IHLI_2( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData ){
	int r = extractField( opcodeData, LD_R_IHLI_R_POS, SINGLE_BIT_LENGTH ); 
	z_80Registers.set8b( translateReg8b( r ), newData );
	
	ostringstream cater;
	cater << "LD " << z_80Registers.get8bName( translateReg8b( r ) ) << ",(HL)";
	describeInstruction( cater.str(), "loads register from memory indexed by HL", newData, z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

//loads from memory dereferenced from offset from IX or IY register
void InstructionManager::LD_r_IIXYpdI( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData ){
	FetchOpcode( waveformFSM ); //need to get d
	nextPart = &InstructionManager::LD_r_IIXYpdI_2;
}
void InstructionManager::LD_r_IIXYpdI_2( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData ){
	int d = tempData = z_80Registers.getSignExtend(  newData , BYTE_SIZE );
	RDoublet selectedReg = ixReg;
	if( extractField( opcodeData, 8+5, 1 ) == 1 ){
		selectedReg = iyReg;
	}
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( selectedReg ) + d, false ); 
	nextPart = &InstructionManager::LD_r_IIXYpdI_3;
}
void InstructionManager::LD_r_IIXYpdI_3( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData ){
	//now that we fetched the data, we need to know were to put it
	int r = extractField( opcodeData, LD_R_IIXYpDI_R_POS, SINGLE_BIT_LENGTH );
	z_80Registers.set8b( translateReg8b( r ), newData );
	
	RDoublet selectedReg = ixReg;
	if( extractField( opcodeData, 8+5, 1 ) == 1 ){
		selectedReg = iyReg;
	}
	
	ostringstream cater;
	int d = tempData;
	cater << "LD " << z_80Registers.get8bName( translateReg8b( r ) ) << ",(" << z_80Registers.get16bName( selectedReg ) << "+" << d << ")";
	describeInstruction( cater.str(), "loads register from memory indexed by IX or IY index off by d", newData, z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

//save to memory index at HL from register
void InstructionManager::LD_IHLI_r( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData ){
	int r = extractField( opcodeData, LD_IHLI_R_R_POS, SINGLE_BIT_LENGTH );
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( hlReg ), true, z_80Registers.get8b( translateReg8b( r ) ) );
	
	ostringstream cater;
	cater << "LD (HL)," << z_80Registers.get8bName( translateReg8b( r ) );
	describeInstruction( cater.str(), "saves to the memory location indexed by HL the value in the specified reg", z_80LogicGate );
	
	nextPart = &InstructionManager::InstructionFetch;
}


//save register into memory indexed by IX or by IY and an offset
void InstructionManager::LD_IIXYpdI_r( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData ){
	FetchOpcode( waveformFSM ); //need to get d
	nextPart = &InstructionManager::LD_IIXYpdI_r_2;
}
void InstructionManager::LD_IIXYpdI_r_2( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData ){
	//determin what r to save
	int r = extractField( opcodeData, LD_IIXYpDI_R_R_POS, SINGLE_BIT_LENGTH );
	//determine which to index off of
	RDoublet selectedIndex = ixReg;
	if( extractField( opcodeData, 5+8, 1 ) == 1 )
		selectedIndex = iyReg;
	//identify d
	int d = z_80Registers.getSignExtend(  newData , BYTE_SIZE );
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( selectedIndex ) + d, true, z_80Registers.get8b( translateReg8b( r ) ) );
	
	ostringstream cater;
	cater << "LD (" << z_80Registers.get16bName( selectedIndex ) << "+" << d << ")," << z_80Registers.get8bName( translateReg8b( r ) );
	describeInstruction( cater.str(), "saves to the memory location indexed by IX or IY offset by 'd' the value in the specified reg", z_80LogicGate );
	
	nextPart = &InstructionManager::InstructionFetch;
}

//load constant into location index by HL
void InstructionManager::LD_IHLI_n( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	FetchOpcode( waveformFSM );//we need an n
	nextPart = &InstructionManager::LD_IHLI_n_2;
}
void InstructionManager::LD_IHLI_n_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int n = newData;
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( hlReg ), true, n );
	
	ostringstream cater;
	cater << "LD (HL),n";
	describeInstruction( cater.str(), "saves a constant to memory indexed by HL", n, z_80LogicGate );
	
	nextPart = &InstructionManager::InstructionFetch;	
}

//saves immediate into memory indexed by (IX+d) or (IY+d) 
void InstructionManager::LD_IIXYpDI_n( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//first we get the d
	FetchOpcode( waveformFSM );
	nextPart = &InstructionManager::LD_IIXYpDI_n_2;
}
void InstructionManager::LD_IIXYpDI_n_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData = newData; //stash d in tempData
	//then we get the n
	FetchOpcode( waveformFSM );
	nextPart = &InstructionManager::LD_IIXYpDI_n_3;
}
void InstructionManager::LD_IIXYpDI_n_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int d = z_80Registers.getSignExtend(  tempData , BYTE_SIZE );
	int n = newData;
	//determine if we are ixing or iying
	RDoublet selectedIndex = ixReg;
	if( (opcodeData & IY_MASK) == IY_MASK )
		selectedIndex = iyReg;
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( selectedIndex )+d, true, n );
	
	ostringstream oppText;
	oppText << "LD " << printIXYpd( d, false ) << ",n";
	describeInstruction( oppText.str(), "saves to the memory location indexed by IX or IY offset by d the supplied constant", n, z_80LogicGate );
	
	nextPart = &InstructionManager::InstructionFetch;	
}

//loads into A memory referenced by BC or DE
void InstructionManager::LD_A_IBCDEI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//first we determine if we are BCing or DEing
	RDoublet indexer = bcReg;
	if( (opcodeData & DE_MASK) == DE_MASK )
		indexer = deReg;
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( indexer ), false );
	nextPart = &InstructionManager::LD_X_IBCDEI_2;
}
void InstructionManager::LD_X_IBCDEI_2( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData ){
	//the data comes in and we stash it away
	z_80Registers.set8b( aReg, newData );
	
	ostringstream cater;
	RDoublet indexer = bcReg;
	if( (opcodeData & DE_MASK) == DE_MASK )
		indexer = deReg;
	cater << "LD A,(" << z_80Registers.get16bName( indexer ) << ")";
	describeInstruction( cater.str(), "loads A with the value referenced by BC or DE", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

//load 8 bit reg A from memory indexed location
void InstructionManager::LD_A_InnI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	FetchOpcode( waveformFSM ); //need to get the nns
	nextPart = &InstructionManager::LD_A_InnI_2;
}
void InstructionManager::LD_A_InnI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData = newData; //got the first n
	FetchOpcode( waveformFSM ); //need to get the second n
	nextPart = &InstructionManager::LD_A_InnI_3;
}
void InstructionManager::LD_A_InnI_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int saveAddress = (newData << 8 | tempData ); //the ns are loaded in little indian
	//also the data is saved in little indian
	waveformFSM->preformMemoryOperation( saveAddress, false );
	nextPart = &InstructionManager::LD_A_InnI_4;	
}
void InstructionManager::LD_A_InnI_4( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	z_80Registers.set8b( aReg, newData );
	
	ostringstream cater;
	cater << "LD A,(nn)";
	describeInstruction( cater.str(), "loads A from a memory location referenced by the supplied constant", newData, z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

//save to memory index at BC or DE from A
void InstructionManager::LD_IBCDEI_A( Z_80LogicGate* z_80LogicGate,WaveformFSM* waveformFSM ,int newData ){
	//first we determine if we are BCing or DEing
	RDoublet indexer = bcReg;
	if( extractField( opcodeData, 4, 1 ) == 1 )
		indexer = deReg;
	
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( indexer ), true, z_80Registers.get8b( aReg ) );
	
	ostringstream cater;
	cater << "LD (" << z_80Registers.get16bName( indexer ) << "),A";
	describeInstruction( cater.str(), "saved to the memory location indexed by BC or DE the value in the accumulator", z_80LogicGate );
	
	nextPart = &InstructionManager::InstructionFetch;
}

//saves 8 bit reg A  to index memory location
void InstructionManager::LD_InnI_A( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	FetchOpcode( waveformFSM ); //need to get the nns
	nextPart = &InstructionManager::LD_InnI_A_2;
}
void InstructionManager::LD_InnI_A_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData = newData; //got the first n
	FetchOpcode( waveformFSM ); //need to get the second n
	nextPart = &InstructionManager::LD_InnI_A_3;
}
void InstructionManager::LD_InnI_A_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int targetAddress = (newData << 8 | tempData ); //the ns are loaded in little indian
	waveformFSM->preformMemoryOperation( targetAddress, true, z_80Registers.get8b( aReg )); 
	
	ostringstream cater;
	cater << "LD (nn),A";
	describeInstruction( cater.str(), "saved to a constant specified location in memory the value from the accumulator", targetAddress, z_80LogicGate );
	
	nextPart = &InstructionManager::InstructionFetch;
}

//loads A with I
void InstructionManager::LD_A_I( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int value = z_80Registers.get8b( iReg );
	z_80Registers.set8b( aReg, value );
	z_80Registers.setFlags( value, BYTE_SIZE, value, 0, false, false, TEST, TEST, CLEAR, z_80Registers.getIFF2()?SET:CLEAR, DNTCARE, CLEAR, HOLD ); 
	
	describeInstruction( "LD A,I", "Copies the interrupt vector base into the accumulator", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

//loads A with R
void InstructionManager::LD_A_R( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int value = z_80Registers.get8b( rReg );
	z_80Registers.set8b( aReg, value );
	z_80Registers.setFlags( value, BYTE_SIZE, value, 0, false, false, TEST, TEST, CLEAR, z_80Registers.getIFF2()?SET:CLEAR, DNTCARE, CLEAR, HOLD ); 
	
	describeInstruction( "LD A,R", "Copies R into A", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instructi
}

//loads I with A
void InstructionManager::LD_I_A( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	z_80Registers.set8b( iReg, z_80Registers.get8b( aReg ) );
	
	describeInstruction( "LD I,A", "Initializes the interrupt vector base into the accumulator", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

//loads R with A
void InstructionManager::LD_R_A( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	z_80Registers.set8b( rReg, z_80Registers.get8b( aReg ) );
	
	describeInstruction( "LD R,A", "Copies A into R", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}


//16-Bit Load Group *****************************************

//load a constent 16b literal into a register pair identified by 
//the member variable workingRegisterPair
void InstructionManager::LD_XX_nn( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	FetchOpcode( waveformFSM ); //need to get some nns
	nextPart = &InstructionManager::LD_XX_nn_2;
}
void InstructionManager::LD_XX_nn_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData = newData;
	FetchOpcode( waveformFSM ); //need to get some nns
	nextPart = &InstructionManager::LD_XX_nn_3;
}
void InstructionManager::LD_XX_nn_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//little indian means that the new data gets moved over.
	int nn = (newData << 8) | tempData;
	z_80Registers.set16b( workingRegisterPair, nn );
	
	ostringstream cater;
	cater << "LD " << z_80Registers.get16bName( workingRegisterPair ) << ",nn";
	describeInstruction( cater.str(), "Loads the sixteen bit register with nn", nn, z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

//LD dd,nn
void InstructionManager::LD_dd_nn( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	workingRegisterPair = translateReg16bDD( extractField( opcodeData, DD_POS, DD_LENGTH ) );
	LD_XX_nn( z_80LogicGate, waveformFSM, newData );
}

//LD IX,nn
//LD IY,nn
void InstructionManager::LD_IXY_nn( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	if( (opcodeData & IY_MASK) == IY_MASK ){
		workingRegisterPair = iyReg;
	}else{
		workingRegisterPair = ixReg;
	}
	LD_XX_nn( z_80LogicGate, waveformFSM, newData );
}

//loads a 16b value in memory into a sixteen bit register pair specified by
//the workingRegisterPair mem
void InstructionManager::LD_XX_InnI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//first off we need the nn
	FetchOpcode( waveformFSM );
	nextPart = &InstructionManager::LD_XX_InnI_2;
}
void InstructionManager::LD_XX_InnI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData = newData;
	FetchOpcode( waveformFSM ); //need to get another n
	nextPart = &InstructionManager::LD_XX_InnI_3;
}
void InstructionManager::LD_XX_InnI_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){

	//nn is in little indian format, so the newest data has the most significance
	int nn = tempData = (newData << 8) | tempData;
	waveformFSM->preformMemoryOperation( nn, false );
	nextPart = &InstructionManager::LD_XX_InnI_4;
}
void InstructionManager::LD_XX_InnI_4( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData2 = newData;
	int nn = tempData;
	waveformFSM->preformMemoryOperation( nn+1, false );
	nextPart = &InstructionManager::LD_XX_InnI_5;
}
void InstructionManager::LD_XX_InnI_5( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int readData = (newData << 8)|tempData2; //little indian
	z_80Registers.set16b( workingRegisterPair, readData );
	
	ostringstream cater;
	int nn = tempData;
	cater << "LD " << z_80Registers.get16bName( workingRegisterPair ) << ",(nn)";
	describeInstruction( cater.str(), "Loads the sixteen bit register with the memory value specified by nn", nn, z_80LogicGate );
	
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}
	
//LD HL,(nn)
void InstructionManager::LD_HL_InnI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	workingRegisterPair = hlReg;
	LD_XX_InnI( z_80LogicGate, waveformFSM, newData );
}

//LD dd,(nn)
void InstructionManager::LD_dd_InnI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	workingRegisterPair = translateReg16bDD( extractField( opcodeData, DD_POS, DD_LENGTH ) );
	LD_XX_InnI( z_80LogicGate, waveformFSM, newData );
}
	
//LD IX,(nn)
//LD IY,(nn)
void InstructionManager::LD_IXY_InnI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	if( (opcodeData & IY_MASK) == IY_MASK ){
		workingRegisterPair = iyReg;
	}else{
		workingRegisterPair = ixReg;
	}
	LD_XX_InnI( z_80LogicGate, waveformFSM, newData );
}

void InstructionManager::LD_InnI_XX( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//first off we need the nn
	FetchOpcode( waveformFSM );
	nextPart = &InstructionManager::LD_InnI_XX_2;
}
void InstructionManager::LD_InnI_XX_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData = newData;
	FetchOpcode( waveformFSM ); //need to get another n
	nextPart = &InstructionManager::LD_InnI_XX_3;
}
void InstructionManager::LD_InnI_XX_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//nn is in little indian format, so the newest data has the most significance
	int nn = tempData = (newData << 8) | tempData;
	
	int lowerOrderedByte = (z_80Registers.get16b( workingRegisterPair ) & 0x00FF);
	
	//we will save the lower ordered bytes first. I am guessing that this is the way that it is done
	waveformFSM->preformMemoryOperation( nn, true, lowerOrderedByte );
	nextPart = &InstructionManager::LD_InnI_XX_4;
}
void InstructionManager::LD_InnI_XX_4( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int nn = tempData;
	
	int higherOrderedByte = ((z_80Registers.get16b( workingRegisterPair ) & 0xFF00 ) >> 8 );
	
	waveformFSM->preformMemoryOperation( nn+1, true, higherOrderedByte );
	
	ostringstream cater;
	cater << "LD (nn)," << z_80Registers.get16bName( workingRegisterPair );
	describeInstruction( cater.str(), "Saves the sixteen bit register pair to the memory location nn", nn, z_80LogicGate );
	
	nextPart = &InstructionManager::InstructionFetch;
}
	

//LD (nn),HL
void InstructionManager::LD_InnI_HL( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	workingRegisterPair = hlReg;
	LD_InnI_XX( z_80LogicGate, waveformFSM, newData );
}

//LD (nn),dd
void InstructionManager::LD_InnI_dd( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	workingRegisterPair = translateReg16bDD( extractField( opcodeData, DD_POS, DD_LENGTH ) );
	LD_InnI_XX( z_80LogicGate, waveformFSM, newData );
}

//LD (nn),IX
//LD (nn),IY
void InstructionManager::LD_InnI_IXY( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	if( (opcodeData & IY_MASK) == IY_MASK ){
		workingRegisterPair = iyReg;
	}else{
		workingRegisterPair = ixReg;
	}
	LD_InnI_XX( z_80LogicGate, waveformFSM, newData );
}

//LD SP,HL
void InstructionManager::LD_SP_HL( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	z_80Registers.set16b( spReg, z_80Registers.get16b( hlReg ) );
	
	describeInstruction( "LD SP,HL", "Initializes the stack pointer with the contests of the HL register", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

//LD SP,IX
//LD SP,IY
void InstructionManager::LD_SP_IXY( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	if( (opcodeData & IY_MASK) == IY_MASK ){
		z_80Registers.set16b( spReg, z_80Registers.get16b( iyReg ) );
	}else{
		z_80Registers.set16b( spReg, z_80Registers.get16b( ixReg ) );
	}
	
	ostringstream cater;
	cater << "LD SP," << z_80Registers.get16bName( ((opcodeData & IY_MASK) == IY_MASK)?iyReg:ixReg );
	describeInstruction( cater.str(), "Initializes the SP with the specified sixteen bit register", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

//pushes the the workingRegisterPair onto the stack
void InstructionManager::PUSH_XX( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData = z_80Registers.get16b( workingRegisterPair );
	int stackPointer = tempData2 = z_80Registers.get16b( spReg );
	//we will push the low ordered byte first
	int lowOrderedByte = (tempData & 0x00FF );
	waveformFSM->preformMemoryOperation( stackPointer - 2, true, lowOrderedByte );
	nextPart = &InstructionManager::PUSH_XX_2;
}
void InstructionManager::PUSH_XX_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int stackPointer = tempData2;
	//now we will push the high ordered byte
	int highOrderedByte = ((tempData & 0xFF00 ) >> 8 );
	waveformFSM->preformMemoryOperation( stackPointer - 1, true, highOrderedByte );
	
	//and we need to decriment the stack pointer as well
	z_80Registers.set16b( spReg, stackPointer - 2 );
	
	ostringstream cater;
	cater << "PUSH " << z_80Registers.get16bName( workingRegisterPair );
	describeInstruction( cater.str(), "Pushes the specified sixteen bit register pair onto the stack", z_80LogicGate );
	
	nextPart = &InstructionManager::InstructionFetch;
}

//PUSH qq
void InstructionManager::PUSH_qq( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	workingRegisterPair = translateReg16bQQ( extractField( opcodeData, QQ_POS, QQ_LENGTH ) );
	PUSH_XX( z_80LogicGate, waveformFSM, newData );
}

//PUSH IX
//PUSH IY
void InstructionManager::PUSH_IXY( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	if( (opcodeData & IY_MASK) == IY_MASK ){
		workingRegisterPair = iyReg;
	}else{
		workingRegisterPair = ixReg;
	}
	PUSH_XX( z_80LogicGate, waveformFSM, newData );
}

//This pops a 16b value off of the stack and places it in the workingRegisterPair
void InstructionManager::POP_XX( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int stackPointer = tempData2 = z_80Registers.get16b( spReg );
	
	//first we will read in the higher byte first
	waveformFSM->preformMemoryOperation( stackPointer + 1, false );
	
	nextPart = &InstructionManager::POP_XX_2;
}
void InstructionManager::POP_XX_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int stackPointer = tempData2;
	
	//the higher byte has come in
	tempData = newData;
	//now we will request the lower byte
	waveformFSM->preformMemoryOperation( stackPointer, false );
	
	nextPart = &InstructionManager::POP_XX_3;
}
void InstructionManager::POP_XX_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//now the lower byte has come int
	
	int read16bValue = ((tempData << 8) | newData);
	
	//we will now put it in the working register and then increment the stack pointer
	z_80Registers.set16b( workingRegisterPair, read16bValue  );
	
	z_80Registers.set16b( spReg, z_80Registers.get16b( spReg ) + 2 );
	
	ostringstream cater;
	cater << "POP " << z_80Registers.get16bName( workingRegisterPair );
	describeInstruction( cater.str(), "Pops the specified sixteen bit register pair off of the stack", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

//POP qq
void InstructionManager::POP_qq( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	workingRegisterPair = translateReg16bQQ( extractField( opcodeData, QQ_POS, QQ_LENGTH ) );
	POP_XX( z_80LogicGate, waveformFSM, newData );
}

//POP IX
//POP IY
void InstructionManager::POP_IXY( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	if( (opcodeData & IY_MASK) == IY_MASK ){
		workingRegisterPair = iyReg;
	}else{
		workingRegisterPair = ixReg;
	}
	POP_XX( z_80LogicGate, waveformFSM, newData );
}

//Exchange, Block Transfer, Block Search Groups ******************


//EX DE,HL
void InstructionManager::EX_DE_HL( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int temp = z_80Registers.get16b( deReg );
	z_80Registers.set16b( deReg, z_80Registers.get16b( hlReg ) );
	z_80Registers.set16b( hlReg, temp );
	
	describeInstruction( "EX DE,HL", "Exchanges the contents of the DE and HL sixteen bit registers", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}
	
//EX AF,AF'
void InstructionManager::EX_AF_AFP( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	z_80Registers.exchangeAF();
	
	describeInstruction( "EX AF,AF'", "Exchanges AF with it's alternate pair AF'", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

//EXX
void InstructionManager::EXX( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	z_80Registers.exchangeBCDEHL();
	
	describeInstruction( "EXX", "Exchanges BC, DE and HL with their alternate pairs", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

//exchanges the contents of the workingRegisterPair with the location indexed by the stack pointer
void InstructionManager::EX_ISPI_XX( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( spReg )+1, false );
	nextPart = &InstructionManager::EX_ISPI_XX_2;
}
void InstructionManager::EX_ISPI_XX_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//the higher ordered byte has come in
	tempData = newData;
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( spReg ), false );
	nextPart = &InstructionManager::EX_ISPI_XX_3;
}
void InstructionManager::EX_ISPI_XX_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//the lower ordered byte has come in
	tempData <<= 8;
	tempData |= newData;
	
	//now it is time to set the workingRegister and then start saveing the value that was in it to the stack
	int dataToSave = tempData2 = z_80Registers.get16b( workingRegisterPair );
	z_80Registers.set16b( workingRegisterPair, tempData );
	
	//first we will save out the higher ordered byte at SP + 1
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( spReg ) + 1, true, ((dataToSave & 0xFF00) >> 8 ) );
	
	nextPart = &InstructionManager::EX_ISPI_XX_4;
}
void InstructionManager::EX_ISPI_XX_4( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int dataToSave = tempData2;
	
	//and now we will save out the lower ordered byte at SP + 0
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( spReg ) + 0, true, ( dataToSave & 0x00FF ) );
	
	ostringstream cater;
	cater << "EX (SP)," << z_80Registers.get16bName( workingRegisterPair );
	describeInstruction( cater.str(), "Swaps the value off the top of the stack with the specified sixteen bit register", z_80LogicGate );
	
	nextPart = &InstructionManager::InstructionFetch;
}
	
//EX (SP),HL
void InstructionManager::EX_ISPI_HL( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	workingRegisterPair = hlReg;
	EX_ISPI_XX( z_80LogicGate, waveformFSM, newData );
}
	
//EX (SP),IX
//EX (SP),IY
void InstructionManager::EX_ISPI_IXY( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	if( (opcodeData & IY_MASK) == IY_MASK ){
		workingRegisterPair = iyReg;
	}else{
		workingRegisterPair = ixReg;
	}
	EX_ISPI_XX( z_80LogicGate, waveformFSM, newData );
}

//LDI
//LDIR

//Load (HL) into (DE), increments the pointers and decrement the byte counter BC
//if LDIR then it repeats until BC = 0
void InstructionManager::LDIIRDDR( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	if( opcodeData == LDI_BASE_OPP || opcodeData == LDD_BASE_OPP || z_80Registers.get16b( bcReg ) != 0 ){
		//first we load from HL
		waveformFSM->preformMemoryOperation( z_80Registers.get16b( hlReg ), false );
		nextPart = &InstructionManager::LDIIRDDR_2;
	}else{
		// set flags for LDIR or LDDR finishing
		z_80Registers.setFlags( 0, WORD_SIZE, 0, 0, false, false, HOLD, HOLD, CLEAR, CLEAR, NOTUSED, CLEAR, HOLD );
	
		if( opcodeData == LDIR_BASE_OPP ){
			describeInstruction( "LDIR", "Load (HL) into (DE), increments the pointers, decrement the byte counter BC and repeats until BC = 0", z_80LogicGate );
		}else if( opcodeData == LDDR_BASE_OPP ){
			describeInstruction( "LDDR", "Load (HL) into (DE), decrements the pointers and the byte counter BC and repeats until BC = 0", z_80LogicGate );
		}
				
		InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
	}
}
void InstructionManager::LDIIRDDR_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int readData = newData;
	//now we save it to the address specified by DE
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( deReg ), true, readData );
	
	//we also need to increment/decriment de and hl and decriment BC
	if( opcodeData == LDI_BASE_OPP || opcodeData == LDIR_BASE_OPP ){
	   z_80Registers.inc16b( hlReg );
	   z_80Registers.inc16b( deReg );
	}else{
	   z_80Registers.dec16b( hlReg );
	   z_80Registers.dec16b( deReg );
	}
	z_80Registers.dec16b( bcReg );
	
	bool bcHitZero = z_80Registers.get16b( bcReg ) == 0;
	
	//depending on if we are a LDI or a LDIR we will repeat.
	//The base case is in the targeted function
	if( opcodeData == LDIR_BASE_OPP || opcodeData == LDDR_BASE_OPP ){
		nextPart = &InstructionManager::LDIIRDDR;
	}else{
		//set flags for LDI finnishing
		z_80Registers.setFlags( 0, WORD_SIZE, 0, 0, false, false, HOLD, HOLD, RESET, bcHitZero?RESET:SET, NOTUSED, RESET, HOLD );
		
		if( opcodeData == LDI_BASE_OPP ){
			describeInstruction( "LDI", "Load (HL) into (DE), increments the pointers and decrement the byte counter BC", z_80LogicGate );
		}else if( opcodeData == LDD_BASE_OPP ){
			describeInstruction( "LDD", "Load (HL) into (DE) and decrements the pointers and the byte counter BC", z_80LogicGate );
		}
		
		nextPart = &InstructionManager::InstructionFetch;
	}
}

void InstructionManager::CPIIRDDR( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//we need to do the load wither or not BC is equal to zero so we
	//can do the compare and set  flags
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( hlReg ), false );
	nextPart = &InstructionManager::CPIIRDDR_2;
}
void InstructionManager::CPIIRDDR_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//we now have the HL value and we can do the compare
	int HLReferencedValue = newData;
	int bcValue = z_80Registers.get16b( bcReg );
	int aValue = z_80Registers.get8b( aReg );
	
	bool bcWillHitZero = bcValue == 1;
	
	
	
	//here we keep chugging along if we are not a repeating instruction
	//or it doesn't matter because BC doesn't equal zero and A doesn't
	//equal the referenced hl value
	if( (bcValue != 0 && aValue != HLReferencedValue) ||
	    (opcodeData == CPI_BASE_OPP || opcodeData == CPD_BASE_OPP) ){
	
	
		//do increment or decriment
		if( opcodeData == CPI_BASE_OPP || opcodeData == CPIR_BASE_OPP ){
			z_80Registers.inc16b(hlReg);
		}else{
			z_80Registers.dec16b(hlReg);
		}
		z_80Registers.dec16b(bcReg);
		
		//now we will loop if we are a repeating instruction
		if( opcodeData == CPIR_BASE_OPP || opcodeData == CPDR_BASE_OPP ){
			InstructionManager::CPIIRDDR( z_80LogicGate, waveformFSM, newData );
		}else{
			//we are a non repeating instruction, so we should set the flags and go home
			z_80Registers.setFlags( aValue - HLReferencedValue, BYTE_SIZE, aValue, HLReferencedValue, false, false, TEST, TEST, TEST, bcWillHitZero?RESET:SET, NOTUSED, SET, HOLD );  
		    
		    if( opcodeData == CPI_BASE_OPP ){
		    	describeInstruction( "CPI", "Sets flags by comparing the accumulator with the memory value referenced by HL, increments HL and decrements BC.  If they are equal, Z gets set.  If BC goes to zero P/V gets reset", z_80LogicGate );
		    }else if( opcodeData == CPD_BASE_OPP ){
		    	describeInstruction( "CPD", "Sets flags by comparing the accumulator with the memory value referenced by HL, and decrements HL and BC.  If they are equal, Z gets set.  If BC goes to zero P/V gets reset", z_80LogicGate );
		    }
		    
		    InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next Instruction
		}
	}else{
		//here we quit because we are a reapeating instruction that met
		//a termination
		//Secial Note: I am interperating what the books says liberaly:
		//because this is a repeat instruction, then B-1 does not make sence if we never do the subtraction
		//this I asume with some experimenting with Multi emulator that the P/V flag
		//is set to 1 if the BC does not equal zero when the instruction finnishes
		z_80Registers.setFlags( aValue - HLReferencedValue, BYTE_SIZE, aValue, HLReferencedValue, false, false, TEST, TEST, TEST, bcValue==0?RESET:SET, NOTUSED, SET, HOLD );
		    
	    if( opcodeData == CPIR_BASE_OPP ){
	    	describeInstruction( "CPIR", "Sets flags by comparing the accumulator with the memory value referenced by HL, increments HL, decrements BC, and repeats until they equal or BC reaches zero.  If they are equal, Z gets set.  If BC goes to zero P/V gets reset", z_80LogicGate );
	    }else if( opcodeData == CPDR_BASE_OPP ){
	    	describeInstruction( "CPDR", "Sets flags by comparing the accumulator with the memory value referenced by HL, decrements HL and BC, and repeats until they equal or BC reaches zero.  If they are equal, Z gets set.  If BC goes to zero P/V gets reset", z_80LogicGate );
	    }

		InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next Instruction
	}	 	
}


//8-Bit Arithmetic and Logical Group******************************
	
//preforms an 8b arithmatic operation on register A with the data
//that comes in the newData argument
void InstructionManager::XXX_A_X( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int operation = extractField( opcodeData, ROPP_LOCATION, ROPP_BIT_LENGTH );
	int oldA = z_80Registers.get8b( aReg );
	int argument = newData;
	int result = 0;
	bool hasBarrow, hasCarry;
	hasBarrow = hasCarry = ((z_80Registers.get8b( fReg ) & C_FLAG) == C_FLAG );
	
	switch( operation ){
		case ADD_ROPP:
			result = oldA + argument;
			z_80Registers.set8b( aReg, result );
			z_80Registers.setFlags( result, BYTE_SIZE, oldA, argument, true, false, TEST, TEST, TEST, NOTUSED, TEST, RESET, TEST );
			break;
		case ADC_ROPP:
		    result = oldA + argument;
			if( hasCarry )result++;
			z_80Registers.set8b( aReg, result );
			z_80Registers.setFlags( result, BYTE_SIZE, oldA, argument, true, hasCarry, TEST, TEST, TEST, NOTUSED, TEST, RESET, TEST );
			break;
		case SUB_ROPP:
			result = oldA - argument;
			z_80Registers.set8b( aReg, result );
			z_80Registers.setFlags( result, BYTE_SIZE, oldA, argument, false, false, TEST, TEST, TEST, NOTUSED, TEST, SET, TEST );
			break;
        case SBC_ROPP:
			result = oldA - argument;
		    if( hasBarrow )result--;
			z_80Registers.set8b( aReg, result );
			z_80Registers.setFlags( result, BYTE_SIZE, oldA, argument, false, hasBarrow, TEST, TEST, TEST, NOTUSED, TEST, SET, TEST );
			break;
        case AND_ROPP:
            result = oldA & argument;
            z_80Registers.set8b( aReg, result );
            z_80Registers.setFlags( result, BYTE_SIZE, oldA, argument, false, false, TEST, TEST, SET, TEST, NOTUSED, RESET, RESET);
            break;
        case OR_ROPP:
        	result = oldA | argument;
        	z_80Registers.set8b( aReg, result );
        	z_80Registers.setFlags( result, BYTE_SIZE, oldA, argument, false, false, TEST, TEST, CLEAR, TEST, NOTUSED, CLEAR, CLEAR );
        	break;
        case XOR_ROPP:
        	result = 0x00;
        	for( int i = 1; i < 0x100; i <<= 1 ){
        		if( (( i & oldA ) == i && ( i & argument ) != i) ||
        			(( i & oldA ) != i && ( i & argument ) == i) ){
        			result |= i;
        	    }
        	}
        	z_80Registers.set8b( aReg, result );
        	z_80Registers.setFlags( result, BYTE_SIZE, oldA, argument, false, false, TEST, TEST, CLEAR, TEST, NOTUSED, CLEAR, CLEAR );
        	break;
        case CP_ROPP:
        	//kind of like a fake subtract
        	result = oldA - argument;
        	z_80Registers.setFlags( result, BYTE_SIZE, oldA, argument, false, false, TEST, TEST, TEST, NOTUSED, TEST, SET, TEST);
	}
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next Instruction
}


//This function takes the integer code for a particular type of arithmatic
//operation and returns the name as a string.  This is used as part of 
//the reporting mechanizem which the software tells the user what each
//instruction is doing.
string InstructionManager::operationToString( int operation ){
	switch( operation ){
		case ADD_ROPP: return "ADD"; //returns don't need brakes after them :-)
		case ADC_ROPP: return "ADC";
		case SUB_ROPP: return "SUB";
		case SBC_ROPP: return "SBC";
		case AND_ROPP: return "AND";
        case OR_ROPP:  return "OR";
        case XOR_ROPP: return "XOR";
        case CP_ROPP:  return "CP";
        default:       return "oops";
	}
}

//This funciton returns a portion of the description for different arithmatic
//operations as strings.
string InstructionManager::operationToDescription( int operation ){
	switch( operation ){
		case ADD_ROPP: return "adds it to accumulator"; //returns don't need brakes after them :-)
		case ADC_ROPP: return "adds it to the accumulator.  If the carry is set then the acumilator is also incremented";
		case SUB_ROPP: return "subtracts it from the accumulator.";
		case SBC_ROPP: return "subtracts it from the accumulator.  If the carry flag is set, then the accumulator is also decremented";
		case AND_ROPP: return "ands it with the accumulator.  Each individual bit is 'anded' together.  The result is stored in the accumulator";
        case OR_ROPP:  return "ors it with the accumulator.  Each individual bit is 'ored' together.  The result is stored in the accumulator";
        case XOR_ROPP: return "xors it with the accumulator.  Each individual bit is 'xored' together.  The result is stored in the accumulator";
        case CP_ROPP:  return "compares it with the accumulator.  Think of it like a subtraction that only affects the flags";
        default:       return "uh... does something with it.  I am not sure. A bug has happened and the program is confused";
	}
}

//ADD A,r
//ADC A,r
//SUB A,r
//...
void InstructionManager::XXX_A_r( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int rCode = extractField( opcodeData, XXX_A_R_R_POS, SINGLE_BIT_LENGTH );
	//the argument is passed in by parammiter
	int argument = z_80Registers.get8b( translateReg8b( rCode ) );
	
	ostringstream cater1;
	int operation = extractField( opcodeData, ROPP_LOCATION, ROPP_BIT_LENGTH );
	cater1 << operationToString( operation ) << " A," << z_80Registers.get8bName( translateReg8b( rCode ) );
	ostringstream cater2;
	cater2 << "Gets the " << z_80Registers.get8bName( translateReg8b( rCode ) ) << " register and " << operationToDescription( operation );
	describeInstruction( cater1.str(), cater2.str(), z_80LogicGate );
	
	XXX_A_X( z_80LogicGate, waveformFSM, argument );
}
	
	
//XXX A,n
void InstructionManager::XXX_A_n( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//in this one we will need to get an n from the instruction stream
	FetchOpcode( waveformFSM );
	nextPart = &InstructionManager::XXX_A_n_2;
}

void InstructionManager::XXX_A_n_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int n = newData;
	
	ostringstream cater1;
	int operation = extractField( opcodeData, ROPP_LOCATION, ROPP_BIT_LENGTH );
	cater1 << operationToString( operation ) << " A,n";
	ostringstream cater2;
	cater2 << "Gets the provided constant n and " << operationToDescription( operation );
	describeInstruction( cater1.str(), cater2.str(), n, z_80LogicGate );
	
	XXX_A_X( z_80LogicGate, waveformFSM, n );
}


//XXX A,(HL)
void InstructionManager::XXX_A_IHLI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//get the data from the HL register
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( hlReg ), false );
	//the preformMemoryOperation will deliver its data to the XXX_A_X function
	nextPart = &InstructionManager::XXX_A_IHLI_2;
}
void InstructionManager::XXX_A_IHLI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int loadedValue = newData;
	
	ostringstream cater1;
	int operation = extractField( opcodeData, ROPP_LOCATION, ROPP_BIT_LENGTH );
	cater1 << operationToString( operation ) << " A,(HL)";
	ostringstream cater2;
	cater2 << "Loads the value from memory referenced by HL and " << operationToDescription( operation );
	describeInstruction( cater1.str(), cater2.str(), newData, z_80LogicGate );
	
	XXX_A_X( z_80LogicGate, waveformFSM, loadedValue );
}
	
	
//XXX A,(IX+d)
//XXX A,(IY+d)
void InstructionManager::XXX_A_IIXYPDI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//first off we need to get the d from the instruction stream
	FetchOpcode( waveformFSM );
	nextPart = &InstructionManager::XXX_A_IIXYPDI_2;
}
void InstructionManager::XXX_A_IIXYPDI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int d = tempData = z_80Registers.getSignExtend(  newData , BYTE_SIZE );
	int address = 0;
	if( (opcodeData & IY_MASK) == IY_MASK ){
		address = z_80Registers.get16b( iyReg ) + d;
	}else{
		address = z_80Registers.get16b( ixReg ) + d;
	}
	
	//we now grab the data from that location and then set the next part to be the XXX_A_X function
	waveformFSM->preformMemoryOperation( address, false );
	nextPart = &InstructionManager::XXX_A_IIXYPDI_3;
}
void InstructionManager::XXX_A_IIXYPDI_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int loadedValue = newData;
	
	ostringstream oppText;
	ostringstream description;
	int d = tempData;
	int operation = extractField( opcodeData, ROPP_LOCATION, ROPP_BIT_LENGTH );
	string ixypd = printIXYpd( d, false );
	oppText << operationToString( operation ) << " A," + ixypd;
	description << "Loads the value from memory referenced by " << ixypd << " and " << operationToDescription( operation );
	describeInstruction( oppText.str(), description.str(), newData, z_80LogicGate );
	
	XXX_A_X( z_80LogicGate, waveformFSM, loadedValue );
}
	
//INC r
//DEC r
void InstructionManager::INCDEC_R( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	RSinglet targetR = translateReg8b( extractField( opcodeData, INCDEC_R_R_POS, SINGLE_BIT_LENGTH ) );
	
	ostringstream oppText;
	ostringstream description;
	
	//this is quit a simple function to implement
	if( (opcodeData & DEC_MASK) == DEC_MASK ){
		z_80Registers.dec8b( targetR ); //these functions automatically set the flags
		oppText << "DEC ";
		description << "decrements ";
	}else{
		z_80Registers.inc8b( targetR );
		oppText << "INC ";
		description << "increments ";
	}
	string regName = z_80Registers.get8bName( targetR );
	oppText << regName;
	description << "the " << regName << " register";
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next Instruction
}

//This function will increment or decriment the value addressed in memory by newData
void InstructionManager::INCDEC_IXXI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int address = tempData = newData;
	
	//first we need to get the value
	waveformFSM->preformMemoryOperation( address, false );
	nextPart = &InstructionManager::INCDEC_IXXI_2;
}
void InstructionManager::INCDEC_IXXI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int readData = newData;
	int address = tempData;
	
	//now we inc or dec it
	if( (opcodeData & DEC_MASK) == DEC_MASK ){
		readData--;
	}else{
		readData++;
	}
	
	//now we will write it back
	waveformFSM->preformMemoryOperation( address, true, readData );
	
	nextPart = &InstructionManager::InstructionFetch;
}

//INC (HL)
//DEC (HL)	
void InstructionManager::INCDEC_IHLI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	ostringstream oppText;
	oppText << (((opcodeData & DEC_MASK) == DEC_MASK)?"DEC ":"INC ") << "(HL)";
	ostringstream description;
	description << (((opcodeData & DEC_MASK) == DEC_MASK)?"Decrements ":"increments ") << "the value in memory pointed to by HL";
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	INCDEC_IXXI( z_80LogicGate, waveformFSM, z_80Registers.get16b( hlReg ) );
}

//INC (IX+d)
//DEC (IX+d)
//INC (IY+d)
//DEC (IX+d)
void InstructionManager::INCDEC_IIXYPDI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//first we need to read in the d
	FetchOpcode( waveformFSM );
	nextPart = &InstructionManager::INCDEC_IIXYPDI;
}
void InstructionManager::INCDEC_IIXYPDI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int d = z_80Registers.getSignExtend(  newData , BYTE_SIZE );
	int address = 0;
	
	if( (opcodeData & IY_MASK) == IY_MASK ){
		address = z_80Registers.get16b( iyReg ) + d;
	}else{
		address = z_80Registers.get16b( ixReg ) + d;
	}
	
	ostringstream oppText;
	ostringstream description;
	oppText << (((opcodeData & DEC_MASK) == DEC_MASK)?"DEC ":"INC ") << printIXYpd( d, false );
	description << (((opcodeData & DEC_MASK) == DEC_MASK)?"Decriments ":"Increments ") << "the value in memory pointed to by (";
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	//we then pass the address into the incdec function
	INCDEC_IXXI( z_80LogicGate, waveformFSM, address );
}

//General Purpose Arithmetic and CPU Control Groups***************

//DDA
void InstructionManager::DDA( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	z_80Registers.BCDUpdate();
	
	describeInstruction( "DDA", "Converts the contents of the accumulator back into BCD following an addition or subtraction", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next Instruction
}

//CPL
void InstructionManager::CPL( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	
	int oldValue = z_80Registers.get8b( aReg );
	int newValue = -1 - oldValue;
	
	z_80Registers.set8b( aReg, newValue );
	z_80Registers.setFlags( newValue, BYTE_SIZE, oldValue, 0, false, false, HOLD, HOLD, SET, HOLD, NOTUSED, SET, HOLD );
	
	describeInstruction( "CPL", "Inverts all the bits in the accumulator", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next Instruction
}

//NEG
void InstructionManager::NEG( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	z_80Registers.neg();
	
	describeInstruction( "NEG", "Preforms the 2's complement negation of the accumulator's contents", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next Instruction
}

//CCF
void InstructionManager::CFF( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	bool cFlagWasSet = ((z_80Registers.get8b( fReg ) & C_FLAG) == C_FLAG );
	z_80Registers.setFlags( 0, BYTE_SIZE, 0, 0, false, false, HOLD, HOLD, DNTCARE, HOLD, NOTUSED, RESET, cFlagWasSet?RESET:SET );
	
	describeInstruction( "CCF", "Toggles the carry flag", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next Instruction
}

//SCF
void InstructionManager::SCF( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	z_80Registers.setFlags( 0, BYTE_SIZE, 0, 0, false, false, HOLD, HOLD, RESET, HOLD, NOTUSED, RESET, SET );
	
	describeInstruction( "SCF", "Sets the carry flag", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next Instruction
}

//NOP
void InstructionManager::NOP( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	
	describeInstruction( "NOP", "No operation", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next Instruction
}

//HALT
void InstructionManager::HALT( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	
	describeInstruction( "HALT", "Stops the machine until it is reset or an interrupt is received", z_80LogicGate );
	
	waveformFSM->preformHalt( z_80LogicGate );
	nextPart = &InstructionManager::HALT_2;
}
void InstructionManager::HALT_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	
	//here we check to see if we execute another NOOP for the
	//halt or if we interupt
	if( !checkInterupts( waveformFSM, z_80LogicGate ) ){
		waveformFSM->preformHalt( z_80LogicGate );
		nextPart = &InstructionManager::HALT_2;
		//if we want halts to single step then we need the next line uncomented
		//if( iStepModeEnabled ) z_80LogicGate->breakSimulation();
	}
}


//DI
void InstructionManager::DI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	z_80Registers.setIFF1( false );
	z_80Registers.setIFF2( false );
	
	//doesn't effect flags

	describeInstruction( "DI", "Disables the interrupts so that the mask able interrupt becomes masked", z_80LogicGate );
	
	//DI and EI do not check for interups.
	//This is automatically handled in the checkInterupts routine
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next Instruction
}

//EI
void InstructionManager::EI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	z_80Registers.setIFF1( true );
	z_80Registers.setIFF2( true );
	
	//doesn't effect flags
	describeInstruction( "EI", "Enables the mask able interrupt pin to be able to receive interrupts", z_80LogicGate );
	
	//DI and EI do not check for interups.
	//This is automatically handled in the checkInterupts rutine
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next Instruction
}

//IM 0
//IM 1
//IM 2
void InstructionManager::IM_0_1_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int modeCode = extractField( opcodeData, IM_NUM_LOCATION, IM_NUM_LENGTH );
	
	switch( modeCode ){
		case IM_MODE_0_CODE:
			describeInstruction( "IM 0", "Sets interrupt mode to mode 0.  Reset instruction will now be read from interrupting device", z_80LogicGate );
			break;
		case IM_MODE_1_CODE:
			describeInstruction( "IM 1", "Sets interrupt mode to mode 1.  Always will interrupt to 0038H", z_80LogicGate );
			break;
		case IM_MODE_2_CODE:
			describeInstruction( "IM 2", "Sets interrupt mode to mode 2.  Calls function pointer found in vector table entry indexed by I and device byte", z_80LogicGate );
			break;
	}
	
	//this function sets the interupt mode
	z_80Registers.setIntModeCode( modeCode );
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next Instruction
}
	

//16-Bit Arithmetic Group ****************************************

//ADD HL,ss
void InstructionManager::ADD_HL_ss( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//extract code
	int ssCode = extractField( opcodeData, SS_POS, SS_LENGTH );
	
	//preform increment
	z_80Registers.add16b( hlReg, translateReg16bSS( ssCode ), false );
	
	string regName = z_80Registers.get16bName( translateReg16bSS( ssCode ) );
	ostringstream oppText;
	oppText << "ADD HL," << regName;
	ostringstream description;
	description << "Adds the value in " << regName << " to the value in HL and stores it in HL";
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next Instruction
}

//ADC HL,ss	
void InstructionManager::ADC_HL_ss( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//extract code
	int ssCode = extractField( opcodeData, SS_POS, SS_LENGTH );
	
	//preform add
	z_80Registers.add16b( hlReg, translateReg16bSS( ssCode ), true );
	
	string regName = z_80Registers.get16bName( translateReg16bSS( ssCode ) );
	ostringstream oppText;
	oppText << "ADC HL," << regName;
	ostringstream description;
	description << "Adds the value in " << regName << " to the value in HL and stores it in HL with a possable cary in from a previouse add";
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next Instruction
}

//SBC HL,ss
void InstructionManager::SBC_HL_ss( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//extract code
	int ssCode = extractField( opcodeData, SS_POS, SS_LENGTH );
	
	//preform subtract with cary
	z_80Registers.sbcHL( translateReg16bSS( ssCode ) );
	
	string regName = z_80Registers.get16bName( translateReg16bSS( ssCode ) );
	ostringstream oppText;
	oppText << "SBC HL," << regName;
	ostringstream description;
	description << "Subtracts the value in " << regName << " from the value in HL and stores it in HL with a possible barrow in from a previous subtraction";
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next Instruction
}
	
//ADD IX,pp
//ADD IY,rr
void InstructionManager::ADD_IXY_pprr( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	
	ostringstream oppText;
	ostringstream description;
	oppText << "ADD ";
	description << "Adds the value from ";
	
	RDoublet source = bcReg;
	RDoublet destination = ixReg;
	//determine source and destination
	if( (opcodeData & IY_MASK ) == IY_MASK ){
		source = translateReg16bRR( extractField( opcodeData, RR_POS, RR_LENGTH ) );
		destination = iyReg;
		string sourceName = z_80Registers.get16bName( source );
		oppText << "IY," << sourceName;
		description << sourceName << " into IY";
	}else{
		source = translateReg16bPP( extractField( opcodeData, PP_POS, PP_LENGTH ) );
		destination = ixReg;
		string sourceName = z_80Registers.get16bName( source );
		oppText << "IX," << sourceName;
		description << sourceName << " into IX";
	}
	
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	z_80Registers.add16b( destination, source, false );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next Instruction
}


//INC ss
//DEC ss
void InstructionManager::INCDEC_ss( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	RDoublet target = translateReg16bSS( extractField( opcodeData, SS_POS, SS_LENGTH ) );
	
	ostringstream oppText;
	ostringstream description;
	
	if( (opcodeData & DEC_MASK_16b) == DEC_MASK_16b ){
		z_80Registers.dec16b( target );
		oppText << "DEC ";
		description << "Decrements ";
	}else{
		z_80Registers.inc16b( target );
		oppText << "INC ";
		description << "Increments ";
	}
	
	string targetName = z_80Registers.get16bName( target );
	oppText << targetName;
	description << "the value in 16b registers " << targetName;
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next Instruction
}

// INC IX
// DEC IX
// INC IY
// DEC IY
void InstructionManager::INCDEC_IXY( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//determine source
	RDoublet target = ixReg;
	if( (opcodeData & IY_MASK) == IY_MASK ){
		target = iyReg;
	}
	
	ostringstream oppText;
	ostringstream description;
	
	//determine action
	if( (opcodeData & DEC_MASK_16b) == DEC_MASK_16b ){
		z_80Registers.dec16b( target );
		oppText << "DEC ";
		description << "Decrements ";
	}else{
		z_80Registers.inc16b( target );
		oppText << "INC ";
		description << "Increments ";
	}
	
	string targetName = z_80Registers.get16bName( target );
	oppText << targetName;
	description << "the value in " << targetName;
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next Instruction
}

//this method tells the instruction manager wither or not it
//should tell the Z_80LogicGate to block at the end of each instruction
//this method is called by the WaveformFSM::setRunMode 
void InstructionManager::setIStepMode( bool shouldStep ){
	iStepModeEnabled = shouldStep;
}

//this method is called by the waveformFSM when it has been
//told by the Z_80LogicGate it has been time to reset
void InstructionManager::doReset( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM ){
	z_80Registers.resetData( z_80LogicGate );
	waveformFSM->preformResetCycle();
	interruptCase = DIDNT_interrupt; //interupt an interupt
	opcodeData = NOP_BASE_OPP;
	systemJustRestarted = true;
	describeInstruction( "reseting", "The Z_80 has recieved a reset. IFF, the PC, I, R and the interupt mode are all reset", z_80LogicGate );
	nextPart = &InstructionManager::InstructionFetch;
}

//sets the current zad mode.  See the definition over the zadMode
//private member variable
void InstructionManager::setZADMode( ZAD_MODE newZadMode, Z_80LogicGate* z_80LogicGate ){
	zadMode = newZadMode;
	z_80LogicGate->notifyOfZadModeChange();
}

//returns the current ZAD_MODE. See the definition over the zadMode
//private member variable
ZAD_MODE InstructionManager::getZADMode(){
	return zadMode;
}

//this function returns the current value of a named
//register.  This function is called by WaveformFSM::getReg
string InstructionManager::getReg( string name ){
	return z_80Registers.getReg( name );
}

//this method is so that the user can manipulate the values of the
//registers.  It is called by waveformFSM:setReg
void InstructionManager::setReg( string regName, string newValue ){
	z_80Registers.setReg( regName, newValue );
}

void InstructionManager::InstructionFetch( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	
	//we are at the start of a new instruction.  We should pause the system.
	if( iStepModeEnabled ) z_80LogicGate->breakSimulation();
	
	//but we will also schedual the events that shall transpire in the future as well.
	if( !checkInterupts( waveformFSM, z_80LogicGate ) ){
		//we can't just call FetchOpcode because we need to increment the
		//pc on the next clock.  
		waveformFSM->fetchOpcode( z_80Registers.get16b( pcReg ) );
		nextPart = &InstructionManager::InstructionFetch_2;
	}
}
void InstructionManager::InstructionFetch_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	z_80Registers.inc16b( pcReg );
	checkForZADCode( z_80LogicGate, waveformFSM, newData );
}


//This function is the braking point which breakes out on the
//special ZAD subrutines to emulate them in code.
//There are two stipulations for the brake out to accure,
//the pc must have just been incremented past the
//special address and the opcode fetched from that location
//must be a NOP.  This makes it so that someone can
//create a genuine function at that location that doesn't
//get short circuited
void InstructionManager::checkForZADCode( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	
	if( zadMode != ZAD_MODE_OFF ){
		
		int pcLastValue = z_80Registers.get16b( pcReg ) - 1;
		
		//The AUTO_1800 wants to be called *if* the opcode is a no-op
		//Thus it can not be in the switch statement with everyone
		//else
		if( pcLastValue == ADDRESS_ZERO ){
				AUTO_1800( z_80LogicGate, waveformFSM, newData );
		}else if( ((unsigned int)newData) != NOP_BASE_OPP && zadMode == ZAD_MODE_SMART ){
			//if the opcode isn't a NOOP, then we don't want to brake out
			//if we are in smart mode
			InstructionDecode( z_80LogicGate, waveformFSM, newData );
		}else switch( pcLastValue ){
			case KBRD_ADDRESS:
				KBRD_EM( z_80LogicGate, waveformFSM, newData );
				break;
			case SCAN_ADDRESS:
				SCAN_EM( z_80LogicGate, waveformFSM, newData );
				break;
			case HEXTO7_ADDRESS:
				HEXTO7_EM( z_80LogicGate, waveformFSM, newData );
				break;
			case MESOUT_ADDRESS:
				MESOUT_EM( z_80LogicGate, waveformFSM, newData );
				break;
			case TONE_ADDRESS:
				TONE_EM( z_80LogicGate, waveformFSM, newData );
				break;
			case DELAY_ADDRESS:
				DELAY_EM( z_80LogicGate, waveformFSM, newData );
				break;
			case INT_MODE_1_RESTART_LOCATION:
				INT_MODE1_EM( z_80LogicGate, waveformFSM, newData );
				break;
			default:
				InstructionDecode( z_80LogicGate, waveformFSM, newData );
				break;
		}
	}else{
		
		//if the ZAD mode is off we will always just decode the opcode and go on with life.
		InstructionDecode( z_80LogicGate, waveformFSM, newData );
	}
}

//the ZAD trainer board expects programs to start at address 1800h.
//This function reads in 0x0000 and if it is a no-op, it will
//asume that the program is realy starting at 1800h and redirect the
//program there.
void InstructionManager::AUTO_1800( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	if( systemJustRestarted && ((unsigned int)newData == NOP_BASE_OPP || zadMode == ZAD_MODE_ON) ){
		//redirect the program counter
		z_80Registers.set16b( pcReg, ZAD_START_ADDRESS );
		
		if( zadMode == ZAD_MODE_SMART ){
			describeInstruction( "1800h Redirect", "Opcode at 0x0000 was a nop, automatically redirecting to 0x1800 in search of start of program.", z_80LogicGate );
		}else{
			describeInstruction( "1800h Redirect", "ZAD mode is on. Jumping to address 0x1800 for start of program.", z_80LogicGate );
		}
		
		//and restart the instruction execution
		InstructionFetch( z_80LogicGate, waveformFSM, newData );
	}else{
		//there was an opcode there, so we will
		//process it instead
		InstructionDecode( z_80LogicGate, waveformFSM, newData );
	}
}
//This scans the keyboard and refreshes the display.
//it will not return until a keypad key is pressed
void InstructionManager::KBRD_EM( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//first flush any old history of this file so we can wait for a new press
	z_80LogicGate->resetCurrentKeyboardHardCode();
	InstructionManager::KBRD_EM_2( z_80LogicGate, waveformFSM, newData );
}
void InstructionManager::KBRD_EM_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	
	z_80LogicGate->setHexDisplay( string( segmentVectorCArray ) );
	int currentKey = z_80LogicGate->getCurrentKeyboardHardCode();
	if( currentKey == H_KEY_NONE ){
		
		waveformFSM->preformMemoryOperation( KBRD_ADDRESS, false );
		nextPart = &InstructionManager::KBRD_EM_2;
		describeInstruction( "Call KBRD", "Waits for a key press and returns a soft keycode. Currently Waiting.", z_80LogicGate );
	}else{
		
		switch( currentKey ){
			case H_KEY_0:
			   z_80Registers.set8b( aReg, 0 );
			   break;
		    case H_KEY_1:
			   z_80Registers.set8b( aReg, 1 );
			   break;
		    case H_KEY_2:
			   z_80Registers.set8b( aReg, 2 );
			   break;
		    case H_KEY_3:
			   z_80Registers.set8b( aReg, 3 );
			   break;
		    case H_KEY_4:
			   z_80Registers.set8b( aReg, 4 );
			   break;
		    case H_KEY_5:
			   z_80Registers.set8b( aReg, 5 );
			   break;
		    case H_KEY_6:
			   z_80Registers.set8b( aReg, 6 );
			   break;
		    case H_KEY_7:
			   z_80Registers.set8b( aReg, 7 );
			   break;
		    case H_KEY_8:
			   z_80Registers.set8b( aReg, 8 );
			   break;
		    case H_KEY_9:
			   z_80Registers.set8b( aReg, 9 );
			   break;
		    case H_KEY_A:
			   z_80Registers.set8b( aReg, 0xA );
			   break;
		    case H_KEY_B:
			   z_80Registers.set8b( aReg, 0xB );
			   break;
		    case H_KEY_C:
			   z_80Registers.set8b( aReg, 0xC );
			   break;
		    case H_KEY_D:
			   z_80Registers.set8b( aReg, 0xD );
			   break;
		    case H_KEY_E:
			   z_80Registers.set8b( aReg, 0xE );
			   break;
		    case H_KEY_F:
			   z_80Registers.set8b( aReg, 0xF );
			   break;
		    case H_KEY_NEXT:
			   z_80Registers.set8b( aReg, S_KEY_NEXT );
			   break;
		    case H_KEY_PREV:
			   z_80Registers.set8b( aReg, S_KEY_PREV );
			   break;
		    case H_KEY_GO:
			   z_80Registers.set8b( aReg, S_KEY_GO );
			   break;
		    case H_KEY_STEP:
			   z_80Registers.set8b( aReg, S_KEY_STEP );
			   break;
		    case H_KEY_DATA:
			   z_80Registers.set8b( aReg, S_KEY_DATA );
			   break;
		    case H_KEY_SET_BRK_PT:
			   z_80Registers.set8b( aReg, S_KEY_SET_BRK_PT );
			   break;
		    case H_KEY_INSERT:
			   z_80Registers.set8b( aReg, S_KEY_INSERT );
			   break;
		    case H_KEY_DELETE:
			   z_80Registers.set8b( aReg, S_KEY_DELETE );
			   break;
		    case H_KEY_PC:
			   z_80Registers.set8b( aReg, S_KEY_PC );
			   break;
		    case H_KEY_ADDR:
			   z_80Registers.set8b( aReg, S_KEY_ADDR );
			   break;
		    case H_KEY_CLR_BRK_PT:
			   z_80Registers.set8b( aReg, S_KEY_CLR_BRK_PT );
			   break;
		    case H_KEY_REG:
			   z_80Registers.set8b( aReg, S_KEY_REG );
			   break;
		    case H_KEY_COPY:
			   z_80Registers.set8b( aReg, S_KEY_COPY );
			   break;
		    case H_KEY_RELA:
			   z_80Registers.set8b( aReg, S_KEY_RELA );
			   break;
		    case H_KEY_DUMP:
			   z_80Registers.set8b( aReg, S_KEY_DUMP );
			   break;
		    case H_KEY_LOAD:
			   z_80Registers.set8b( aReg, S_KEY_LOAD );
			   break;
		}
		RET( z_80LogicGate, waveformFSM, newData );
		describeInstruction( "Call KBRD", "Waits for a key press and returns a soft keycode", z_80LogicGate );
		
	}
}

//This will scan the keyboard and refreshes the display exactly once,
//and then returns to the caller immediaetly
//the return value is placed in the A reg
void InstructionManager::SCAN_EM( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//reset real old key codes
	if( z_80LogicGate->getKeyboardHardCodeAge() > SCAN_KEY_TIME_OUT ){
		z_80LogicGate->resetCurrentKeyboardHardCode();
	}
	
	int keyCode = z_80LogicGate->getCurrentKeyboardHardCode();
	z_80Registers.setFlags( keyCode, BYTE_SIZE, 0, keyCode, true, false, HOLD, TEST, HOLD, HOLD, NOTUSED, HOLD, HOLD );
	z_80Registers.set8b( aReg, keyCode );
	z_80LogicGate->setHexDisplay( string( segmentVectorCArray ) );
	RET( z_80LogicGate, waveformFSM, newData );
	
	ostringstream description;
	description << "Scans the keyboard once and returns a hard keycode.  Key presses expire after " << SCAN_KEY_TIME_OUT << " time steps";
	describeInstruction( "Call SCAN", description.str(), z_80LogicGate );
}


//HEXTO7 Address: 0FF1H  Hex data found in the display
//vector is converted to 7-segment numberic codes, and
//copied intot the segment vector.

//the first thing we need to do is grab the bytes out of
//memory at the display vector
void InstructionManager::HEXTO7_EM( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	waveformFSM->preformMemoryOperation( DISPV_ADDRESS, false );
	nextPart = &InstructionManager::HEXTO7_EM_2;
}
void InstructionManager::HEXTO7_EM_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData = (newData << (BITS_IN_BYTE*2));
	waveformFSM->preformMemoryOperation( DISPV_ADDRESS + 1, false );
	nextPart = &InstructionManager::HEXTO7_EM_3;
}
void InstructionManager::HEXTO7_EM_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData |= (newData << BITS_IN_BYTE );
	waveformFSM->preformMemoryOperation( DISPV_ADDRESS + 2, false );
	nextPart = &InstructionManager::HEXTO7_EM_4;
}
void InstructionManager::HEXTO7_EM_4( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData |= newData;
	ostringstream stream;
	stream << uppercase << hex << setfill( '0' ) << setw(6) << tempData;
	string hexAsString = stream.str();
	for( int i = 0; i < NUM_DISP_DIGITS; ++i ){
		segmentVectorCArray[i] = hexAsString.at(i);
	}
	//now because this function was called we will return
	RET( z_80LogicGate, waveformFSM, newData );
	describeInstruction( "Call HEXTO7", "Prepares the hex digits in the display vector (DISPV) for displaying by KBRD or SCAN", z_80LogicGate );
}

//Prepares a 6-character ASCII message for display
//IX should point to the first characters immediately
//following the first
void InstructionManager::MESOUT_EM( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData = 6;//six characters
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( ixReg ), false );
	nextPart = &InstructionManager::MESOUT_EM_2;
}
void InstructionManager::MESOUT_EM_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	if( tempData > 1 ){
		//check to make sure that the character is a valid
		//character
		if( (newData >= 'A' && newData <= 'Z')
		  || (newData >= '0' && newData <= '9' )
		  || (newData == '+') || (newData == '-' )
	      || (newData == '_') || (newData == '(' )
	      || (newData == ')') ){
			segmentVectorCArray[ 6-tempData ] = (char)newData;
		}else{
			segmentVectorCArray[ 6-tempData ] = ' ';
	    }
		
		tempData--;
		int newIXValue = z_80Registers.get16b( ixReg ) + 1;
		z_80Registers.set16b( ixReg, newIXValue );
		waveformFSM->preformMemoryOperation( newIXValue, false );
		nextPart = &InstructionManager::MESOUT_EM_2;
	}else{
		if( (newData >= 'A' && newData <= 'Z')
		  || (newData >= '0' && newData <= '9' )
		  || (newData == '+') || (newData == '-' )
	      || (newData == '_') || (newData == '(' )
	      || (newData == ')') ){
			segmentVectorCArray[ 5 ] = (char)newData;
		}else{
			segmentVectorCArray[ 6-tempData ] = ' ';
	    }
		RET(z_80LogicGate, waveformFSM, newData);
		describeInstruction( "Call MESOUT", "Prepares the six digit ASCII string pointed to by IX for displaying by KBRD or SCAN", z_80LogicGate );
	}
}

void InstructionManager::TONE_EM( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//TODO
	RET( z_80LogicGate, waveformFSM, newData );
	describeInstruction( "Call TONE", "Causes a tone to be generated by the speakers with frequency specified by TCHPER (1F49H) with length specified by HL", z_80LogicGate );		
}

//This will delay the number of 'millisecions' in HL
//This will execute instruction fetches from its own memory
//address to stall time.
//It executes aproxamatly the same number of instructions
//as the real one would have
void InstructionManager::DELAY_EM( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData = z_80Registers.get16b( hlReg ) * (126 * 2 + 5);
	DELAY_EM_2( z_80LogicGate, waveformFSM, newData );
}
void InstructionManager::DELAY_EM_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	if( tempData == 0 ){
		RET(z_80LogicGate, waveformFSM, newData);
		describeInstruction( "Call DELAY", "Pauses the system by the amount specified in HL", z_80LogicGate );
	}else{
		//this is what will take up time
		waveformFSM->fetchOpcode( DELAY_ADDRESS );
		nextPart = &InstructionManager::DELAY_EM_2;
		tempData--;
	}
}

//The 'operating system' has code at this location to
//jump to the address specified by the memory locations
//1F41H and 1F42H
void InstructionManager::INT_MODE1_EM( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//first load 1F41H
	waveformFSM->preformMemoryOperation( ZAD_INT1_TARGET, false );
	nextPart = &InstructionManager::INT_MODE1_EM_2;
}
void InstructionManager::INT_MODE1_EM_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData = newData;
	//now load 1F42H
	waveformFSM->preformMemoryOperation( ZAD_INT1_TARGET+1, false );
	nextPart = &InstructionManager::INT_MODE1_EM_3;
}
void InstructionManager::INT_MODE1_EM_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int interuptTarget = ( newData << 8 ) | tempData;
	
	//debug
	cout << "INT_MODE1_EM tameData: " << tempData << " newData: " << newData << " interuptTarget: " << interuptTarget << endl;
	
	z_80Registers.set16b( pcReg, interuptTarget );
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
	
	ostringstream description;
	description << "A NMI has accured jumping to address 0038H. The pc will now be set to the two byte address located at 1F41H and 1F42H which is ";
	description << hex << uppercase << interuptTarget << "H";
	describeInstruction( "Mode 1 NMI", description.str(), z_80LogicGate );
}


void InstructionManager::FetchOpcode( WaveformFSM* waveformFSM ){
	waveformFSM->fetchOpcode( z_80Registers.get16b( pcReg ) );
	z_80Registers.inc16b( pcReg );
}

//Called  by WaveformFSM to request service
void InstructionManager::requestInstruction( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	(this->*nextPart)( z_80LogicGate, waveformFSM, newData );
}



//newData should contain an opcode
void InstructionManager::InstructionDecode( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	opcodeData = (unsigned long)newData;
	systemJustRestarted = false;
	if( this->opcodeHash[ opcodeData ] != NULL ){
		(this->*opcodeHash[ opcodeData ])( z_80LogicGate, waveformFSM, newData );
	}else{
		//bad opcodes will be treated as no-ops
		InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
	}
}

//Rotate and Shift Group********************************
//RLCA
void InstructionManager::RLCA( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int aValue = z_80Registers.get8b( aReg );
	bool signBitSet = ( aValue & SIGN_8BIT_MASK ) == SIGN_8BIT_MASK;
	aValue <<= 1;
	if( signBitSet ){
		aValue |= 1;
	}
	z_80Registers.set8b( aReg, aValue );
	z_80Registers.setFlags( aValue, BYTE_SIZE, aValue, 0, false, false, HOLD, HOLD, RESET, NOTUSED, HOLD, CLEAR, signBitSet?SET:RESET );
	
	describeInstruction( "RLCA", "Shifts the bits in the accumulator left popping the most significant bit into the carry flag and into the least significant position", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

//RLA
void InstructionManager::RLA( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int aValue = z_80Registers.get8b( aReg );
	bool signBitSet = ( aValue & SIGN_8BIT_MASK ) == SIGN_8BIT_MASK;
	aValue <<= 1;
	if( (z_80Registers.get8b( fReg ) & C_FLAG)==C_FLAG ){
		aValue |= 1;
	}
	z_80Registers.set8b( aReg, aValue );
	z_80Registers.setFlags( aValue, BYTE_SIZE, aValue, 0, false, false, HOLD, HOLD, RESET, NOTUSED, HOLD, CLEAR, signBitSet?SET:RESET );
	
	describeInstruction( "RLA", "Shifts the bits in the accumulator left popping the most significant bit into the carry flag, and the carry flag into the least significant position", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}
	
//RRCA
void InstructionManager::RRCA( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int aValue = z_80Registers.get8b( aReg );
	bool firstBitSet = ( aValue & 0x01 ) == 0x01;
	aValue >>= 1;
	if( firstBitSet ){
		aValue |= SIGN_8BIT_MASK;
	}
	z_80Registers.set8b( aReg, aValue );
	z_80Registers.setFlags( aValue, BYTE_SIZE, aValue, 0, false, false, HOLD, HOLD, RESET, NOTUSED, HOLD, CLEAR, firstBitSet?SET:RESET );
	
	describeInstruction( "RRCA", "Shifts the bits in the accumulator right popping the least significant bit into the carry flag and into the most significant position", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}
	
//RRA
void InstructionManager::RRA( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int aValue = z_80Registers.get8b( aReg );
	bool firstBitSet = ( aValue & 0x01 ) == 0x01;
	aValue >>= 1;
	if( (z_80Registers.get8b( fReg ) & C_FLAG)==C_FLAG ){
		aValue |= SIGN_8BIT_MASK;
	}
	z_80Registers.set8b( aReg, aValue );
	z_80Registers.setFlags( aValue, BYTE_SIZE, aValue, 0, false, false, HOLD, HOLD, RESET, NOTUSED, HOLD, CLEAR, firstBitSet?SET:RESET );
	
	describeInstruction( "RRA", "Shifts the bits in the accumulator right popping the least significant bit into the carry flag, and the carry flag into the most significant position", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

//This function takes a value rotates it according to the rotateMove.
//This function also sets the flags
int InstructionManager::rotateize( int oldValue, int rotationAction ){
	int newValue = 0;
	bool signBitSet = (oldValue & SIGN_8BIT_MASK) == SIGN_8BIT_MASK;
	bool firstBitWasSet = (oldValue & 0x01 ) == 0x01;
	bool cFlagWasSet = (z_80Registers.get8b( fReg ) & C_FLAG) == C_FLAG;
	bool setCFlag = false;
	int firstBitFromFlag = cFlagWasSet?0x01:0;
	int firstBitFromSign = signBitSet?0x01:0;
	int signBitFromFlag = cFlagWasSet?SIGN_8BIT_MASK:0;
	int signBitFromFirst = firstBitWasSet?SIGN_8BIT_MASK:0;
	int signBitFromSign = signBitSet?SIGN_8BIT_MASK:0;
	
	switch( rotationAction ){
		case ROTATER_RLC:
			newValue = (oldValue << 1) | firstBitFromSign;
			setCFlag = signBitSet;
			break;
		case ROTATER_RL:
			newValue = (oldValue << 1 ) | firstBitFromFlag;
			setCFlag = signBitSet;
			break;
		case ROTATER_RRC:
			newValue = (oldValue >> 1 ) | signBitFromFirst;
			setCFlag = firstBitWasSet;
			break;
		case ROTATER_RR:
			newValue = (oldValue >> 1 ) | signBitFromFlag;
			setCFlag = firstBitWasSet;
			break;
		case ROTATER_SLA:
			newValue = (oldValue << 1 );
			setCFlag = signBitSet;
			break;
		case ROTATER_SRA:
			newValue = (oldValue >> 1 ) | signBitFromSign;
			setCFlag = firstBitWasSet;
			break;
		case ROTATER_SRL:
			newValue = (oldValue >> 1 );
			setCFlag = firstBitWasSet;
			break;
	}
	
	z_80Registers.setFlags( newValue, BYTE_SIZE, oldValue, 0, false, false, HOLD, HOLD, CLEAR, TEST, NOTUSED, RESET, setCFlag?SET:RESET );
	
	return newValue;
}

//looks up the name of the rotation operation so that we
//can show the instruction in the pop-up window
string decodeRotationName( int rotationAction ){
	switch( rotationAction ){
		case ROTATER_RLC: return "RLC";
		case ROTATER_RL:  return "RL";
		case ROTATER_RRC: return "RRC";
		case ROTATER_RR:  return "RR";
		case ROTATER_SLA: return "SLA";
		case ROTATER_SRA: return "SRA";
		case ROTATER_SRL: return "SRL";
		default:          return "oops";
	}
}

//returns a partial description of the operation of the rotation
//for the pop-up window
string decodeRotationDescription( int rotationAction ){
	switch( rotationAction ){
		case ROTATER_RLC: return "shifts the bits left popping the most significant into the carry flag and back in as least significant";
		case ROTATER_RL:  return "shifts the bits left popping the most significant into the carry flag and the carry flag in as the least significant";
		case ROTATER_RRC: return "shifts the bits right popping the least significant into the carry flag and back in as the most significant";
		case ROTATER_RR:  return "shifts the bits right popping the least significant into the carry flag and the carry flag in as the most significant";
		case ROTATER_SLA: return "shifts the bits right popping the most significant into the carry flag with a zero bit filling the least significant";
		case ROTATER_SRA: return "shifts the bits left popping the least significant into the carry flag without changing the sign bit";
		case ROTATER_SRL: return "shifts the bits right popping the least significant into the carry flag and setting the most significant to zero";
		default:          return "oops, this program has a bug because I have no idea what is currently going on...";
	}
}

//RLC r
//RL r
//RRC r
//RR r
//SLA r
//SRA r
//SRL r
void InstructionManager::ROTATE_R( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	RSinglet reg = translateReg8b( extractField( opcodeData, ROTATER_R_LOCATION, SINGLE_BIT_LENGTH ) );
	int rotationAction = extractField( opcodeData, ROTATER_LOCATION, ROTATER_LENGTH );
	z_80Registers.set8b( reg, rotateize( z_80Registers.get8b( reg ), rotationAction ) );
	
	ostringstream oppText;
	oppText << decodeRotationName( rotationAction ) << " " << z_80Registers.get8bName( reg );
	ostringstream description;
	description << "Takes the value from the " << z_80Registers.get8bName( reg ) << " and " << decodeRotationDescription( rotationAction );
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

 
//RLC (HL)
//RL (HL)
//RRC (HL)
//RR (HL)
//SLA (HL)
//SRA (HL)
//SRL (HL)
void InstructionManager::ROTATE_IHLI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//we need to first read in the value from HL
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( hlReg ), false );
	nextPart = &InstructionManager::ROTATE_IHLI_2;
}
void InstructionManager::ROTATE_IHLI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int rotationAction = extractField( opcodeData, ROTATER_LOCATION, ROTATER_LENGTH );
	int oldHLValue = newData;
	int newHLValue = rotateize( oldHLValue, rotationAction );
	
	//now we need to save the HL value back
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( hlReg ), true, newHLValue );
	
	ostringstream oppText;
	oppText << decodeRotationName( rotationAction ) << " (HL)";
	ostringstream description;
	description << "Referencing the value in memory pointed to by (HL) it " << decodeRotationDescription( rotationAction );
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	nextPart = &InstructionManager::InstructionFetch;
}
	
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
void InstructionManager::ROTATE_IIXYpDI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int d = tempData = z_80Registers.getSignExtend(  ( opcodeData & 0x0000FF00 ) >> BITS_IN_BYTE , BYTE_SIZE );
	//read in from the IX or IY
	if( ( (opcodeData >> BITS_IN_WORD) & IY_MASK ) == IY_MASK ){
		waveformFSM->preformMemoryOperation( z_80Registers.get16b( iyReg ) + d, false );
	}else{
		waveformFSM->preformMemoryOperation( z_80Registers.get16b( ixReg ) + d, false );
	}
	nextPart = &InstructionManager::ROTATE_IIXYpDI_2;
}
void InstructionManager::ROTATE_IIXYpDI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int rotationAction = extractField( opcodeData, ROTATER_LOCATION, ROTATER_LENGTH );
	int oldValue = newData;
	int d = tempData;
	int newValue = rotateize( oldValue, rotationAction );
		
	//now write it back
	if( ( (opcodeData >> BITS_IN_WORD) & IY_MASK ) == IY_MASK ){
		waveformFSM->preformMemoryOperation( z_80Registers.get16b( iyReg ) + d, true, newValue );
	}else{
		waveformFSM->preformMemoryOperation( z_80Registers.get16b( ixReg ) + d, true, newValue );
	}
	
	ostringstream oppText;
	ostringstream description;
	string ixypd = printIXYpd( d, true );
	oppText << decodeRotationName( rotationAction ) << " " << ixypd;
	description << "Referencing the value in memory pointed to by " << ixypd << " " << decodeRotationDescription( rotationAction );
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	//the flags are set in rotateize
	nextPart = &InstructionManager::InstructionFetch;
}


//RLD
void InstructionManager::RLD( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//read in what is in hl
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( hlReg ), false );	
	nextPart = &InstructionManager::RLD_2;
}
void InstructionManager::RLD_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int aValue = z_80Registers.get8b( aReg );
	int IHLIValue = newData;
	int aNewValue = (aValue & 0xF0) | ((IHLIValue & 0xF0) >> 4 );
	int IHLINewValue = ((IHLIValue & 0x0F) << 4) | (aValue & 0x0F);
	z_80Registers.set8b( aReg, aNewValue );
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( hlReg ), true, IHLINewValue );
	
	//acording to http://www.geocities.com/SiliconValley/Peaks/3938/z80_faq.htm#Q-23
	//the parity is set by the new value of the a register and not the value that is going
	//out to the hl referneced memory location
	z_80Registers.setFlags( aNewValue, BYTE_SIZE, aValue, IHLIValue, false, false, TEST, TEST, RESET, TEST, NOTUSED, RESET, HOLD );
	
	describeInstruction( "RLD", "Moves the A's least significant nibble which replaces the least significant nibble pointed to by HL which replaces the most significant nibble pointed to by HL which replaces A's least significant nibble", z_80LogicGate );
	
	nextPart = &InstructionManager::InstructionFetch;
}
	
	
 
//RRD
void InstructionManager::RRD( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//read in what is in hl
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( hlReg ), false );
	nextPart = &InstructionManager::RRD_2;
}
void InstructionManager::RRD_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int aValue = z_80Registers.get8b( aReg );
	int IHLIValue = newData;
	int aNewValue = (aValue & 0xF0) | ( IHLIValue & 0x0F );
	int IHLINewValue = ((aValue & 0x0F) << 4) | (( IHLIValue & 0xF0) >> 4 );
	z_80Registers.set8b( aReg, aNewValue );
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( hlReg ), true, IHLINewValue );
	
	//acording to http://www.geocities.com/SiliconValley/Peaks/3938/z80_faq.htm#Q-23
	//the parity is set by the new value of the a register and not the value that is going
	//out to the hl referneced memory location
	z_80Registers.setFlags( aNewValue, BYTE_SIZE, aValue, IHLIValue, false, false, TEST, TEST, RESET, TEST, NOTUSED, RESET, HOLD );
	
	describeInstruction( "RLD", "Moves the A's least significant nibble which replaces the most significant nibble pointed to by HL which replaces the least significant nibble pointed to by HL which replaces A's least significant nibble", z_80LogicGate );
	
	nextPart = &InstructionManager::InstructionFetch;
}

//Bit Set, Reset and Test Group**************************
//This function is used by the below bit functions
//to do the testing of the bit and then setting the flags
void InstructionManager::BIT_TESTER( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int testValue = newData;
	int b = extractField( opcodeData, BITS_LENGTH, BITS_LOCATION );
	bool bitZero = (extractField( testValue, b, 1 ) == 0);
	z_80Registers.setFlags( 0, BYTE_SIZE, 0, 0, false, false, DNTCARE, bitZero?SET:RESET, SET, DNTCARE, NOTUSED, RESET, HOLD );
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

//This determinds the ix+d or iy+d value by examining the current opcode
int InstructionManager::getIXYpDAddress(){
	int baseAddress = 0;
	
	//chop off postFix
	int prefix = opcodeData >> (BITS_IN_BYTE*2);
	
	//see if we have an ix or an iy offset
	if( (prefix & IY_MASK) == IY_MASK){
		baseAddress = z_80Registers.get16b( iyReg );
	}else{
		baseAddress = z_80Registers.get16b( ixReg );
	}
	
	//compute d
	int d = (opcodeData & 0x0000FF00) >> BITS_IN_BYTE;
	d = z_80Registers.getSignExtend( d, BYTE_SIZE );
	
	//add on d and return
	return baseAddress + d;
}

//BIT b,r
void InstructionManager::BIT_b_r( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	RSinglet reg = translateReg8b( extractField( opcodeData, BIT_B_R_R_LOCATION, SINGLE_BIT_LENGTH ) );
	
	ostringstream oppText;
	int b = extractField( opcodeData, BITS_LENGTH, BITS_LOCATION );
	oppText << "BIT " << b << "," << z_80Registers.get8bName( reg );
	ostringstream description;
	description << "Tests to see if bit " << b << " in register " << z_80Registers.get8bName( reg ) << " is set or reset.  If set the zero flag (Z) is set, if reset, reset";
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	BIT_TESTER( z_80LogicGate, waveformFSM, z_80Registers.get8b( reg ) );
}

//BIT b,(HL)
void InstructionManager::BIT_b_IHLI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( hlReg ), false );
	
	ostringstream oppText;
	int b = extractField( opcodeData, BITS_LENGTH, BITS_LOCATION );
	oppText << "BIT " << b << ",(HL)";
	ostringstream description;
	description << "Tests to see if bit " << b << " is set or reset at the memory address pointed to by HL.  If set the zero flag (Z) is set, if reset, reset";
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	nextPart = &InstructionManager::BIT_TESTER;
}

//BIT b,(IX+d)
//BIT b,(IY+d)
void InstructionManager::BIT_B_IIXYpDI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	waveformFSM->preformMemoryOperation( getIXYpDAddress(), false );
	
	ostringstream oppText;
	int b = extractField( opcodeData, BITS_LENGTH, BITS_LOCATION );
	int d = (opcodeData & 0x0000FF00) >> BITS_IN_BYTE;
	d = z_80Registers.getSignExtend( d, BYTE_SIZE );
	oppText << "BIT " << b << "," << printIXYpd( d, true );
	ostringstream description;
	description << "Tests to see if bit " << b << " is set or reset at the referenced memory address.  If set the zero flag (Z) is set, if reset, reset";
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	nextPart = &InstructionManager::BIT_TESTER;
}

int InstructionManager::changeBitTo( int newBitValue, int oldValue, int bitPos ){
	int returnValue = 0;
	if( newBitValue == 1 ){
		returnValue = putPartIn( oldValue, 1, bitPos );
	}else{
		returnValue = z_80Registers.getNigation( oldValue, BYTE_SIZE );
		returnValue = putPartIn( returnValue, 1, bitPos );
		returnValue = z_80Registers.getNigation( returnValue, BYTE_SIZE );
	}
	return returnValue;
}

//SET b,r
//RES b,r
void InstructionManager::SETRES_B_R( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	RSinglet reg = translateReg8b( extractField( opcodeData, SETRES_B_R_R_LOCATION, SINGLE_BIT_LENGTH ) );
	int b = extractField( opcodeData, BITS_LENGTH, BITS_LOCATION );
	int newBitValue = ((opcodeData & SET_MASK) == SET_MASK)?1:0;
	int newValue = changeBitTo( newBitValue, z_80Registers.get8b( reg ), b );
	z_80Registers.set8b( reg, newValue );
	
	ostringstream oppText;
	string regName = z_80Registers.get8bName( reg );
	oppText << (((opcodeData & SET_MASK) == SET_MASK)?"SET ":"RES ") << b << "," << regName;
	ostringstream description;
	description << "Changes the bit at location " << b << " in register " << regName << " to a " << (((opcodeData & SET_MASK) == SET_MASK)?"one":"zero");
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
	//no flags change on a set or reset
}	
	
//This function will do a bit set or reset on the value
//passed in newData, and then will save it back to
//the address saved in tempData;
void InstructionManager::SETRES_B_IXXI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int address = tempData;
	int oldValue = newData;
	int b = tempData2 = extractField( opcodeData, BITS_LENGTH, BITS_LOCATION );
	
	//determine new bit
	int newBitValue = ((opcodeData & SET_MASK) == SET_MASK)?1:0;
	
	//combine with oldValue
	int newValue = changeBitTo( newBitValue, oldValue, b );
	
	//now time to save it back
	waveformFSM->preformMemoryOperation( address, true, newValue );
	nextPart = &InstructionManager::InstructionFetch;
}	

//SET b,(HL)  
//RES b,(HL)
void InstructionManager::SETRES_B_IHLI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int b = tempData2;
	int address = tempData = z_80Registers.get16b( hlReg );
	waveformFSM->preformMemoryOperation( address, false );
	
	ostringstream oppText;
	oppText << (((opcodeData & SET_MASK) == SET_MASK)?"SET ":"RES ") << b << ",(HL)";
	ostringstream description;
	description << "Changes the bit at location " << b << " in the memory pointed to by HL to a " << (((opcodeData & SET_MASK) == SET_MASK)?"one":"zero");
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	nextPart = &InstructionManager::SETRES_B_IXXI;
}


//SET b,(IX+d)
//SET b,(IY+d)
//RES b,(IX+d)
//RES b,(IY+d)
void InstructionManager::SETRES_B_IIXYpDI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int address = tempData = getIXYpDAddress();
	waveformFSM->preformMemoryOperation( address, false );
	
	ostringstream oppText;
	int d = (opcodeData & 0x0000FF00) >> BITS_IN_BYTE;
	int b = extractField( opcodeData, BITS_LENGTH, BITS_LOCATION );
	oppText << (((opcodeData & SET_MASK) == SET_MASK)?"SET ":"RES ") << b << "," << printIXYpd( d, true );
	ostringstream description;
	description << "Changes the bit at location " << b << " in the memory pointed to by HL to a " << (((opcodeData & SET_MASK) == SET_MASK)?"one":"zero");
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	nextPart = &InstructionManager::SETRES_B_IXXI;
}

//returns (IX+d) or (IY+d) formated as a string with d as a number
//is used by functions to print their description
string InstructionManager::printIXYpd( int d, bool hasPostFix ){
	ostringstream oppText;
	int withOutPostFix = opcodeData;
	if( hasPostFix ){
		withOutPostFix >>= (BITS_IN_BYTE*2);
	}
	oppText << "(";
	if( (withOutPostFix & IY_MASK) == IY_MASK ){
		oppText << "IY";
	}else{
		oppText << "IX";
	}
	if( d > 0 ){
		oppText << "+";
	}
	oppText << d << ")";
	
	return oppText.str();
}

//Jump Group*********************************************

//This returns a string representation of a condition
//code
string InstructionManager::conditionToString( int condition ){
	switch( condition ){
		case CC_NZ: return "NZ";
		case CC_Z:  return "Z";
		case CC_NC: return "NC";
		case CC_C:  return "C";
		case CC_PO: return "PO";
		case CC_PE: return "PE";
		case CC_P:  return "P";
		case CC_M:  return "M";
		default:    return "oops";
	}
}
//This returns a string containing a description of a condition
string InstructionManager::conditionToDescription( int condition ){
	switch( condition ){
		case CC_NZ: return "last result was non zero";
		case CC_Z:  return "last result was zero";
		case CC_NC: return "last result did not result resulted in a carry or barow";
		case CC_C:  return "last result resulted in a carry or barrow";
		case CC_PO: return "last result had an odd parity";
		case CC_PE: return "last result had an even parity";
		case CC_P:  return "last result was positive";
		case CC_M:  return "last result was negative";
		default:    return "oops";
	}
}

//JP nn
void InstructionManager::JP_nn( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//first we need to load the two n's
	FetchOpcode( waveformFSM );
	nextPart = &InstructionManager::JP_nn_2;
}
void InstructionManager::JP_nn_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData = newData;
	FetchOpcode( waveformFSM );
	nextPart = &InstructionManager::JP_nn_3;
}
void InstructionManager::JP_nn_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int jumpTarget = ((newData << BITS_IN_BYTE) | tempData );
	z_80Registers.set16b( pcReg, jumpTarget );
	
	describeInstruction( "JP nn", "Jumps to the absolute address nn without affecting the stack", jumpTarget, z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}


//JP cc,nn
void InstructionManager::JP_cc_nn( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//first we need to load the two n's
	FetchOpcode( waveformFSM );
	nextPart = &InstructionManager::JP_cc_nn_2;
}
void InstructionManager::JP_cc_nn_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData = newData;
	FetchOpcode( waveformFSM );
	nextPart = &InstructionManager::JP_cc_nn_3;
}
void InstructionManager::JP_cc_nn_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int jumpTarget = ((newData << BITS_IN_BYTE) | tempData );
	//test condition
	if( testCondition( extractField( opcodeData, CC_LOCATION, CC_LENGTH ) ) ){
		z_80Registers.set16b( pcReg, jumpTarget );
	}
	
	ostringstream oppText;
	ostringstream description;
	int conditionCode = extractField( opcodeData, CC_LOCATION, CC_LENGTH );
	oppText << "JP " << conditionToString( conditionCode ) << ",nn";
	description << "Jumps to nn if the " << conditionToDescription( conditionCode );
	describeInstruction( oppText.str(), description.str(), jumpTarget, z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

//JR e
void InstructionManager::JR_e( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//need to get the e
	FetchOpcode( waveformFSM );
	nextPart = &InstructionManager::JR_e_2;
}
void InstructionManager::JR_e_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int e = newData;
	z_80Registers.jr( e );
	
	ostringstream description;
	description << "Jumps to the address computed by adding e, that is n to " << z_80Registers.get16b( pcReg ) << ", the current PC.  Does not effect the stack";
	describeInstruction( "JR e", description.str(), e, z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}	

//JR C,e
void InstructionManager::JR_C_e( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//need to get the e
	FetchOpcode( waveformFSM );
	
	nextPart = &InstructionManager::JR_C_e_2;
}
void InstructionManager::JR_C_e_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int e = newData;
	if( (z_80Registers.get8b( fReg ) & C_FLAG) == C_FLAG ){
		z_80Registers.jr( e );
	}
	
	describeInstruction( "JR C,e", "Jumps to the relative address computed by adding e, that is n, to the PC if the carry flag is set", e, z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

//JR NC,e
void InstructionManager::JR_NC_e( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//need to get the e
	FetchOpcode( waveformFSM );
	nextPart = &InstructionManager::JR_NC_e_2;
}
void InstructionManager::JR_NC_e_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int e = newData;
	if( (z_80Registers.get8b( fReg ) & C_FLAG) != C_FLAG ){
		z_80Registers.jr( e );
	}
	
	describeInstruction( "JR NC,e", "Jumps to the relative address computed by adding e, that is n, to the PC if the carry flag is not set", e, z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}
	
//JR Z,e
void InstructionManager::JR_Z_e( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//need to get the e
	FetchOpcode( waveformFSM );
	nextPart = &InstructionManager::JR_Z_e_2;
}
void InstructionManager::JR_Z_e_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int e = newData;
	if( (z_80Registers.get8b( fReg ) & Z_FLAG) == Z_FLAG ){
		z_80Registers.jr( e );
	}
	
	describeInstruction( "JR Z,e", "Jumps to the relative address computed by adding e, that is n, to the PC if the zero flag is set", e, z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}


//JR NZ,e
void InstructionManager::JR_NZ_e( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//need to get the e
	FetchOpcode( waveformFSM );
	nextPart = &InstructionManager::JR_NZ_e_2;
}
void InstructionManager::JR_NZ_e_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int e = newData;
	if( (z_80Registers.get8b( fReg ) & Z_FLAG) != Z_FLAG ){
		z_80Registers.jr( e );
	}
	
	describeInstruction( "JR NZ,e", "Jumps to the relative address computed by adding e, that is n, to the PC if the zero flag is not set", e, z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}


//JP (HL)
void InstructionManager::JP_IHLI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	z_80Registers.set16b( pcReg, z_80Registers.get16b( hlReg ) );
	
	describeInstruction( "JP (HL)", "Loads the PC with HL", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}
	
//JP (IX)
//JP (IY)
void InstructionManager::JP_IIXYI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//need to load value indexed by HL
	RDoublet addresser = ((opcodeData & IY_MASK)==IY_MASK) ? iyReg: ixReg;
	z_80Registers.set16b( pcReg, z_80Registers.get16b( addresser ) );
	
	ostringstream oppText;
	string addresserName = z_80Registers.get16bName( addresser );
	oppText << "JP (" << addresserName << ")";
	ostringstream description;
	description << "Loads the PC with " << addresserName;
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}

//DJNZ, e
void InstructionManager::DJNZ_e( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//need to get the e
	FetchOpcode( waveformFSM );
	nextPart = &InstructionManager::DJNZ_e_2;
}
void InstructionManager::DJNZ_e_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int e = newData;
	int bNewValue = z_80Registers.get8b( bReg ) - 1;
	
	z_80Registers.set8b( bReg, bNewValue );
	
	if( bNewValue != 0 ){
		z_80Registers.jr( e );
	}
	
	describeInstruction( "DJNZ, e", "Decrements the B register and then jumps to e + PC if B is not zero", e, z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData );//next instruction
}	
		

//Call and Return Group**********************************

bool InstructionManager::testCondition( int conditionToTest ){
	bool willCall = false;
	int fValue = z_80Registers.get8b( fReg );
	
	//now we decide if we are going to call or not
	switch( conditionToTest ){
		case CC_NZ:
			willCall = ( fValue & Z_FLAG ) != Z_FLAG;
			break;
		case CC_Z:
			willCall = ( fValue & Z_FLAG ) == Z_FLAG;
			break;
		case CC_NC:
			willCall = ( fValue & C_FLAG ) != C_FLAG;
			break;
		case CC_C:
			willCall = ( fValue & C_FLAG ) == C_FLAG;
			break;
		case CC_PO:
			willCall = ( fValue & P_V_FLAG ) != P_V_FLAG;
			break;
		case CC_PE:
			willCall = ( fValue & P_V_FLAG ) == P_V_FLAG;
			break;
		case CC_P:
			willCall = ( fValue & S_FLAG ) != S_FLAG;
			break;
		case CC_M:
			willCall = ( fValue & S_FLAG ) == S_FLAG;
			break;
		case CC_T:
		    //I made this one up...
			willCall = true;
			break;
	}
	return willCall;
}

//This function will call depending on the CC code in
//the tempData reg.
void InstructionManager::CALL_XX_nn( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//our first job will be to pull in the nns
	FetchOpcode( waveformFSM );
	nextPart = &InstructionManager::CALL_XX_nn_2;
}
void InstructionManager::CALL_XX_nn_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData2 = newData;
	FetchOpcode( waveformFSM );
	nextPart = &InstructionManager::CALL_XX_nn_3;
}
void InstructionManager::CALL_XX_nn_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int cc = tempData;

	int nn = tempData2 = (newData << BITS_IN_BYTE) | tempData2;
	
	ostringstream oppText;
	ostringstream description;
	if( cc != CC_T ){
		oppText << "CALL " << conditionToString( cc ) << ",nn";
	}else{
		oppText << "CALL nn";
	}
	description << "Will push the current PC to the stack load PC with nn if the " << conditionToDescription( cc );
	describeInstruction( oppText.str(), description.str(), nn, z_80LogicGate );

	//now we decide if we are going to call or not	
	if( testCondition( cc ) ){		
		//if we are going to call we need to push the pc onto the stack
		int spValue = z_80Registers.get16b( spReg );
		int pcValue = z_80Registers.get16b( pcReg );
		waveformFSM->preformMemoryOperation( spValue - 1, true, ((pcValue & 0xFF00) >> BITS_IN_BYTE) );
		
		nextPart = &InstructionManager::CALL_XX_nn_4;
	}else{
		InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next Instruction
	}
}
void InstructionManager::CALL_XX_nn_4( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int nn = tempData2;
	int spValue = z_80Registers.get16b( spReg );
	int pcValue = z_80Registers.get16b( pcReg );
	waveformFSM->preformMemoryOperation( spValue - 2, true, (pcValue & 0x00FF) );
	
	z_80Registers.set16b( spReg, spValue - 2 );
	z_80Registers.set16b( pcReg, nn );
	//doesn't affect flags
	
	nextPart = &InstructionManager::InstructionFetch;
}


//CALL nn
void InstructionManager::CALL_nn( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData = CC_T;
	CALL_XX_nn( z_80LogicGate, waveformFSM, newData );
}

//CALL cc,nn
void InstructionManager::CALL_cc_nn( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//extract cc
	tempData = extractField( opcodeData, CC_LOCATION, CC_LENGTH );
	CALL_XX_nn( z_80LogicGate, waveformFSM, newData );
}

//RET
void InstructionManager::RET( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//first we pop the return address off of the stack
	int spValue = tempData2 = z_80Registers.get16b( spReg );
	
	waveformFSM->preformMemoryOperation( spValue, false );
	
	nextPart = &InstructionManager::RET_2;
}
void InstructionManager::RET_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int spValue = tempData2;
	tempData2 = newData;
	waveformFSM->preformMemoryOperation( spValue + 1, false );
	nextPart = &InstructionManager::RET_3;
}
void InstructionManager::RET_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//little endian
	int newPc = (newData << BITS_IN_BYTE) | tempData2;
	int spValue = z_80Registers.get16b( spReg );
	z_80Registers.set16b( spReg, spValue + 2 );
	z_80Registers.set16b( pcReg, newPc );
	
	//we use RET to finnish off some other instructions.
	//This if keeps us from overwriting their descriptions
	//of themselves.
	if( opcodeData == RET_BASE_OPP ){
		describeInstruction( "RET", "Returns from a regular function by popping the stack to the PC", z_80LogicGate );
	}
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next instruction
}

//RET cc
void InstructionManager::RET_cc( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	if( testCondition( extractField( opcodeData, CC_LOCATION, CC_LENGTH ) ) ){
		RET( z_80LogicGate, waveformFSM, newData );
	}else{
		InstructionFetch( z_80LogicGate, waveformFSM, newData );
	}
	
	ostringstream oppText;
	ostringstream description;
	int conditionCode = extractField( opcodeData, CC_LOCATION, CC_LENGTH );
	oppText << "RET " << conditionToString( conditionCode );
	description << "Returns from a function by popping a the stack to the PC if the " << conditionToDescription( conditionCode );
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
}

//RETI
//RETN
void InstructionManager::RETIN( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	if( (opcodeData & I_MASK) != I_MASK ){
		z_80Registers.setIFF1( z_80Registers.getIFF2() );
	}
	
	RET( z_80LogicGate, waveformFSM, newData );
	
	
	if( (opcodeData & I_MASK) != I_MASK ){
		describeInstruction( "RETI", "Returns from a mask able interrupt service routine.  This opp code is sniffed off the bus by Z-80 peripherals so that they know to reset", z_80LogicGate );
	}else{
		describeInstruction( "RETN", "Returns from a non-mask able interrupt service routine.  This renables regular interrupts if they had been enabled before the non-mask able interrupt happened", z_80LogicGate );
	}
	//RET above takes care of FetchInstruction
}

//RST p
void InstructionManager::RST_p( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int pCode = extractField( opcodeData, P_LOCATION, P_LENGTH );
	
	//we will store our target destination in tempData2;
	switch( pCode ){
		case P_00H: tempData2 = 0x00; break;
		case P_08H: tempData2 = 0x08; break;
		case P_10H: tempData2 = 0x10; break;
		case P_18H: tempData2 = 0x18; break;
		case P_20H: tempData2 = 0x20; break;
		case P_28H: tempData2 = 0x28; break;
		case P_30H: tempData2 = 0x30; break;
		case P_38H: tempData2 = 0x38; break;
	}
	
	// we need to push the pc onto the stack
	int spValue = z_80Registers.get16b( spReg );
	int pcValue = z_80Registers.get16b( pcReg );
	waveformFSM->preformMemoryOperation( spValue - 1, true, ((pcValue & 0xFF00) >> BITS_IN_BYTE) );
	
	nextPart = &InstructionManager::RST_p_2;
}
void InstructionManager::RST_p_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int nn = tempData2;
	int spValue = z_80Registers.get16b( spReg );
	int pcValue = z_80Registers.get16b( pcReg );
	waveformFSM->preformMemoryOperation( spValue - 2, true, (pcValue & 0x00FF) );
	
	z_80Registers.set16b( spReg, spValue - 2 );
	z_80Registers.set16b( pcReg, nn );
	//doesn't affect flags
	
	ostringstream oppText;
	oppText << "RST " << hex << uppercase << nn << "H";
	describeInstruction( oppText.str(), "Calls to the specified address.  Is normally supplied by interrupting IO devices in conjunction with IO mode zero", z_80LogicGate );
	
	nextPart = &InstructionManager::InstructionFetch;
}

//Input and Output Group*********************************
//IN A,(n)
void InstructionManager::IN_A_INI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//first we need to read in the n
	FetchOpcode(waveformFSM);
	nextPart = &InstructionManager::IN_A_INI_2;
}
void InstructionManager::IN_A_INI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int n = tempData = newData;
	waveformFSM->preformIOInput( n );
	nextPart = &InstructionManager::IN_A_INI_3;
}
void InstructionManager::IN_A_INI_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int input = newData;
	z_80Registers.set8b( aReg, input );
	
	int n = tempData;
	describeInstruction( "IN A,(n)", "Reads from input port n, and places the data in register A", n, z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next instruction
	//all flags hold
}


//IN F,(C)
void InstructionManager::IN_F_ICI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	waveformFSM->preformIOInput( z_80Registers.get16b( bcReg ) );
	nextPart = &InstructionManager::IN_F_ICI_2;
}
void InstructionManager::IN_F_ICI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int input = newData;
	//all we do with IN F,(C) is set the flags
	//how do we test the H flag when we haven't added or subtracted???
	z_80Registers.setFlags( input, BYTE_SIZE, input, 0, false, false, TEST, TEST, TEST, TEST, NOTUSED, RESET, HOLD );
	
	describeInstruction( "IN F,(C)", "Fakes a read from the IO port referenced by register C and sets only the flags", z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next instruction
}

//IN r,(C)
void InstructionManager::IN_R_ICI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	                           //asserting bc instead of just c is an extension that most manufactured z80 do.
	waveformFSM->preformIOInput( z_80Registers.get16b( bcReg ) );
	nextPart = &InstructionManager::IN_R_ICI_2;
}
void InstructionManager::IN_R_ICI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int input = newData;
	
	z_80Registers.set8b( translateReg8b( extractField( opcodeData, IN_R_ICI_R_LOCATION, SINGLE_BIT_LENGTH ) ), input );
	
	z_80Registers.setFlags( input, BYTE_SIZE, input, 0, false, false, TEST, TEST, TEST, TEST, NOTUSED, RESET, HOLD );
	
	ostringstream oppText;
	string regName = z_80Registers.get8bName( translateReg8b( extractField( opcodeData, IN_R_ICI_R_LOCATION, SINGLE_BIT_LENGTH ) ) );
	oppText << "IN " << regName << ",(C)";
	ostringstream description;
	description << "Reads in from the port specified by C and places the result in register " << regName;
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next instruction
}

//INI
void InstructionManager::INI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//read in from (C) //really (BC)
	waveformFSM->preformIOInput( z_80Registers.get16b( bcReg ) );
	nextPart = &InstructionManager::INI_2;
}
void InstructionManager::INI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//and send it out to (HL)
	int input = newData;
	int hlValue = z_80Registers.get16b( hlReg );
	int bNewValue = z_80Registers.get8b( bReg ) - 1;
	waveformFSM->preformMemoryOperation( hlValue, true, input );
	z_80Registers.set8b( bReg, bNewValue );
	z_80Registers.set16b( hlReg, hlValue + 1 );
	
	//because I didn't know how to set the N flag, I went searching and found this page
	//http://www.z80.info/z80undoc3.txt
	//it has the following rules for setting the flags
	
	//"""""
	//INI/INIR/IND/INDR     SZ5*3***  Flags affected as in DEC B
	//OUTI/OTIR/OUTD/OTDR   SZ5*3***  Flags affected as in DEC B

	//Another weird one. S,Z,5,3 are affected by the decrease of B, like in DEC B.
	//The N flag is a copy of bit 7 of the last value read from/written too the
	//I/O port. The C and H flag is set as follows: Take register C, add one to it
	//if the instruction increases HL otherwise decrease it by one. Now, add the
	//the value of the I/O port (read or written) to it, and the carry of this
	//last addition is copied to the C and H flag (so C and H flag are the same). 
	//Beats me, but tests show it to be true. 
	//"""""
	
	//I think I will only set the N flag
	bool signBitSet = ((input & SIGN_8BIT_MASK) == SIGN_8BIT_MASK);
	z_80Registers.setFlags( input, BYTE_SIZE, input, 0, false, false, DNTCARE, (bNewValue==0)?SET:RESET, DNTCARE, DNTCARE, NOTUSED, signBitSet?SET:RESET, DNTCARE );

	describeInstruction( "INI", "Loads the memory location pointed to by HL with the port referenced by C, decrements B and increments HL", z_80LogicGate );

	nextPart = &InstructionManager::InstructionFetch;
}
	
//INIR
void InstructionManager::INIR( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//we init the tempData so that if B is zero to start with, we will set the N flag to 0
	//and not the same thing every time.
	tempData = 0;
	INIR_2( z_80LogicGate, waveformFSM, newData );
}
void InstructionManager::INIR_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	if( z_80Registers.get8b( bReg ) != 0 ){
		waveformFSM->preformIOInput( z_80Registers.get16b( bcReg ) );
		nextPart = &InstructionManager::INIR_3;
	}else{
		int input = tempData;
		bool signBitSet = (input & SIGN_8BIT_MASK) == SIGN_8BIT_MASK;
		z_80Registers.setFlags( input, BYTE_SIZE, input, 0, false, false, DNTCARE, SET, DNTCARE, DNTCARE, NOTUSED, signBitSet?SET:RESET, DNTCARE );
		
		describeInstruction( "INIR", "Loads the memory location pointed to by HL with the port referenced by C, decrements B, increments HL and repeats until B equals zero", z_80LogicGate );
		
		InstructionFetch( z_80LogicGate, waveformFSM, newData );
	}
}
void InstructionManager::INIR_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int input = tempData = newData;
	int hlValue = z_80Registers.get16b( hlReg );
	waveformFSM->preformMemoryOperation( hlValue, true, input );
	
	z_80Registers.set8b( bReg, z_80Registers.get8b( bReg ) - 1 );
	z_80Registers.set16b( hlReg, hlValue + 1 );
	
	nextPart = &InstructionManager::INIR_2; //loop up
}

//IND
void InstructionManager::IND( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//read in from (C) //really (BC)
	waveformFSM->preformIOInput( z_80Registers.get16b( bcReg ) );
	nextPart = &InstructionManager::IND_2;
}
void InstructionManager::IND_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//and send it out to (HL)
	int input = newData;
	int hlValue = z_80Registers.get16b( hlReg );
	int bNewValue = z_80Registers.get8b( bReg ) - 1;
	bool signBitSet = ((input & SIGN_8BIT_MASK) == SIGN_8BIT_MASK);
	waveformFSM->preformMemoryOperation( hlValue, true, input );
	z_80Registers.set8b( bReg, bNewValue );
	z_80Registers.set16b( hlReg, hlValue - 1 );
	z_80Registers.setFlags( input, BYTE_SIZE, input, 0, false, false, DNTCARE, (bNewValue==0)?SET:RESET, DNTCARE, DNTCARE, NOTUSED, signBitSet?SET:RESET, DNTCARE );

	describeInstruction( "IND", "Loads the memory location pointed to by HL with the port referenced by C and decrements B and HL", z_80LogicGate );

	nextPart = &InstructionManager::InstructionFetch;
}

//INDR
void InstructionManager::INDR( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//we init the tempData so that if B is zero to start with, we will set the N flag to 0
	//and not the same thing every time.
	tempData = 0;
	INDR_2( z_80LogicGate, waveformFSM, newData );
}
void InstructionManager::INDR_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	if( z_80Registers.get8b( bReg ) != 0 ){
		waveformFSM->preformIOInput( z_80Registers.get16b( bcReg ) );
		nextPart = &InstructionManager::INDR_3;
	}else{
		int input = tempData;
		bool signBitSet = (input & SIGN_8BIT_MASK) == SIGN_8BIT_MASK;
		z_80Registers.setFlags( input, BYTE_SIZE, input, 0, false, false, DNTCARE, SET, DNTCARE, DNTCARE, NOTUSED, signBitSet?SET:RESET, DNTCARE );
		
		describeInstruction( "INDR", "Loads the memory location pointed to by HL with the port referenced by C, decrements B and HL and repeats until B equals zero", z_80LogicGate );
		
		InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next instruction
	}
}
void InstructionManager::INDR_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int input = tempData = newData;
	int hlValue = z_80Registers.get16b( hlReg );
	waveformFSM->preformMemoryOperation( hlValue, true, input );
	
	z_80Registers.set8b( bReg, z_80Registers.get8b( bReg ) - 1 );
	z_80Registers.set16b( hlReg, hlValue - 1 );
	
	nextPart = &InstructionManager::INDR_2; //loop up
}

//OUT (n),A
void InstructionManager::OUT_INI_A( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//first we need to read in the n
	FetchOpcode(waveformFSM);
	nextPart = &InstructionManager::OUT_INI_A_2;
}
void InstructionManager::OUT_INI_A_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int n = newData;
	int aValue = z_80Registers.get8b( aReg );
	waveformFSM->preformIOOutput( n, aValue );
	bool signBitWasSet = ((aValue & SIGN_8BIT_MASK) == SIGN_8BIT_MASK);
	z_80Registers.setFlags( aValue, BYTE_SIZE, aValue, 0, false, false, HOLD, HOLD, HOLD, HOLD, NOTUSED, signBitWasSet?SET:RESET, DNTCARE );
	
	describeInstruction( "OUT (n),A", "Outputs to port n the value held in register A", n, z_80LogicGate );
	
	nextPart = &InstructionManager::InstructionFetch;
}

//OUT (C),r
void InstructionManager::OUT_ICI_R( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int regValue = z_80Registers.get8b( translateReg8b( extractField( opcodeData, OUT_ICI_R_R_LOCATION, SINGLE_BIT_LENGTH ) ) );
	bool signBitWasSet = ((regValue & SIGN_8BIT_MASK) == SIGN_8BIT_MASK);
	waveformFSM->preformIOOutput( z_80Registers.get16b( bcReg ), regValue );
	z_80Registers.setFlags( regValue, BYTE_SIZE, regValue, 0, false, false, HOLD, HOLD, HOLD, HOLD, NOTUSED, signBitWasSet?SET:RESET, DNTCARE );
	
	ostringstream oppText;
	string regName = z_80Registers.get8bName( translateReg8b( extractField( opcodeData, OUT_ICI_R_R_LOCATION, SINGLE_BIT_LENGTH ) ) );
	oppText << "OUT (C)," << regName;
	ostringstream description;
	description << "Outputs to the port specified by register C the value held in register " << regName;
	describeInstruction( oppText.str(), description.str(), z_80LogicGate );
	
	nextPart = &InstructionManager::InstructionFetch;
}
	
//OUTI
void InstructionManager::OUTI( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	//first we must read in from the HL register
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( hlReg ), false );
	nextPart = &InstructionManager::OUTI_2;
}
void InstructionManager::OUTI_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int hlReferencedValue = newData;
	waveformFSM->preformIOOutput( z_80Registers.get16b( bcReg ), hlReferencedValue );
	bool signBitWasSet = ((hlReferencedValue & SIGN_8BIT_MASK) == SIGN_8BIT_MASK);
	int newBValue = z_80Registers.get8b( bReg ) - 1;
	z_80Registers.set8b( bReg, newBValue );
	z_80Registers.set16b( hlReg, z_80Registers.get16b( hlReg ) + 1 );
	z_80Registers.setFlags( hlReferencedValue, BYTE_SIZE, hlReferencedValue, 0, false, false, DNTCARE, (newBValue==0)?SET:RESET, DNTCARE, NOTUSED, DNTCARE, signBitWasSet?SET:RESET, DNTCARE );
	
	describeInstruction( "OUTI", "Outputs to the port specified by register C the memory value pointed to by register HL, decrements B and increments HL", z_80LogicGate );
	
	nextPart = &InstructionManager::InstructionFetch;
}

//OTIR
void InstructionManager::OTIR( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData = 0;
	OTIR_2( z_80LogicGate, waveformFSM, newData );
}
void InstructionManager::OTIR_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	if( z_80Registers.get8b( bReg ) != 0 ){
		waveformFSM->preformMemoryOperation( z_80Registers.get16b( hlReg ), false );
		nextPart = &InstructionManager::OTIR_3;
	}else{
		bool signBitWasSet = ((tempData & SIGN_8BIT_MASK)== SIGN_8BIT_MASK );
		z_80Registers.setFlags( tempData, BYTE_SIZE, tempData, 0, false, false, DNTCARE, SET, DNTCARE, DNTCARE, NOTUSED, signBitWasSet?SET:RESET, DNTCARE );
		
		describeInstruction( "OTIR", "Outputs to the port specified by register C the memory value pointed to by register HL, decrements B, increments HL and repeats until B equals zero", z_80LogicGate );
		
		InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next instruction
	}
}
void InstructionManager::OTIR_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int hlReferencedData = tempData = newData;
	waveformFSM->preformIOOutput( z_80Registers.get16b( bcReg ), hlReferencedData );
	z_80Registers.set8b( bReg, z_80Registers.get8b( bReg ) - 1 );
	z_80Registers.set16b( hlReg, z_80Registers.get16b( hlReg ) + 1 );
	nextPart = &InstructionManager::OTIR_2; //loop
}

//OUTD
void InstructionManager::OUTD( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	waveformFSM->preformMemoryOperation( z_80Registers.get16b( hlReg ), false );
	nextPart = &InstructionManager::OUTD_2;
}
void InstructionManager::OUTD_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int hlReferencedData = newData;
	waveformFSM->preformIOOutput( z_80Registers.get16b( bcReg ), hlReferencedData );
	z_80Registers.set8b( bReg, z_80Registers.get8b( bReg ) - 1 );
	z_80Registers.set16b( hlReg, z_80Registers.get16b( hlReg ) - 1 );
	
	describeInstruction( "OUTD", "Outputs to the port specified by register C the memory value pointed to by register HL and decrements B and HL", z_80LogicGate );
	
	nextPart = &InstructionManager::InstructionFetch;
}

//OTDR
void InstructionManager::OTDR( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	tempData = 0;
	OTDR_2( z_80LogicGate, waveformFSM, newData );
}
void InstructionManager::OTDR_2( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	if( z_80Registers.get8b( bReg ) != 0 ){
		waveformFSM->preformMemoryOperation( z_80Registers.get16b( hlReg ), false );
		nextPart = &InstructionManager::OTDR_3;
	}else{
		bool signBitWasSet = ((tempData & SIGN_8BIT_MASK)== SIGN_8BIT_MASK );
		z_80Registers.setFlags( tempData, BYTE_SIZE, tempData, 0, false, false, DNTCARE, SET, DNTCARE, DNTCARE, NOTUSED, signBitWasSet?SET:RESET, DNTCARE );

		describeInstruction( "OTDR", "Outputs to the port specified by register C the memory value pointed to by register HL, decrements B and HL and repeats until B equals zero", z_80LogicGate );

		InstructionFetch( z_80LogicGate, waveformFSM, newData ); //next instruction
	}
}
void InstructionManager::OTDR_3( Z_80LogicGate* z_80LogicGate, WaveformFSM* waveformFSM, int newData ){
	int hlReferencedData = tempData = newData;
	waveformFSM->preformIOOutput( z_80Registers.get16b( bcReg ), hlReferencedData );
	z_80Registers.set8b( bReg, z_80Registers.get8b( bReg ) - 1 );
	z_80Registers.set16b( hlReg, z_80Registers.get16b( hlReg ) - 1 );
	nextPart = &InstructionManager::OTDR_2; //loop
}
