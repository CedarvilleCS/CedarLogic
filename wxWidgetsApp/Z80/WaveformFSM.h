#ifndef WAVEFORMFSM_H_
#define WAVEFORMFSM_H_

#define RESET_ADDRESS 				0

#define CLK_CYCLE_TIME_1			1000
#define CLK_PULSE_WIDTH_HIGH_2 		470
#define CLK_PULSE_WIDTH_LOW_3		270
#define CLK_F_TIME_4				270
#define CLK_R_TIME_5				30

#define CLK_R_TO_ADDRESS_6	380
#define ADDRESS_TO_MREQ_F_7			370
#define CLK_F_TO_MREQ_8				260
#define CLK_R_TO_MREQ_9				260
#define MREQ_PULSE_WIDTH_HIGH_10	410

#define MREQ_PULSE_WIDTH_LOW_11		890
#define CLK_F_TO_MREQ_R_12			260
#define CLK_F_TO_RD_F_13			340
#define CLK_R_TO_RD_R_14			260
#define DATA_SETUP_TO_CLK_R_15		140

#define DATA_HOLD_TO_RD_R_16		0
#define WAIT_SETUP_TO_CLK_F_17		190
#define WAIT_HOLD_AFTER_CLK_F_18	0
#define CLK_R_TO_M1_F_19			340
#define CLK_R_TO_M1_R_20			340

#define CLK_R_TO_RFSH_F_21			460
#define CLK_R_TO_RFSH_R_22			390
#define CLK_F_TO_RD_R_23			290
#define CLK_R_TO_RD_F_24			260
#define DATA_SETUP_TO_CLK_F_25		160

#define ADDRESS_STBLE_BFOR_IORQ_F_26 790
#define CLK_R_TO_IORQ_F_27			240
#define CLK_F_TO_IORQ_R_28			290
#define DATA_STABLE_BFOR_WR_F_29	470
#define CLK_F_TO_WR_F_30			240

#define WR_PULSE_WIDTH_31			890
#define CLK_F_TO_WR_R_32			260
#define DATA_STABLE_BFOR_WR_F_33	30
#define CLK_R_TO_WR_F_34			210
#define DATA_STABLE_FROM_WR_R_35	290

#define CLK_F_TO_HALT_R_OR_F_36		760
#define NMI_PULSE_WIDTH_37			210
#define BUSREQ_SETUP_TO_CLK_R_38	210
#define BUSREQ_HOLD_AFTER_CLK_R_39	0
#define CLK_R_TO_BUSACK_F_40		310

#define CLK_F_TO_BUSACK_R_41		290
#define CLK_R_TO_DATA_FLOAT_42		240
#define CLK_R_TO_CONTROL_FLOAT_43	290
#define CLK_R_TO_ADDRESS_FLOAT_44	290
#define CONTROL_R_TO_ADDRESS_HOLD_45 400

#define RESET_TO_CLK_R_SETUP_46		240
#define RESET_TO_CLK_R_HOLD_47		0
#define INT_TO_CLK_R_SETUP_48		210
#define INT_TO_CLK_R_HOLD_49		0
#define M1_F_TO_IORQ_F_50			2300

#define CLK_F_TO_IORQ_F_51			290
#define CLK_R_TO_IORQ_R_52			260
#define CLK_F_TO_DATA_VALID_53		290

#define NUM_IO_WAIT_STATES			1
#define NUM_INT_WAIT_STATES			2

#define NICE_DELAY_AMOUNT			250


//TODO: need to implement listening to /BUSREQ

class Z_80LogicGate;

//The WaveformFSM handles the logic inputs that the
// Z_80LogicGate receive from the logic core.  
//It maintains an internal finite state machine 
//by keeping a pointer to the next function that 
//should be called on the rising edge of the next clock.  
//These functions represent the T states of the Z-80.  
//The WaveformFSM is responsible for generating the Z-80 
//wave forms that will be visible on the scope.
class WaveformFSM{
private:
	InstructionManager instructionManager;
	
	//When reseting, we check the pin on the rising edge,
	//but advance to the next state on the falling edge.
	//Thus we need this var to remember if we the reset
	//quit
	bool resetWasActive;
	
	//Even when starting a reset we check the pin on
	//the rising edge, but then must handle it on the
	//falling edge.  This bool lets us do that
	bool sawInitingReset;
	
