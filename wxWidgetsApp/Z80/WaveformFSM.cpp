#include<iostream>

using namespace std;

#include"Z_80LogicGate.h"
#include"WaveformFSM.h"

//an empty string to start of a series of concatenations
const string e = "";



//------------------WavefromFSM------------------------------

	//descriptions from "1982/83 Data Book" by Zilog
	//ignore references to refreshing dynamic memory.
	//this functionality will not be implemented because
	//it will detract from the student's ability to 
	//understand the meaning of each fetch from memory
	
	//The CPU executes instructions by pro-
	//ceeding through a specific sequence of opera-
	//tions:
	//*Memory read or write
	//*I/O device read or write
	//*Interrupt acknowledge
	//
	//The basic clock period is refered to as a T time
	// or cycle, and three or more T cycles make up a 
	//machine cycle(M1, M2 or M3 for instance).  Machine 
	//cycles can be extended either by the CPU 
	//automatically inserting one or more Wait states
	// or by the insertion of one or more Wait states 
	//by the user

WaveformFSM::WaveformFSM( Z_80LogicGate* z_80LogicGate ):
instructionManager( z_80LogicGate )
{
	
	tStepModeEnabled = false;
	hasRecievedNMI = false;
	isInHalt = false;
	
	nextTState = &WaveformFSM::bootState;
	
	sawInitingReset = false;
}

//This is the state that the waveformFSM is set to by the
//constructor.  It starts everything off
void WaveformFSM::bootState( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	//init thease values so that they
	//don't have the last thing that was
	//in the display when the file was saved last
	z_80LogicGate->setInfoString("");
	z_80LogicGate->setHexDisplay("000000");
	
	resetSignals( z_80LogicGate );
	instructionManager.requestInstruction( z_80LogicGate, this, data );
}

//This is called by each state to update the user on what is going
//on on that T-State
void WaveformFSM::describeState( string name, int num, string description, Z_80LogicGate* z_80LogicGate ){
	if( tStepModeEnabled ){
		ostringstream summary;
		summary << "T" << num << " of " << name << endl;
		summary << endl;
		summary << "description: " << description;
		z_80LogicGate->setInfoString( summary.str() );
	}
}

//This gets called from the constructor and when the waveformFSM
//gets reset
void WaveformFSM::resetSignals( Z_80LogicGate* z_80LogicGate ){
	//one of the first things we should do is init
	//all the inputs of the Z_80LogicGate to non active
	//a delay of 1 is sent just so that there is some delay
	//and not no delay
	z_80LogicGate->setBUSACKActive( false, 1 );
	z_80LogicGate->setHALTActive( false, 1 );
	z_80LogicGate->setIORQActive( false, 1 );
	z_80LogicGate->setM1Active( false, 1 );
	z_80LogicGate->setMREQActive( false, 1 );
	z_80LogicGate->setRDActive( false, 1 );
	z_80LogicGate->setWRActive( false, 1 );
}
	

//Z-80 instruction fetch T-States

//Instruction Opcode Fetch.  The CPU places the contents 
//of the Program Counter (PC) on the address bus at the 
//start of the cycle (Figure 5). Approximately one-half 
//clock cycle later, /MREQ goes active.  When active, 
// /RD indicates that the memory data can be enabled onto 
//the CPU data bus.

//The CPU samples the /WAIT input with the falling edge 
//of clock state T2.  During clock states T3 and T4 of 
//an M1 cycle dynamic RAM refresh can occur while the 
//CPU starts decoding and executing the instructions.  
//When the Refresh Control signal becomes active, 
//refreshing of dynamic memory can take place.

//This function is called by the InstructionManager
//to get the waveformFSM to fetch an opcode from
//the designated address in memory.  The address
//is index by only 16 bits, so any extra bits are
//truncated.
//wavefromFSM will contact the InstructionManager
//when it is done fetching the opcode
void WaveformFSM::fetchOpcode( AddressType PC ){
	address = PC;

	nextTState = &WaveformFSM::IFT1;
}


