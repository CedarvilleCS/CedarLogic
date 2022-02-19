#include "circuit.hpp"
#include <string>

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

std::vector<Event> Circuit::simulate(const std::vector<Event>& n_plus_1) {
	// Set all output junctions given their input junction values
	process_gates(data);

	// Process all the incoming user-events

	// Get all the networks with a state change so we can create events with the new state for each
	// internal wire.
	auto v = process_networks(data);

	return std::vector<Event>();
}


//std::vector<event> circuit::process(const event e)
//{
//	switch (e.action) {
//	case (action::add): {
//		if (is_gate(e.entity_type)) return std::vector<event>{add_gate(e, data)};
//		else return std::vector<event>{error_event("unimplemented add: " + to_string(e.entity_type), e)};
//	}
//	case(action::remove): {
//		if (is_gate(e.entity_type)) return remove_gate(e, data);
//		else return std::vector<event>{error_event("unimplemented remove: " + to_string(e.entity_type), e)};
//	}
//	default:
//		return std::vector<event>();
//	}
//}


//
//Event add_gate(Event e, Circuit_Data& data) {
//	assert(is_gate(e.entity_type));
//	if (e.n_outputs != 1) return error_event("n_ouptuts must be == 1", e);
//	if (e.n_inputs < 2) return error_event("n_inputs must be > 1", e);
//
//	std::vector<uint32_t> input_js = add_x_input_junctions(e.n_inputs, data);
//	std::vector<uint32_t> output_js = add_x_output_junctions(1, data);
//
//	switch (e.entity_type) {
//	case(Entities::AND):
//		data.gates.push_back(std::make_unique < Gate>(logic::AND, input_js, output_js[0]));
//		break;
//	case(Entities::NAND):
//		data.gates.push_back(std::make_unique < Gate>(logic::NAND, input_js, output_js[0]));
//		break;
//	case(Entities::OR):
//		data.gates.push_back(std::make_unique < Gate>(logic::OR, input_js, output_js[0]));
//		break;
//	case(Entities::NOR):
//		data.gates.push_back(std::make_unique<Gate>(logic::NOR, input_js, output_js[0]));
//		break;
//	case(Entities::XOR):
//		data.gates.push_back(std::make_unique < Gate>(logic::XOR, input_js, output_js[0]));
//		break;
//	}
//	return added_event(data.gates.size() - 1, e);
//}
