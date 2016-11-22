#ifndef Z_80LOGICGATE_H_
#define Z_80LOGICGATE_H_


//define the types that will be used for
//the address and the databus
typedef unsigned long AddressType;
typedef unsigned long DatabusType;

#include"../logic/logic_gate.h"
#include"InstructionManager.h"
#include"WaveformFSM.h"


//run modes of execution
#define CONTINUOUS 	0
//T-state stepping
#define T_STEP		1
//Instruction level stepping
#define I_STEP		2

//These functions take strings and
//make them paramiter transportable.
//(i.e. they remove all the spaces
//and encode them as something else)
string encodeParam( string param );
string decodeParam( string param );
string toHex( int value );
string toDec( int value );


class Z_80LogicGate : public Gate{
private:
	
	//we keep track of our current run mode so that
	//we can respond to queries about that property
	int currentRunMode;
	
	//a handy method for turning a bool into a ONE
	//or ZERO
	StateType getLowActiveState( bool isActive );
	
	//if the gate hasn't recieved a rise event
	//it should process any fall events.
	bool hasRecievedRise;
	
	//This is what is currently on the hex display
	//in the pop-up window
	string hexDisplay;
	
	//This is what is currently in the info
	//string in the pop-up
	string infoString;
	
	//This is set true when we display a warning
	//on the hex display so that we can remember
	//to take the warning back off once it has
	//been corrected
	bool powerWarning;
	
	//This lets us know if we have set the display
	//to the running continuouse display yet or
	//not
	bool setContinouseInfo;
	
	//This holds what was in the hex display
	//before we put the warning up so we can put
	//it back.
	string infoStringBackup;
	
	//This holds the property of the last key that
	//has been pressed.
	int currentKeyboardHardCode;
	
	//This holds the simulation time of when
	//the key got pressed.  This allows keys
	//to expire so that that the ZAD scan
	//function doesn't pick up a key press
	//that happened a long time ago.
	TimeType keyboardHardCodeSetTime;
	
	//The keyboard key gets set in the paramiter's box
	//However, we can't request the sim time until
	//next time we 'run'.  This is a flag so that
	//we will do that.
	bool needToTimeStampKeyStroke;
	
	//This lets safeListChangedParam know if it is safe
	//to directly list a paramiter or if it needs to stash
	//it first
	bool runningProcessGate;
	
	//keeps track of the paramiters which are waiting
	//for a gateProcess call to be listed in Gate
	vector<string> safeListedParams;
	
	//listChangedParam is dangerouse to call because if it is not called
	//from a that was called from gateProcess, an asertion will fail
	//and the system will crash.  This method will make sure it is 
	//safe to call it and then forward it the changed paramiter.
	//If it is not safe, it will log it in a collection which
	//will then be iterated next time gateProcess is called.
	//This method is private because only methods in Z_80LogicGate
	//should tecknically know the names of properties.
	void safeListChangedParam( string paramName );
	
	//this is called from gateProcess to go ahead and list all the
	//paramiters which were listed by safeListParam which didn't get logged
	void processSafeListedParams();
	
	
	//This needs to be listed as a member var after runningProcessGate,
	//so that runningProcessGate is set to false before the
	//waveformFSM constructor is called.
	//See the text in the waveformFSM file for a description
	//of what this var is for.
	WaveformFSM waveformFSM;
	
public:
	//constructor
	Z_80LogicGate();
	
	//these methods are used by WaveformFSM
	//to set the gates inputs and outputs
	void setAddressBus( AddressType address );
	void floatAddressBus();
	void setBUSACKActive( bool isActive, TimeType delay );
	void setDataBus( DatabusType data, TimeType delay );
	void floatDataBus();
	void setHALTActive( bool isActive, TimeType delay );
	void setIORQActive( bool isActive, TimeType delay );
	void setM1Active( bool isActive, TimeType delay );
	void setMREQActive( bool isActive, TimeType delay );
	void setRDActive( bool isActive, TimeType delay );
	void setWRActive( bool isActive, TimeType delay );
	
	
	//these methods are used by WavefromFSM
	//to get the data on the inputs and outputs
	bool isWAITActive();
	bool isINTActive();
	bool isRESETActive();
	DatabusType readDataBus();
	
	//a handy method for returning true if a mentioned
	//method is active
	bool isLowActiveState( StateType state );
	bool isHighActiveState( StateType state );
	
	//this method is called by the waveformFSM or InstructionManager
	//when they need to cause the simulation to stop because
	//they have hit their breakpoints
	void breakSimulation();
	
	//this method is called by Z_80Registers when a register changes
	//this is so that the GUI is noteified of the current content
	//of the registers.
	void notifyOfRegisterChange( string regName );
	
	//this method is called by instrucitonManager to let this class
	//know that it has changed the zad mode and it wants this class
	//to let the gui know.
	void notifyOfZadModeChange();
	
	//this method is used to change the hex display that
	//is in the pop-up window.  It is called by the instruction
	//manager when it is emulating the ZAD KBRD and SCAN
	//functions
	void setHexDisplay( string newValue );
	
	//this method is used to change the info string that
	//is in the pop-up window.  It is called by the
	//instruction manager when it is on instruction
	//level stepping and (TODO) by the WaveformFSM when
	//going through T-state level stepping
	void setInfoString( string newValue );
	
	//this is used by the emulated ZAD functions KBRD and SCAN
	//inside of the instructionManager to determine what
	//was key was the last pressed.
	int getCurrentKeyboardHardCode();
	
	//This allows the calling rutine to determine how old
	//the key is.  Thus the SCAN method can ignore a key
	//if it determines that it is too old
	int getKeyboardHardCodeAge();
		
	//we don't want to get a real old key, the first thing
	//KBRD will do is call this so that the key has to be
	//pressed after the program halts within the KBRD
	//routine
	void resetCurrentKeyboardHardCode();
	
	//This function is used to test if the user remembered to
	//power and ground the z80.  If this method returns false,
	//then the z80 will refuse to simulate
	bool isPowered();
	
	//these methods are the gates interface to the rest of the world
	bool setParameter( string paramName, string value );
	string getParameter( string paramName );
	void gateProcess();
	

	
	
};

#endif /*Z_80LOGICGATE_H_*/