void WaveformFSM::IFT1( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		//send out the address
		z_80LogicGate->setAddressBus(address);
		z_80LogicGate->setMREQActive( true, CLK_R_TO_ADDRESS_6 + ADDRESS_TO_MREQ_F_7 );
		z_80LogicGate->setM1Active( true, CLK_R_TO_M1_F_19 );
		z_80LogicGate->floatDataBus();
		
		
		describeState( "Instruction Opcode Fetch", 1, e + "^ The address 0x" + toHex( address ) + " is on the data-lines, the MREQ is active, M1 is active and the data-bus is floating", z_80LogicGate  );
	}else{
		//send out read command
		z_80LogicGate->setRDActive( true, CLK_F_TO_RD_F_13 );
		
		describeState( "Instruction Opcode Fetch", 1, e + "v RD is active to request opcode", z_80LogicGate  );
	
		nextTState = &WaveformFSM::IFT2;	
	}
}
void WaveformFSM::IFT2( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		
		describeState( "Instruction Opcode Fetch", 2, "^ Waiting for for memory to produce results", z_80LogicGate  );
	}else{
		if( !z_80LogicGate->isWAITActive() ){
			nextTState = &WaveformFSM::IFT3;
			describeState( "Instruction Opcode Fetch", 2, "v The wait pin wasn't active, proceeding to state T3", z_80LogicGate  );
		}else{
			describeState( "Instruction Opcode Fetch", 2, "v Acknowledging wait pin.", z_80LogicGate  );
		}
	}
}
void WaveformFSM::IFT3( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		//this would be were we are sending out the refresh address
		//however, we will just send out zero
		z_80LogicGate->setAddressBus( 0 );
		
		//we skip the rest of the wave form for the MREQ signal
		//because it deals with refreshing dynamic memory
		z_80LogicGate->setMREQActive( false, CLK_R_TO_MREQ_9 );
		
		z_80LogicGate->setRDActive( false, CLK_R_TO_RD_R_14 );
		z_80LogicGate->setM1Active( false, CLK_R_TO_M1_R_20 );
		
		data = z_80LogicGate->readDataBus();

		describeState( "Instruction Opcode Fetch", 3, e + "^ Data 0x" + toHex( data ) + " (" + toDec( data ) + ") is read from data bus.  Address-bus is reset (refresh not emulated), MREQ is inactive, RD is inactive and M1 is no longer active.", z_80LogicGate  );
	}else{
		describeState( "Instruction Opcode Fetch", 3, "v Proceeding to T4", z_80LogicGate  );
		nextTState = &WaveformFSM::IFT4;
	}
}
void WaveformFSM::IFT4( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		hasRecievedINT = z_80LogicGate->isINTActive();
		describeState( "Instruction Opcode Fetch", 4, e + "^ Noting that interrupt is " + (hasRecievedINT?"":"not ") + "active" , z_80LogicGate  );
	}else{
		//comunication with the InstructionManager must
		//happen on a falling edge.  If it is done on 
		//a rising edge, the waveform the InstructionManager
		//orders will recieve our falling edge.
		
		
		describeState( "Instruction Opcode Fetch", 4, "v Processing opcode", z_80LogicGate  );
		
		instructionManager.requestInstruction( z_80LogicGate, this, data );
	}
}


//Memory Read or Write Cycles.  Figure 6 shows the 
//timing of memory read or write cycles other than 
//an opcode fetch (/M1) cycle.  The /MREQ and /RD 
//signals function exactly  as in the fetch cycle.  
//In a memory write cycle, /MReq also becomes active 
//when the address bus is stable.  The /WR line is 
//active when the data bus is stable, so that it can 
//be used directly ad an r//W pulse to most semiconductor 
//memories.

//This function is called by the InstructionManage
//to get the waveformFSM to fetch or save data to
//the designated place in memory.   The address
//is index by only 16 bits, so any extra bits are
//truncated.
//wavefromFSM will contact the InstructionManager
//when it is done
void WaveformFSM::preformMemoryOperation( AddressType newAddress, bool newIsWrite, int newData ){
	isWrite = newIsWrite;
	data = newData;
	address = newAddress;
	
	nextTState = &WaveformFSM::MEMT1;
}