	//when this is true, the waveformFSM will instruct the
	//Z_80LogicGate to signal that the simulation should
	//pause at the end of each instruction.
	bool tStepModeEnabled;
	
	//this boolean is set when by the z_80LogicGate when
	//it recieves an acychronouse NMI signal.
	//to it is set back to false by waveformFSM when
	//it has been acknolaged
	bool hasRecievedNMI;
	
	//this boolean set or reset at the resing clock of each operation
	//that the InstructionManager can request.  In actuallity
	//the INT should only be sampled on the last operation
	//that the InstructionManager orders, but the WaveformFSM
	//does not know which one that is.
	//The WaveformFSM then checks to see if this interupt
	//has accured or the NMI has accured at the end of each
	//instruction
	bool hasRecievedINT;
	
	//a halt is continued the same way as when it
	//is started to begin with.  Because of this
	//we need to know if we are continueing a halt
	//or if we are starting a halt to know if we need
	//to send the halt signal active.
	bool isInHalt;
	
	//Some waveforms such as the Interupt Acknowledge Cycle
	//and the Input or Output Cycle automaticaly
	//add one or two wait staits.  Therefore we must
	//be able to count them
	int waitStateCounter;
	
	//because a halt doesn't do anything,
	//I have compressed all the T states into a single
	//function.  To seperate the states from eachother
	//I have this integer.
	int haltTState;
	
	//For a reset to be effective, it must be held
	//high for three consecuative T states
	//This var is used to count thease states
	int resetCount;
	
	//private data that is used by variouse waveforms
	AddressType address;
	DatabusType data;
	bool isWrite;

