#pragma once
#include <vector>
#include <set>
#include "logic.hpp"
#include <queue>
#include <tuple>
#include "event.hpp"

class Network {
public:
	// Get the state of the network
	const Logic_Value get_state() const { return state; }

	bool has_junction(junction_ref j) const;

	Logic_Value state;
	std::set<junction_ref> inputs;
	std::set<junction_ref> outputs;
	std::set<wire_ref> wires;
};

// A wire is the "line" between two junction "points"
struct Wire {

	// Default constructor leaves object in invalid state so no default constructor
	Wire() = delete;

	// Construct a wire between two junctions.
	// Wire manages relationship with junctions from construction through destruction.
	Wire(junction_ref a, junction_ref b) : j1(a), j2(b) {};

	~Wire() {}

	const junction_ref j1;
	const junction_ref j2;
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
	const Logic_Value get_state() const {
		if (stateFunction == nullptr) return Logic_Value::HI_Z;
		else return stateFunction();
	}

	// Set state function
	void set_state_function(const Logic_Value(*stateFunc)()) {
		stateFunction = stateFunc;
	}

private:
	// Using a function ptr so Inputs doesn't need to have an awareness of networks.
	const Logic_Value(*stateFunction)();
};

const struct Gate {
	// A gate cannot exist without a logicFunction and it's junctions
	Gate() = delete;

	// Pass the gate the logic function it should use, it's inputs and output junction(s).
	Gate(const Logic_Value(*logicFunc)(std::vector<Logic_Value>), std::vector<junction_ref> in, junction_ref out) :
		logicFunction(logicFunc),
		inputs(in),
		output(out)
		{}

	// index 0 is LSB
	const std::vector<junction_ref> inputs;
		
	// one bit output junction
	const junction_ref output;

	// Function pointer to a function that takes a vector of inputs and returns
	// a single output
	const Logic_Value(*logicFunction)(std::vector<Logic_Value>);
};

// A GUI_Junction is a set of Circuit junctions, often four, which have internal wires connecting them all.
const struct GUI_Junction {
	// A GUI_Junction cannot exist without some number of connections
	GUI_Junction() = delete;

	// Pass the number of connections, cannot be zero. Also pass wires connecting all the junctions together
	// TODO factory method?
	GUI_Junction(std::vector<junction_ref> connections, std::vector<wire_ref> wires) : connections(connections), internal_wires(wires) {}

	const std::vector<junction_ref> connections;
	const std::vector<wire_ref> internal_wires;
};