//memory read or write TStates
void WaveformFSM::MEMT1( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		z_80LogicGate->setAddressBus( address );
		if( !isWrite ){
			z_80LogicGate->floatDataBus();
		}
		
		describeState( (isWrite?"Memory Write Cycle":"Memory Read Cycle"), 1, e + "^ Address bus set to 0x" + toHex( address ) + " (" + toDec( address ) + ") " + (isWrite?"":", Databus floats)"), z_80LogicGate  );
	}else{
		z_80LogicGate->setMREQActive( true, CLK_F_TO_MREQ_8 );
		if( !isWrite ){
			z_80LogicGate->setRDActive(true, CLK_F_TO_RD_F_13 );
			describeState( "Memory Read Cycle", 1, "v MREQ is active to signal request, RD is active to show what type or request", z_80LogicGate  );
		}else{
			z_80LogicGate->setDataBus( data, CLK_F_TO_DATA_VALID_53 );
			z_80LogicGate->setWRActive( true, CLK_F_TO_DATA_VALID_53+DATA_STABLE_BFOR_WR_F_29 );
			describeState( "Memory Write Cycle", 1, e + "v MREQ is active to signal request, WR is active to show what type of request and the data 0x" + toHex( data ) + " (" + toDec( data ) + ") is placed on the data-bus", z_80LogicGate  );
		}
		nextTState = &WaveformFSM::MEMT2;
	}
}
void WaveformFSM::MEMT2( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		if( isWrite ){
			describeState( "Memory Write Cycle", 2, e + "^ Waiting for memory to latch", z_80LogicGate  );
		}else{
			describeState( "Memory Read Cycle", 2, e + "^ Waiting for data from memory to become stable", z_80LogicGate  );
		}
	}else{
		if( !z_80LogicGate->isWAITActive() ){
			nextTState = &WaveformFSM::MEMT3;
			describeState( (isWrite?"Memory Write Cycle":"Memory Read Cycle"), 2, "v Honoring wait signal", z_80LogicGate  );
		}else{
			describeState( (isWrite?"Memory Write Cycle":"Memory Read Cycle"), 2, "v Wait signal isn't active, proceeding to T3", z_80LogicGate  );
		}
	}
}
void WaveformFSM::MEMT3( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		//TODO after the io waveform has been implemented, make sure
		//that the hasRecievedNMI has been added to that as well.
		hasRecievedINT = z_80LogicGate->isINTActive();
		
		describeState( (isWrite?"Memory Write Cycle":"Memory Read Cycle"), 3, e + "^ Noting that mask-able interrupt is " + (hasRecievedINT?"":"not ") + "active", z_80LogicGate  );
	}else{
		z_80LogicGate->setMREQActive( false, CLK_F_TO_MREQ_R_12 );
		if( !isWrite ){
			z_80LogicGate->setRDActive( false, CLK_F_TO_RD_R_23 );
			data = z_80LogicGate->readDataBus();
			describeState( "Memory Read Cycle", 3, "v MREQ and RD are no longer active and the data 0x" + toHex( data ) + " (" + toDec( data ) + ") is read off of the data lines", z_80LogicGate );
		}else{
			z_80LogicGate->setWRActive( false, CLK_F_TO_WR_R_32 );
			describeState( "Memory Read Cycle", 3, "v MREQ and WR are no longer active", z_80LogicGate );
		}
		nextTState = &WaveformFSM::MEMT4ish;
	}
}

//the MEM waveform doesn't realy have a state four.  However
//it needs to dispatch some events during the next waveform's
//first cycle. The 4thish state will recall nextTState
//to keep the next wavefrom from getting behind
void WaveformFSM::MEMT4ish( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	z_80LogicGate->setAddressBus(0);
	z_80LogicGate->floatDataBus();
	//call instruction manager to get next instruction
	//but then call it without waiting for another clock
	//because we are are an ish state
	
	instructionManager.requestInstruction( z_80LogicGate, this, data );
	(this->*nextTState)( z_80LogicGate, isRisingEdge );
}


//This function is called by the InstructionManager
//to determine if a NMI or an INT interupt has been
//recieved. This function call does not invoke a
//waveform.
InterruptCase WaveformFSM::checkInteruptState(){
	if( hasRecievedNMI ){
		hasRecievedNMI = false;
		return NMI_interrupt;
	}else if( hasRecievedINT ){
		hasRecievedINT = false;
		return INT_interrupt;
	}else{
		return DIDNT_interrupt;
	}
}

