#include "entities.hpp"

bool Network::has_junction(uint32_t j) const
{
	return output_junction_indexes.find(j) != output_junction_indexes.end() ||
		input_junction_indexes.find(j) != input_junction_indexes.end();
}

Wire::Wire(uint32_t junction_index_a, uint32_t junction_index_b) :
	junction_index_a(junction_index_a),
	junction_index_b(junction_index_b)
{};

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
	return added_event(data.gates.size(), e);
}

void remove_junctions(std::vector<uint32_t> indexes, Circuit_Data& data) {
	while (!indexes.empty()) {
		uint32_t index = indexes.back();
		indexes.pop_back();
 
		for (auto iter = data.networks.begin(); iter != data.networks.end(); iter++)
		{
			if ((*iter)->has_junction(index)) {
				(*iter)->dirty = true;
				break;
			}
		}
		data.junctions[index].reset(); // Delete the junction
	}
}

Event remove_gate(Event e, Circuit_Data& data) {
	assert(is_gate(e.entity_type));

	auto ref = e.circuit_ref;
	if (ref >= data.gates.size()) return error_event("Gate does not exist.", e);

	std::vector<uint32_t> junctions_to_delete = data.gates[ref]->input_junction_indexes;
	junctions_to_delete.push_back(data.gates[ref]->output_junction_index);

	remove_junctions(junctions_to_delete, data);

	data.gates[ref].reset();

	return removed_event(e);
}