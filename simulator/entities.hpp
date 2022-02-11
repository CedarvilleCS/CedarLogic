#pragma once

#include <vector>
#include <set>
#include "logic.hpp"
#include "event.hpp"
#include <queue>
#include <tuple>
#include <string>
#include <cassert>
#include <memory>

// A wire is the "line" between two junction "points"
struct Wire {

	// Default constructor leaves object in invalid state so no default constructor
	Wire() = delete;

	// Construct a wire between two junctions.
	// Wire manages relationship with junctions from construction through destruction.
	Wire(uint32_t junction_index_a, uint32_t junction_index_b);

	~Wire() {}

	const uint32_t junction_index_a;
	const uint32_t junction_index_b;
};

// This is the only place where you can connect and disconnect wires.
// Except, if a wire disappears, we need to know if the network is still sound.
class Junction {
public:
	// Get the logic value of this junction
	virtual const Logic_Value get_state() const = 0;
};

// An output is a junction with it's own driven state.
class Output : public Junction {
public:

	// Get the logic value of this junction
	const Logic_Value get_state() const { return driven_state; };

	// TODO: make it so only the entity who owns the output can change this.
	// TODO: likely will guarantee by circuit construction
	void set_state(Logic_Value val) { driven_state = val; };

private:
	Logic_Value driven_state;
};

// An input is a junction who's state is driven.
class Input : public Junction {
public:

	// Inputs must know what to call to get their state.
	Input() : stateFunction(nullptr) {};

	// Pass me the function I should call when I need to know my state
	Input(const Logic_Value(*stateFunc)()) : stateFunction(stateFunc) {};

	// get state
	const Logic_Value get_state() const;

	// Set state function
	void set_state_function(const Logic_Value(*stateFunc)());

private:
	// Using a function ptr so Inputs doesn't need to have an awareness of networks.
	const Logic_Value(*stateFunction)();
};

class Network {
public:
	// Get the state of the network
	const Logic_Value get_state() const { return state; }

	bool has_junction(uint32_t junction_index) const;

	// One of it's junctions has been deleted since last this was cleared
	bool dirty = false;

	Logic_Value state;
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
	std::vector< std::unique_ptr<Gate>> gates;
	std::vector< std::unique_ptr<GUI_Junction>> gui_junctions;
	std::vector< std::unique_ptr<Junction>> junctions;
	std::set< std::unique_ptr<Network>> networks;
	std::vector<std::unique_ptr<Wire>> wires;
};

// Create error event with given string and original event
Event error_event(const std::string& err, const Event e);

/**
* Create Added event with index and original event
*/
Event added_event(uint32_t index, const Event e);

/**
* Create Removed event with index and original event
*/
Event removed_event(const Event e);

/**
* Add x input junctions to circuit and return their indexes
*/
std::vector<uint32_t> add_x_input_junctions(uint32_t x, Circuit_Data& data);

/**
* Add x output junctions to circuit and return their indexes
*/
std::vector<uint32_t> add_x_output_junctions(uint32_t x, Circuit_Data& data);

/**
* Return whether or not the given entity is a kind of gate.
*/
bool is_gate(Entities e);

// Add a gate to the given data from an event and return an event
Event add_gate(Event e, Circuit_Data& data);

// Remove the gate and all it's subcomponents
Event remove_gate(Event e, Circuit_Data& data);