//Copied out "1982/83 Data Book" page 42

//Input or Output Cycles.  Figure 7 shows the
//timing for and I/O read or I/O write operation.
//During I/O operations, the CPU automatically
//inserts a single Wait state (T_w).  This extra Wait
//state allows sufficient time for an I/O port to
//decond the address from the port address lines.

//This functino is how the InstructionManager can order
//an Input or Output Cycle
void WaveformFSM::preformIOInput( int portNum ){
	isWrite = false;
	address = portNum;
	nextTState = &WaveformFSM::IO1;
}
void WaveformFSM::preformIOOutput( int portNum, int value ){
	isWrite = true;
	address = portNum;
	data = value;
	nextTState = &WaveformFSM::IO1;
}
void WaveformFSM::IO1( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		//send out the address
		z_80LogicGate->setAddressBus(address);//check
		if( !isWrite ){
			z_80LogicGate->floatDataBus();//check
			describeState( "Input Cycle", 1, e + "^ Address bus set to address 0x" + toHex( address ) + " (" + toDec( address ) + "), and the data-bus is floating", z_80LogicGate );
		}else{
			describeState( "Output Cycle", 1, e + "^ Address bus set to address 0x" + toHex( address ) + " (" + toDec( address ) + ")", z_80LogicGate );
		}
	}else{
		if( isWrite ){
			z_80LogicGate->setDataBus( data, CLK_F_TO_DATA_VALID_53 );
			describeState( "Output Cycle", 1, e + "v Output value of 0x" + toHex( data ) + " (" + toDec( data ) + ") placed on data-bus", z_80LogicGate );
		}else{
			describeState( "Input Cycle", 1, "v Proceeding to state T2", z_80LogicGate );
		}
		nextTState = &WaveformFSM::IO2;	
		waitStateCounter = 0;
	}
}
void WaveformFSM::IO2( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		if( waitStateCounter == 0 ){
			z_80LogicGate->setIORQActive( true, CLK_R_TO_IORQ_F_27 );
			if( isWrite ){
				z_80LogicGate->setWRActive( true, CLK_R_TO_WR_F_34 );
				describeState( "Output Cycle", 2, "^ IORQ and WR are active to signal IO write", z_80LogicGate );
			}else{
				z_80LogicGate->setRDActive( true, CLK_R_TO_RD_F_24 );
				describeState( "Input Cycle", 2, "^ IORQ and RD are active to signal IO read", z_80LogicGate );
			}
		}else{
			describeState( e + (isWrite?"Output ":"Input ") + "Cycle", 2, "^ Waiting", z_80LogicGate );
		}
	}else{
		if( waitStateCounter >= NUM_IO_WAIT_STATES && !z_80LogicGate->isWAITActive() ){
			nextTState = &WaveformFSM::IO3;
			describeState( e + (isWrite?"Output ":"Input ") + "Cycle", 2, "v Proceeding to T3", z_80LogicGate );
		}else{
			if( waitStateCounter < NUM_IO_WAIT_STATES ){
				describeState( e + (isWrite?"Output ":"Input ") + "Cycle", 2, "v Automatically adding a wait state", z_80LogicGate );
			}else{
				describeState( e + (isWrite?"Output ":"Input ") + "Cycle", 2, "v Acknowledging wait signal", z_80LogicGate );
			}
			waitStateCounter++;
		}
	}
}
void WaveformFSM::IO3( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		hasRecievedINT = z_80LogicGate->isINTActive();
		describeState( e + (isWrite?"Output ":"Input ") + "Cycle", 3, e + "^ Noting mask-able interrupt is " + (hasRecievedINT?"":"not ") + "active", z_80LogicGate  );
	}else{
		//it looks like the address lasts until the next waveform changes
		//it to something else.  Instead of haveing two conflicting commands
		//that land close to the same time, we will just not command
		//the address bus to do anything
		
		z_80LogicGate->setIORQActive( false, CLK_F_TO_IORQ_R_28 );
		
		if( isWrite ){
			z_80LogicGate->setWRActive( false, CLK_F_TO_WR_R_32 );
			//we will let the next waveform take the data off of the
			//bus line.  I don't think that is this wave's job
			describeState( "Output Cycle", 3, "v IORQ and WR no longer active.", z_80LogicGate  );
		}else{
			data = z_80LogicGate->readDataBus();
			z_80LogicGate->setRDActive( false, CLK_F_TO_RD_R_23 );
			describeState( "Input Cycle", 3, e + "v IORQ and RD no longer active.  Processing data 0x" + toHex( data ) + " (" + toDec( data ) + ") read from port", z_80LogicGate  );
		}
		
		instructionManager.requestInstruction( z_80LogicGate, this, data );
	}
}


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

