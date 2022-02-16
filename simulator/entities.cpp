#include "entities.hpp"

bool Network::has_junction(uint32_t j) const
{
	return output_junction_indexes.find(j) != output_junction_indexes.end() ||
		input_junction_indexes.find(j) != input_junction_indexes.end();
}

bool Network::has_wire(uint32_t wire_index) const
{
	return wire_indexes.find(wire_index) != wire_indexes.end();
}

Wire::Wire(uint32_t junction_index_a, uint32_t junction_index_b) :
	junction_index_a(junction_index_a),
	junction_index_b(junction_index_b)
{}
bool Wire::has_junction(uint32_t index) const
{
	return index == junction_index_a || index == junction_index_b;
}

Output::Output() : driven_state(Logic_Value::HI_Z) {};

const Logic_Value Input::get_state() const {
	if (stateFunction == nullptr) return Logic_Value::HI_Z;
	else return stateFunction();
}

void Input::set_state_function(const Logic_Value(*stateFunc)()) {
	stateFunction = stateFunc;
}

GUI_Junction::GUI_Junction(std::vector<uint32_t> junction_indexes, std::vector<uint32_t> internal_wire_indexes) : 
	junction_indexes(junction_indexes), 
	internal_wire_indexes(internal_wire_indexes) 
{}

Event error_event(const std::string& err, const Event e) {
	Event ret(e);
	ret.err = err;
	ret.action = Action::Error;
	return ret;
}

Event added_event(uint32_t index, const Event e) {
	Event ret(e);
	ret.circuit_ref = index;
	ret.action = Action::Added;
	ret.err.clear();
	return ret;
}

Event removed_event(const Event e)
{
	Event ret(e);
	ret.action = Action::Removed;
	ret.err.clear();
	return ret;
}

/**
* Add x input junctions to circuit and return their indexes
*/
std::vector<uint32_t> add_x_input_junctions(uint32_t x, Circuit_Data& data) {
	std::vector<uint32_t> ret;
	for (uint32_t i = 0; i < x; i++) {
		data.junctions.push_back(std::make_unique<Input>());
		ret.push_back(data.junctions.size());
	}
	return ret;
}

/**
* Add x output junctions to circuit and return their indexes
*/
std::vector<uint32_t> add_x_output_junctions(uint32_t x, Circuit_Data& data) {
	std::vector<uint32_t> ret;
	for (uint32_t i = 0; i < x; i++) {
		data.junctions.push_back(std::make_unique<Output>());
		ret.push_back(data.junctions.size());
	}
	return ret;
}

std::vector<Event> remove_junction(uint32_t index, Circuit_Data& data)
{
	std::vector<Event> resulting_events;

	// Delete my wires
	std::vector<uint32_t> my_wires;

	for (uint32_t i = 0; i < data.wires.size(); i++) {
		if (data.wires[i]->has_junction(index)) my_wires.push_back(i);
	}

	for (uint32_t i = 0; i < my_wires.size(); i++) {
		Event e = remove_wire(my_wires[i], data);
		resulting_events.push_back(e);
	}
		
	// Dirty my network so it get's rebuilt at the end of this frame.
	for (auto iter = data.networks.begin(); iter != data.networks.end(); iter++)
	{
		if ((*iter)->has_junction(index)) {
			(*iter)->dirty = true;
			break;
		}
	}

	// Delete the junction
	data.junctions[index].reset();

	return resulting_events;
}

/**
* Return whether or not the given entity is a kind of gate.
*/
bool is_gate(Entities e) {
	switch (e) {
	case(Entities::AND):
	case(Entities::NAND):
	case(Entities::OR):
	case(Entities::NOR):
	case(Entities::XOR):
		return true;
	default:
		return false;
	}
}

Event add_gate(Event e, Circuit_Data& data) {
	assert(is_gate(e.entity_type));
	if (e.n_outputs != 1) return error_event("n_ouptuts must be == 1", e);
	if (e.n_inputs < 2) return error_event("n_inputs must be > 1", e);

	std::vector<uint32_t> input_js = add_x_input_junctions(e.n_inputs, data);
	std::vector<uint32_t> output_js = add_x_output_junctions(1, data);

	switch (e.entity_type) {
	case(Entities::AND):
		data.gates.push_back(std::make_unique < Gate>(logic::AND, input_js, output_js[0]));
		break;
	case(Entities::NAND):
		data.gates.push_back(std::make_unique < Gate>(logic::NAND, input_js, output_js[0]));
		break;
	case(Entities::OR):
		data.gates.push_back(std::make_unique < Gate>(logic::OR, input_js, output_js[0]));
		break;
	case(Entities::NOR):
		data.gates.push_back(std::make_unique<Gate>(logic::NOR, input_js, output_js[0]));
		break;
	case(Entities::XOR):
		data.gates.push_back(std::make_unique < Gate>(logic::XOR, input_js, output_js[0]));
		break;
	}
	return added_event(data.gates.size() - 1, e);
}

std::vector<Event> remove_gate(Event e, Circuit_Data& data) {
	assert(is_gate(e.entity_type));

	if (e.circuit_ref >= data.gates.size()) {
		return std::vector<Event>{error_event("Gate does not exist.", e)};
	}

	auto v = remove_gate(e.circuit_ref, data);

	Event ret_e;
	ret_e.action = Action::Removed;
	ret_e.entity_type = e.entity_type;
	ret_e.circuit_ref = e.circuit_ref;
	v.push_back(ret_e);
	return v;
}

std::vector<Event> remove_gate(uint32_t index, Circuit_Data& data)
{
	std::vector<Event> resulting_events;

	std::vector<uint32_t> junctions_to_delete = data.gates[index]->input_junction_indexes;
	junctions_to_delete.push_back(data.gates[index]->output_junction_index);

	while (!junctions_to_delete.empty()) {
		uint32_t index = junctions_to_delete.back();
		junctions_to_delete.pop_back();

		std::vector<Event> events = remove_junction(index, data);
		resulting_events.insert(resulting_events.end(), events.begin(), events.end());
	}

	data.gates[index].reset();

	return resulting_events;
}

uint32_t add_wire(uint32_t j_index_a, uint32_t j_index_b, Circuit_Data& data)
{
	// Create the wire
	data.wires.push_back(std::make_unique<Wire>(j_index_a, j_index_b));

	// Get the index of the wire
	uint32_t index = data.wires.size() - 1;

	// Add it to it's network(s) and mark network(s) as dirty (likely a merge)
	for (auto iter = data.networks.begin(); iter != data.networks.end(); iter++) {
		if ((*iter)->has_junction(j_index_a) || (*iter)->has_junction(j_index_b)) {
			(*iter)->dirty = true;
			(*iter)->wire_indexes.insert(index);
		}
	}

	return index;
}

Event remove_wire(Event e, Circuit_Data& data)
{
	assert(e.entity_type == Entities::WIRE);

	if (e.circuit_ref >= data.wires.size()) {
		return error_event("Gate does not exist.", e);
	}

	return remove_wire(e.circuit_ref, data);
}

Event remove_wire(uint32_t index, Circuit_Data& data)
{
	// Remove myself from my network
	for (auto iter = data.networks.begin(); iter != data.networks.end(); iter++) {
		if ((*iter)->has_wire(index)) {
			(*iter)->wire_indexes.erase(index);
			break;
		}
	}

	// Delete myself from the circuit data
	data.wires[index].reset();

	Event e;
	e.action = Action::Removed;
	e.entity_type = Entities::WIRE;
	e.circuit_ref = index;
	return e;
}