	//this function pointer maintains the state of
	//this finite state machine
	void (WaveformFSM::*nextTState)( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
		
	//This gets called from the constructor and when the waveformFSM
	//gets reset in gateProcess
	void resetSignals( Z_80LogicGate* z_80LogicGate );
	
	//This is called by each state to update the user on what is going
	//on on that T-State
	void describeState( string name, int num, string description, Z_80LogicGate* z_80LogicGate );

	//This is the state that the waveformFSM is set to by the
	//constructor.  It starts everything off
    void bootState( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
	

	//instruction fetch waveform TStates
	//these functions are not called directly 
	//(with exception of the first) but are
	//called by using the nextTState function pointer
	//That pointer is called by nextState()
    void IFT1( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
    void IFT2( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
    void IFT3( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
    void IFT4( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
    
    //memory read or write TStates
    void MEMT1( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
    void MEMT2( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
    void MEMT3( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
    //the MEM waveform doesn't realy have a state four.  However
    //it needs to dispatch some events during the next waveform's
    //first cycle. The 4thish state will recall nextTState
    //to keep the next wavefrom from getting behind
    void MEMT4ish( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
    
	void IO1( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
	void IO2( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
	void IO3( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
    
	void INT1( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
	void INT2( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
	void INT3( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
	void INT4( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
	
	void NMI1( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
	void NMI2( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
	void NMI3( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
	void NMI4( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
	void NMI5( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
    
    //this is the waveform that we do when we are in a halt
    //this takes care of all 4 Tstates of one halt cycle
    void HALT1_4( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
    
    void RESET1( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
	void RESET2( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
	void RESET3( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
    

public:

	//the z_80LogicGate pointer is being passed in to the registers
	//so that they can tell the Z_80LogicGate when their values change 
	//so that it can tell the GUI
	WaveformFSM( Z_80LogicGate* z_80LogicGate );

	//This function is called by the InstructionManager
	//to get the waveformFSM to fetch an opcode from
	//the designated address in memory.  The address
	//is index by only 16 bits, so any extra bits in the PC are
	//truncated.
	//wavefromFSM will contact the InstructionManager
	//when it is done fetching the opcode
	void fetchOpcode( AddressType PC );
	
	
	//This function is called by the InstructionManager
	//to determine if a NMI or an INT interupt has been
	//recieved. This function call does not invoke a
	//waveform.
	InterruptCase checkInteruptState();
	
	//This function is called by the InstructionManage
	//to get the waveformFSM to fetch or save data to
	//the designated place in memory.   The address
	//is index by only 16 bits, so any extra bits are
	//truncated.
	//wavefromFSM will contact the InstructionManager
	//when it is done
	void preformMemoryOperation( AddressType newAddress, bool newIsWrite, int newData = 0 );
	
	
	//Copied out "1982/83 Data Book" page 42
	
	//Input or Output Cycles.  Figure 7 shows the
	//timing for and I/O read or I/O write operation.
	//During I/O operations, the CPU automatically
	//inserts a single Wait state (T_w).  This extra Wait
	//state allows sufficient time for an I/O port to
	//decond the address from the port address lines.
	
	//This functino is how the InstructionManager can order
	//an Input or Output Cycle
	void preformIOInput( int portNum );
	void preformIOOutput( int portNum, int value );
	
	//Copied out "1982/83 Data Book" page 43
	//Interrupt Request/Acknowledge Cycle.  The
	//CPU samples the interrupt signal with the ris-
	//ing edge of the last clock cycle at the end of
	//andy instruction (Figure 8).  When an interrupt
	//is accepted, a special /M1 cycle is generated.
	//During this /M1 cycle, /IORQ becomes active
	//(instead of /MREQ> to indicate that inter-
	//rupting decive can place an 8-bit vector on the
	//data bus.  The CPU automatically adds two
	//Wait states to this cycle.
	
	//This function is how the InstructionMachine can order
	//an Interupt Request/Acknowledge Cycle
	void preformINTRequestCycle( int pcValue );
	
	//Copied out "1982/83 Data Book" page 43
	//Non-Maskable Interrupt Request Cycle.
	///NMI is sampled at the same time as the
	//maskable interrupt input /INT but has higher
	//priority and cannot be disabled under software
	//control.  The subsequent timing is similar to
	//that of a normal memory read operation except
	//that data put on the bus by the memory is
	//ignored.  The CPU instead executes a restart
	//(RST) operation and jumps to the /NMI service
	//routine located at address 00566H (Figure 9)
	
	//This function is how the InstructionMachine can order
	//an Interupt Request/Acknowledge Cycle
	void preformNMIRequestCycle( int pcValue );
	
	
	//This instruction is called by the InstructionManager to preform a halt
	//the waveformFSM will still report back to the instructionManager at the end
	//of each 4 states.  The instructionManager will spacificaly reorder a new
	//halt until an interupt is received.
	//the instructionManager knowns if an interupt has accured by
	//quering the waveformFSM in the same way as it does at the end
	//of every instruction.
	void preformHalt( Z_80LogicGate* z_80LogicGate );
	
	//This function is how the reset cycle is ordered.
	//Because the when it is called does not depend
	//on where we are in an instruction.  It is ordered
	//by the instruction manager
	void preformResetCycle();


	//The Z_80LogicGate calls this every time the 
	//Z_80LogicGate receives a rising clock edge.  
	//This function then forwards the call to the 
	//private function that is responsible for handling 
	//the current T state.  
	void nextState( Z_80LogicGate* z_80LogicGate, bool isRisingEdge );
	
	//this function returns the current value of a named
	//register.  This function is called by Z_80LogicGate::getParameter
	string getReg( string name );
	
	//this method is so that the user can manipulate the values of the
	//registers
	void setReg( string regName, string newValue );
	
	//this function is how the Z_80LogicGate sets the run-mode
	//of both the waveformFSM and the InstructionManager
	void setRunMode( int run_mode );
	
	//this function is how the Z_80LogicGate notifies the waveformFSM
	//when it has received a NMI
	void notifyOfNMI();
	
	//this function is called when the the reset key in the popup
	//is called or the WaveformFSM counts three clocks worths
	//of the reset being held active
	void notifyInstructionManagerOfReset( Z_80LogicGate* z_80LogicGate );
	
	
	//this is how the z_80LogicGate tells the instruction manager
	//what the zad mode is.  The ZAD mode represents how the 
	//system responds when visiting addresses 'ocupied' by ZAD
	//operating system functions.
	void setZADMode( ZAD_MODE zadMode, Z_80LogicGate* z_80LogicGate );
	
	//this is how the z_80LogicGate tells the instruction manager
	//what the zad mode is.  The ZAD mode represents how the 
	//system responds when visiting addresses 'ocupied' by ZAD
	//operating system functions.
	ZAD_MODE getZADMode();
};

#endif /*WAVEFORMFSM_H_*/
