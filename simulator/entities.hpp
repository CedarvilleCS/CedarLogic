#pragma once

#include <vector>
#include <set>
#include "logic.hpp"
#include <optional>

enum class Gates {
	AND, XOR, OR, NAND, NOR // XNOR not implemented
};


// A wire is the "line" between two junction "points"
struct Wire {

	bool has_junction(uint32_t index) {
		return junction_indexes.find(index) != junction_indexes.end();
	}

	// keeps track of all connected junctions
	std::set<uint32_t> junction_indexes;
};

// This is the only place where you can connect and disconnect wires.
// Except, if a wire disappears, we need to know if the network is still sound.
class Junction {
public:
	enum class Type {
		Input, Output
	};

	// Child classes must supply their type on construction.
	Junction() = delete;

	// This class must know it's type from birth.
	Junction(Type type) : type(type) {}

	// Get the type of junction this is.
	const Type get_type() const {
		return type;
	}

private:
	Type type;
};

// An output is a junction with it's own driven state.
class Output : public Junction {
public:
	// Default constructor
	Output();

	Logic_Value driven_state;
};

// An input is a junction who's state is driven.
class Input : public Junction {
public:

	// Inputs must know what to call to get their state.
	Input();

	// Using a function ptr so Inputs doesn't need to have an awareness of networks.
	std::optional<uint32_t> network_index;
};

class Network {
public:
	// Get the state of the network
	const Logic_Value get_state() const { return state; }

	bool has_junction(uint32_t junction_index) const;

	bool has_wire(uint32_t wire_index) const;

	// One of it's wires or junctions has been deleted since last this was cleared
	bool dirty = false;

	Logic_Value state = Logic_Value::HI_Z;
	std::set<uint32_t> input_junction_indexes;
	std::set<uint32_t> output_junction_indexes;
	std::set<uint32_t> wire_indexes;
};

class Gate {
public:
	// A gate cannot exist without a logicFunction and it's junctions
	Gate() = delete;

	// Pass the gate the logic function it should use, it's inputs and output junction.
	Gate(const Logic_Value(*logicFunc)(std::vector<Logic_Value>), std::vector<uint32_t> input_junction_indexes, uint32_t output_junction_index) :
		logicFunction(logicFunc),
		input_junction_indexes(input_junction_indexes),
		output_junction_index(output_junction_index)
		{}

	// index 0 is LSB
	const std::vector<uint32_t> input_junction_indexes;
		
	// one bit output junction
	const uint32_t output_junction_index;

	// Function pointer to a function that takes a vector of inputs and returns
	// a single output
	const Logic_Value(*logicFunction)(std::vector<Logic_Value>);
};

// A GUI_Junction is a set of Circuit junctions, often four, which have internal wires connecting them all.
struct GUI_Junction {
	// A GUI_Junction cannot exist without some number of connections
	GUI_Junction() = delete;

	// Pass the number of connections, cannot be zero. Also pass wires connecting all the junctions together
	// TODO factory method?
	GUI_Junction(std::vector<uint32_t> junction_indexes, std::vector<uint32_t> internal_wire_indexes);

	const std::vector<uint32_t> junction_indexes;
	const std::vector<uint32_t> internal_wire_indexes;
};

struct Circuit_Data {
	std::vector<Gate*> gates;
	std::vector<Junction*> junctions;
	std::vector<Network*> networks;
	std::vector<Wire*> wires;
};

/**
* Add x input junctions to circuit and return their indexes
*/
std::vector<uint32_t> add_x_input_junctions(uint32_t x, Circuit_Data& data);

/**
* Add x output junctions to circuit and return their indexes
*/
std::vector<uint32_t> add_x_output_junctions(uint32_t x, Circuit_Data& data);

/**
* Remove junction
* 
* Returns indexes of consequently deleted wires.
*/
std::vector<uint32_t> remove_junction(uint32_t index, Circuit_Data& data);


// Add a gate, returns it's index.
uint32_t add_gate(uint32_t n_inputs, Gates type, Circuit_Data& data);

/**
* Remove gate by index.
* 
* Returns indexes of consequently deleted wires.
*/
std::vector<uint32_t> remove_gate(uint32_t index, Circuit_Data& data);

/**
* Add wire, returns wire's index.
*/
uint32_t add_wire(std::set<uint32_t> wire_indexes, Circuit_Data& data);

/**
* Remove wire by index
* 
* Will dirty it's network.
* 
* TODO: remove_junction also dirties network and it calls this function
* provide a version that doesn't duplicate that work?
*/
void remove_wire(uint32_t index, Circuit_Data& data);


void process_gates(Circuit_Data& data);

// Return the indexes of every network with a new state.
std::vector<uint32_t> process_networks(Circuit_Data& data);