//This function is how the InstructionManager can order
//an Interupt Request/Acknowledge Cycle
void WaveformFSM::preformINTRequestCycle( int pcValue ){
	address = pcValue;
	nextTState = &WaveformFSM::INT1;
}
void WaveformFSM::INT1( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		z_80LogicGate->setAddressBus(address);
		z_80LogicGate->setM1Active( true, CLK_R_TO_M1_F_19 );
		z_80LogicGate->floatDataBus();
		
		describeState( "Interrupt Request/Acknowledge Cycle", 1, e + "^ Address buss set to the PC 0x" + toHex( address ) + " (" + toDec( address ) + "), M1 is active and the data-bus is floating", z_80LogicGate );
	}else{
		waitStateCounter = 0;
		
		describeState( "Interrupt Request/Acknowledge Cycle", 1, e + "v Proceeding to T2", z_80LogicGate );
		
		nextTState = &WaveformFSM::INT2;
	}
}
void WaveformFSM::INT2( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		
		describeState( "Interrupt Request/Acknowledge Cycle", 2, e + "^ Waiting on interrupting io-device", z_80LogicGate );
		
	}else{
		if( waitStateCounter == 0 ){
			z_80LogicGate->setIORQActive( true, CLK_F_TO_IORQ_F_51 );
		}
		
		//the interupt service rutine automaticaly has
		//two wait states in it.  Therefore
		//we will use the member vairable data to tell
		//us if we have waited at least once yet.
		if( waitStateCounter >= NUM_INT_WAIT_STATES && !z_80LogicGate->isWAITActive() ){
			nextTState = &WaveformFSM::INT3;
			
			describeState( "Interrupt Request/Acknowledge Cycle", 2, e + "v Proceeding to T3", z_80LogicGate );
		}else{
			if( waitStateCounter < NUM_INT_WAIT_STATES ){
				describeState( "Interrupt Request/Acknowledge Cycle", 2, "v Automatically adding waiting state", z_80LogicGate );
			}else{
				describeState( "Interrupt Request/Acknowledge Cycle", 2, "v Acknowledging WAIT signal", z_80LogicGate );
			}
			
			waitStateCounter++;
		}
	}
}
void WaveformFSM::INT3( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		//this would be were we are sending out the refresh address
		//however, we will just send out zero
		z_80LogicGate->setAddressBus( 0 );
		
		//we skip the rest of the wave form for the MREQ signal
		//because it deals with refreshing dynamic memory
		z_80LogicGate->setIORQActive( false, CLK_R_TO_IORQ_R_52 );
		
		z_80LogicGate->setM1Active( false, CLK_R_TO_M1_R_20 );
		
		data = z_80LogicGate->readDataBus();
		
		describeState( "Interrupt Request/Acknowledge Cycle", 3, e + "^ Address-bus set to zero (instead of refreshed...), IORQ and M1 are no longer active and the data 0x" + toHex( data ) + " (" + toDec( data ) + ") is latched off of the data-bus", z_80LogicGate );
	}else{
		nextTState = &WaveformFSM::INT4;
		
		describeState( "Interrupt Request/Acknowledge Cycle", 3, "v Proceeding to state 4", z_80LogicGate );
	}
}
void WaveformFSM::INT4( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		hasRecievedINT = z_80LogicGate->isINTActive();
	
		describeState( "Interrupt Request/Acknowledge Cycle", 4, e + "^ Noting mask-able interrupt is " + (hasRecievedINT?"":"not ") + "active", z_80LogicGate  );
	}else{
		//communication with the InstructionManager must
		//happen on a falling edge.  If it is done on 
		//a rising edge, the waveform the InstructionManager
		//orders will receive our falling edge.
		
		describeState( "Interrupt Request/Acknowledge Cycle", 4, "v Processing interrupt", z_80LogicGate  );
		
		instructionManager.requestInstruction( z_80LogicGate, this, data );
	}
}

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
void WaveformFSM::preformNMIRequestCycle( int pcValue ){
	address = pcValue;
	nextTState = &WaveformFSM::NMI1;
}

