
#pragma once

#include <string>
#include <sstream>

enum struct MessageType {
	// core -> GUI
	SET_WIRE_STATE,
	DONESTEP,
	COMPLETE_INTERIM_STEP,
		
	// GUI -> core
	REINITIALIZE,
	CREATE_GATE,
	CREATE_WIRE,
	DELETE_GATE,
	DELETE_WIRE,
	SET_GATE_INPUT,
	SET_GATE_INPUT_PARAM,
	SET_GATE_OUTPUT,
	SET_GATE_OUTPUT_PARAM,
	SET_GATE_PARAM,
	STEPSIM,
	UPDATE_GATES
};

class Message {
public:
	MessageType type;
	Message(MessageType t) : type(t) { };
};
	
class Message_SET_WIRE_STATE : public Message {
public:
	int wireId;
	int state;
	Message_SET_WIRE_STATE( int wid, int s ) : Message(MessageType::SET_WIRE_STATE), wireId(wid), state(s) { };
};

class Message_DONESTEP : public Message {
public:
	int logicTime;
	Message_DONESTEP( int lt ) : Message(MessageType::DONESTEP), logicTime(lt) {};
};

class Message_COMPLETE_INTERIM_STEP : public Message {
public:
	Message_COMPLETE_INTERIM_STEP() : Message(MessageType::COMPLETE_INTERIM_STEP) { }
};

class Message_REINITIALIZE : public Message {
public:
	Message_REINITIALIZE() : Message(MessageType::REINITIALIZE) { }
};
	
class Message_CREATE_GATE : public Message {
public:
	std::string gateType;
	int gateId;
	Message_CREATE_GATE(std::string gt, int gid ) : Message(MessageType::CREATE_GATE), gateType(gt), gateId(gid) {};
};

class Message_CREATE_WIRE : public Message {
public:
	int wireId;
	Message_CREATE_WIRE( int wid ) : Message(MessageType::CREATE_WIRE), wireId(wid) {};
};

class Message_DELETE_GATE : public Message {
public:
	int gateId;
	Message_DELETE_GATE( int gid ) : Message(MessageType::DELETE_GATE), gateId(gid) {};
};

class Message_DELETE_WIRE : public Message {
public:
	int wireId;
	Message_DELETE_WIRE( int wid ) : Message(MessageType::DELETE_WIRE), wireId(wid) {};
};

class Message_SET_GATE_INPUT : public Message {
public:
	int gateId;
	std::string inputId;
	int wireId;
	bool disconnect;
	Message_SET_GATE_INPUT( int gid, std::string iid, int wid, bool d = false ) : Message(MessageType::SET_GATE_INPUT), gateId(gid), inputId(iid), wireId(wid), disconnect(d) {};
};

class Message_SET_GATE_INPUT_PARAM : public Message {
public:
	int gateId;
	std::string inputId;
	std::string paramName;
	std::string paramValue;
	Message_SET_GATE_INPUT_PARAM( int gid, std::string iid, std::string pN, std::string pV ) : Message(MessageType::SET_GATE_INPUT_PARAM), gateId(gid), inputId(iid), paramName(pN), paramValue(pV) {};
};

class Message_SET_GATE_OUTPUT : public Message {
public:
	int gateId;
	std::string outputId;
	int wireId;
	bool disconnect;
	Message_SET_GATE_OUTPUT( int gid, std::string oid, int wid, bool d = false ) : Message(MessageType::SET_GATE_OUTPUT), gateId(gid), outputId(oid), wireId(wid), disconnect(d) {};
};

class Message_SET_GATE_OUTPUT_PARAM : public Message {
public:
	int gateId;
	std::string outputId;
	std::string paramName;
	std::string paramValue;
	Message_SET_GATE_OUTPUT_PARAM( int gid, std::string oid, std::string pN, std::string pV ) : Message(MessageType::SET_GATE_OUTPUT_PARAM), gateId(gid), outputId(oid), paramName(pN), paramValue(pV) {};
};

class Message_SET_GATE_PARAM : public Message {
public:
	int gateId;
	std::string paramName;
	std::string paramValue;
	Message_SET_GATE_PARAM( int gid, std::string pN, std::string pV ) : Message(MessageType::SET_GATE_PARAM), gateId(gid), paramName(pN), paramValue(pV) {};
	Message_SET_GATE_PARAM( int gid, std::string pN, long pV, bool useHex = false ) : Message(MessageType::SET_GATE_PARAM), gateId(gid), paramName(pN) {
		std::ostringstream oss; oss << (useHex ? std::hex : std::dec) << pV; paramValue = oss.str();
	};
};

class Message_STEPSIM : public Message {
public:
	int numSteps;
	Message_STEPSIM( int n ) : Message(MessageType::STEPSIM), numSteps(n) {};
};

class Message_UPDATE_GATES : public Message {
public:
	Message_UPDATE_GATES() : Message(MessageType::UPDATE_GATES) { }
};