void WaveformFSM::NMI1( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		z_80LogicGate->setAddressBus(address);
		z_80LogicGate->setM1Active( true, CLK_R_TO_M1_F_19 );
		describeState( "Non-Maskable Interrupt Request Cycle", 1, "^ Address bus set to PC 0x" + toHex( address ) + " (" + toDec( address ) + ") and M1 set active", z_80LogicGate );
	}else{
		z_80LogicGate->setMREQActive( true, CLK_F_TO_MREQ_8 );
		z_80LogicGate->setRDActive(true, CLK_F_TO_RD_F_13 );
		
		describeState( "Non-Maskable Interrupt Request Cycle", 1, "v MREQ and RD set active", z_80LogicGate );
		
		nextTState = &WaveformFSM::NMI2;
	}
}
void WaveformFSM::NMI2( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		describeState( "Non-Maskable Interrupt Request Cycle", 2, "^", z_80LogicGate );
	}else{
		describeState( "Non-Maskable Interrupt Request Cycle", 2, "v Proceeding to T3", z_80LogicGate );
		nextTState = &WaveformFSM::NMI3;
	}
}
void WaveformFSM::NMI3( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		z_80LogicGate->setAddressBus(0);
		z_80LogicGate->setM1Active( false, CLK_R_TO_M1_R_20 );
		z_80LogicGate->setMREQActive( false, CLK_R_TO_MREQ_9 );
		z_80LogicGate->setRDActive( false, CLK_R_TO_M1_R_20 );
		//reresh too, but we aren't doing that
		describeState( "Non-Maskable Interrupt Request Cycle", 3, "^ Address bus set to 0 (Would be Refresh), M1, MREQ and RD no longer active", z_80LogicGate );
	}else{
		
		describeState( "Non-Maskable Interrupt Request Cycle", 3, "v Proceeding to state T4", z_80LogicGate );
		
		nextTState = &WaveformFSM::NMI4;
	}
}
void WaveformFSM::NMI4( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		describeState( "Non-Maskable Interrupt Request Cycle", 4, "^", z_80LogicGate );
	}else{
		describeState( "Non-Maskable Interrupt Request Cycle", 4, "v Proceeding to state T5. (End of Refresh would have happened here)", z_80LogicGate );
		//end of mem request at the refresh address would have accured herd
		nextTState = &WaveformFSM::NMI5;
	}
}
void WaveformFSM::NMI5( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		//refresh signal would have ended here.
		hasRecievedINT = z_80LogicGate->isINTActive();
		describeState( "Non-Maskable Interrupt Request Cycle", 5, e + "^ Noting mask able interrupt is " + (hasRecievedINT?"":"not ") + "active", z_80LogicGate  );
	}else{
		describeState( "Non-Maskable Interrupt Request Cycle", 5, "Processing non-maskable interupt", z_80LogicGate  );
		instructionManager.requestInstruction( z_80LogicGate, this, data );
	}
}
		
		



//This instruction is called by the InstructionManager to preform a halt
//the waveformFSM will still report back to the instructionManager at the end
//of each 4 states.  The instructionManager will spacificaly reorder a new
//halt until an interupt is received.
//the instructionManager knowns if an interupt has accured by
//quering the waveformFSM in the same way as it does at the end
//of every instruction.
void WaveformFSM::preformHalt( Z_80LogicGate* z_80LogicGate ){
	//we need to immediately send the halt signal down after a delay of type 36.
	//then we need to start a 5 cycle halt waveform
	
	//we are assuming that the instruction manager is calling this function because
	//from a call to the instruction manager from the wavefromFSM on the falling edge
	//of a final state.
	if( !isInHalt ){
		z_80LogicGate->setHALTActive( true, CLK_F_TO_HALT_R_OR_F_36 );
		isInHalt = true;
	}
	
	haltTState = 1;
	nextTState = &WaveformFSM::HALT1_4;
}


//this is the waveform that we do when we are in a halt
//this takes care of all 4 Tstates of one halt cycle
void WaveformFSM::HALT1_4( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		if( haltTState == 4 ){
			hasRecievedINT = z_80LogicGate->isINTActive();
			describeState( "Halt Acknowledge Cycle", 4, e + "^ Noting maskable interrupt is " + (hasRecievedINT?"":"not ") + "active", z_80LogicGate  );
		}else{
			describeState( "Halt Acknowledge Cycle", haltTState, "^", z_80LogicGate  );
		}
	}else{
		if( haltTState < 4 ){
			describeState( "Halt Acknowledge Cycle", haltTState, e + "v Proceeding to state T" + toDec( haltTState + 1 ), z_80LogicGate  );
			
			haltTState++;
		}else{
			describeState( "Halt Acknowledge Cycle", haltTState, e + "v Determining further action" + toDec( haltTState + 1 ), z_80LogicGate  );
			
			//ask the instruction manager if we are done yet.
			instructionManager.requestInstruction( z_80LogicGate, this, 0 );
			//now see what our pointer got set too.
			//if it isn't still halt, then we know the halt is over
			//and we should reliece the halt signal
			if( nextTState !=  &WaveformFSM::HALT1_4 ){
				z_80LogicGate->setHALTActive( false, CLK_F_TO_HALT_R_OR_F_36 );
				isInHalt = false;
			}
		}
	}
}


//Copied out "1982/83 Data Book" page 43

//Reset Cycle.
///RESET must be active for at least
//three clock cycles for the CPU to properly
//accept it.  As long as /RESET remains active, the
//address and data buses float, and the control
//outputs are inactive.  Once /RESET goes
//inactive, three internal T cycles are consumed
//before the CPU resumes normal processing
//operation. /RESET clears the PC register, so the
//first opcode fetch will be to location 0000
//(Figure 12).

//This function is how the InstructionMachine can order
//an Reset Cycle
void WaveformFSM::preformResetCycle(){
	nextTState = &WaveformFSM::RESET1;
}

void WaveformFSM::RESET1( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){ //qwert
		//here we must float the address bus
		//and the data lines and raise M1
		//and a ton of other things
		
		z_80LogicGate->floatAddressBus();
		z_80LogicGate->floatDataBus();
		z_80LogicGate->setM1Active( false, NICE_DELAY_AMOUNT );
		z_80LogicGate->setMREQActive( false, NICE_DELAY_AMOUNT );
		z_80LogicGate->setRDActive( false, NICE_DELAY_AMOUNT );
		z_80LogicGate->setWRActive( false, NICE_DELAY_AMOUNT );
		z_80LogicGate->setIORQActive( false, NICE_DELAY_AMOUNT );
		z_80LogicGate->setBUSACKActive( false, NICE_DELAY_AMOUNT );
		z_80LogicGate->setHALTActive( false, NICE_DELAY_AMOUNT );
		resetWasActive = true;
		
		describeState( "Reset Cycle", 1, "^ Seting control signals to inactive", z_80LogicGate);
		
	}else{
		describeState( "Reset Cycle", 1, "v", z_80LogicGate );
		nextTState = &WaveformFSM::RESET2;
	}
}
void WaveformFSM::RESET2( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
		//here we check to see if the halt pin has been let up
		if( !z_80LogicGate->isRESETActive() ){
			describeState( "Reset Cycle", 2, "^ Reset is no longer active. Now going to count to three.", z_80LogicGate );
			resetWasActive = false;
		}else{
			describeState( "Reset Cycle", 2, "^ Reset is still active. Chilling here in 2", z_80LogicGate );
		}
	}else{
		describeState( "ResetCycle", 2, "v", z_80LogicGate );
		if( !resetWasActive ){
			nextTState = &WaveformFSM::RESET3;
			waitStateCounter = 1; //will count to three
		}
	}
}
void WaveformFSM::RESET3( Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	if( isRisingEdge ){
	  //describeState( "Interrupt Request/Acknowledge Cycle", 4, e + "^ Noting mask-able interrupt is " + (hasRecievedINT?"":"not ") + "active", z_80LogicGate  );
	
		describeState( "ResetCycle", 3, e + "^ On count " + toDec( waitStateCounter ), z_80LogicGate );
	}else{
		describeState( "ResetCycle", 3, e + "v On count " + toDec( waitStateCounter ), z_80LogicGate );
		waitStateCounter++;
		if( waitStateCounter == 3 ){	
			instructionManager.requestInstruction( z_80LogicGate, this, 0 );
		}
	}
}
	

//The Z_80LogicGate calls this every time the Z_80LogicGate 
//receives a rising clock edge.  This function then forwards 
//the call to the private function that is responsible for 
//handling the current T state.  
void WaveformFSM::nextState(Z_80LogicGate* z_80LogicGate, bool isRisingEdge ){
	//check to see if we should block
	if( tStepModeEnabled ) z_80LogicGate->breakSimulation();
	
	//even if the simulation pauses we should still scedual
	//the events that should take place in the future.
	(this->*nextTState)( z_80LogicGate, isRisingEdge );
	
	//here we count the number of cycles we see a reset
	//active.  If it gets to three we will notify the
	//InstructionManager
	if( isRisingEdge ){
		if( z_80LogicGate->isRESETActive() ){
			if( resetCount == 3 ){
				sawInitingReset = true;
				resetCount = 0;
				//notifyInstructionManagerOfReset( z_80LogicGate );
			}else{
				resetCount++;
			}
		}else{
			resetCount = 0;
		}
		//we also reset if someone is pressing the reset button
		if( z_80LogicGate->getCurrentKeyboardHardCode() == H_KEY_RESET ){
			z_80LogicGate->resetCurrentKeyboardHardCode();
			sawInitingReset = true;
			//notifyInstructionManagerOfReset( z_80LogicGate );
		}
	}else if( sawInitingReset ){
		//we must only change waveforms on the falling edge so that each T
		//state recieves both its rising and falling edge.
		//Thus we use the sawInitingReset to sample on the rising
		//but transition on the falling
		sawInitingReset = false;
		notifyInstructionManagerOfReset( z_80LogicGate );
	}
}


//this function returns the current value of a named
//register.  This function is called by Z_80LogicGate::getParameter
string WaveformFSM::getReg( string name ){
	return instructionManager.getReg( name );
}

//this method is so that the user can manipulate the values of the
//registers
void WaveformFSM::setReg( string regName, string newValue ){
	instructionManager.setReg( regName, newValue );
}

//this function is how the Z_80LogicGate sets the run-mode
//of both the waveformFSM and the InstructionManager
void WaveformFSM::setRunMode( int runMode ){
	tStepModeEnabled = (runMode == T_STEP);
	instructionManager.setIStepMode( runMode == I_STEP );
}

//this function is how the Z_80LogicGate notifies the waveformFSM
//when it has received a NMI
void WaveformFSM::notifyOfNMI(){
	hasRecievedNMI = true;
}

//this function is called when the the reset key in the popup
//is called or the WaveformFSM counts three clocks worths
//of the reset being held active
void WaveformFSM::notifyInstructionManagerOfReset( Z_80LogicGate* z_80LogicGate ){
	instructionManager.doReset( z_80LogicGate, this );
}

//this is how the z_80LogicGate tells the instruction manager
//what the zad mode is.  The ZAD mode represents how the 
//system responds when visiting addresses 'ocupied' by ZAD
//operating system functions.
void WaveformFSM::setZADMode( ZAD_MODE zadMode, Z_80LogicGate* z_80LogicGate ){
	instructionManager.setZADMode( zadMode, z_80LogicGate );
}

//this is how the z_80LogicGate tells the instruction manager
//what the zad mode is.  The ZAD mode represents how the 
//system responds when visiting addresses 'ocupied' by ZAD
//operating system functions.
ZAD_MODE WaveformFSM::getZADMode(){
	return instructionManager.getZADMode();
}
//------------end of WaveformFSM-----------------------------
