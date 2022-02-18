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

Output::Output() : driven_state(Logic_Value::HI_Z), Junction(Type::Output) {};

Input::Input() : Junction(Type::Input) {}

GUI_Junction::GUI_Junction(std::vector<uint32_t> junction_indexes, std::vector<uint32_t> internal_wire_indexes) : 
	junction_indexes(junction_indexes), 
	internal_wire_indexes(internal_wire_indexes) 
{}

/**
* Add x input junctions to circuit and return their indexes
*/
std::vector<uint32_t> add_x_input_junctions(uint32_t x, Circuit_Data& data) {
	std::vector<uint32_t> ret;
	for (uint32_t i = 0; i < x; i++) {
		data.junctions.push_back(new Input());
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
		data.junctions.push_back(new Output());
		ret.push_back(data.junctions.size());
	}
	return ret;
}

std::vector<uint32_t> remove_junction(uint32_t index, Circuit_Data& data)
{
	std::vector<uint32_t> deleted_wires;

	// Delete my wires
	std::vector<uint32_t> my_wires;

	for (uint32_t i = 0; i < data.wires.size(); i++) {
		if (data.wires[i]->has_junction(index)) my_wires.push_back(i);
	}

	for (uint32_t i = 0; i < my_wires.size(); i++) {
		remove_wire(my_wires[i], data);
		deleted_wires.push_back(i);
	}
		
	// Do not need to concern with updating any networks since they will 
	// have been affected when wires were deleted. If no wires were connected
	// To junction, then junction wasn't part of a network. Again, no worries.

	// Delete the junction
	delete data.junctions[index];
	data.junctions[index] = nullptr;

	return deleted_wires;
}

uint32_t add_gate(uint32_t n_inputs, Gates type, Circuit_Data& data)
{
	std::vector<uint32_t> input_js = add_x_input_junctions(n_inputs, data);
	std::vector<uint32_t> output_js = add_x_output_junctions(1, data);

	switch (type) {
	case(Gates::AND):
		data.gates.push_back(new Gate(logic::AND, input_js, output_js[0]));
		break;
	case(Gates::NAND):
		data.gates.push_back(new Gate(logic::NAND, input_js, output_js[0]));
		break;
	case(Gates::OR):
		data.gates.push_back(new Gate(logic::OR, input_js, output_js[0]));
		break;
	case(Gates::NOR):
		data.gates.push_back(new Gate(logic::NOR, input_js, output_js[0]));
		break;
	case(Gates::XOR):
		data.gates.push_back(new Gate(logic::XOR, input_js, output_js[0]));
		break;
	}

	// Return the index of the new gate (which is the end of the vector)
	return data.gates.size() - 1;
}


std::vector<uint32_t> remove_gate(uint32_t index, Circuit_Data& data)
{
	
	std::vector<uint32_t> junctions_to_delete = data.gates[index]->input_junction_indexes;
	junctions_to_delete.push_back(data.gates[index]->output_junction_index);

	std::vector<uint32_t> deleted_wire_indexes;
	while (!junctions_to_delete.empty()) {
		uint32_t index = junctions_to_delete.back();
		junctions_to_delete.pop_back();

		auto v = remove_junction(index, data);
		deleted_wire_indexes.insert(deleted_wire_indexes.begin(), v.begin(), v.end());
	}

	delete data.gates[index];
	data.gates[index] = nullptr;

	return deleted_wire_indexes;
}

void add_junction_to_network(uint32_t junction_index, uint32_t network_index, Circuit_Data& data) {	
	auto type = data.junctions[junction_index]->get_type();

	if (type == Junction::Type::Input) {
		// Add junction index to network as input junction
		data.networks[network_index]->input_junction_indexes.insert(junction_index);

		// Add network pointer to input junction so input junction can find it's state.
		Junction* j_ptr = data.junctions[junction_index];
		Input* input_j_ptr = static_cast<Input*>(j_ptr);
		input_j_ptr->network_index = network_index;
	}

	else {
		// Add junction index to network as input junction
		data.networks[network_index]->output_junction_indexes.insert(junction_index);
	}
}

void add_wire_to_network(uint32_t wire_index, uint32_t network_index, Circuit_Data& data)
{


}

uint32_t add_wire(std::set<uint32_t> wire_indexes, Circuit_Data& data)
{
	//// Create the wire
	//data.wires.push_back(new Wire{ wire_indexes });

	//// Get the index of the wire
	//uint32_t wire_index = data.wires.size() - 1;

	//// Add it to it's network(s) and mark network(s) as dirty (find network via junctions)
	//bool network_found = false;
	//for (uint32_t i = 0; i < data.networks.size(); i++) {
	//	if (data.networks[i] != nullptr) {
	//		Network* net = data.networks[i];
	//		if (net->has_junction(j_index_a) || net->has_junction(j_index_b)) {
	//			net->wire_indexes.insert(wire_index);
	//			add_junction_to_network(j_index_a, i, data); // one of these at least
	//			add_junction_to_network(j_index_b, i, data); // will have no effect
	//			// data.wires.push_back(j_index_a)
	//			net->dirty = true;
	//			network_found = true;
	//		}
	//	}
	//}

	//// Junctions were part of no pre-existing networks so create new network.
	//if (network_found == false) {
	//	// Create new network
	//	data.networks.push_back(new Network());

	//	// Get that index
	//	uint32_t network_index = data.networks.size() - 1;

	//	// Add it's wire
	//	data.networks[network_index]->wire_indexes.insert(wire_index);

	//	// Give it its junctions
	//	add_junction_to_network(j_index_a, network_index, data);
	//	add_junction_to_network(j_index_b, network_index, data);
	//	
	//}

	//return wire_index;
	return 0;
}

void remove_wire_from_network(uint32_t wire_index, uint32_t network_index, Circuit_Data& data) {
	// Remove myself from network
	// If I was the last wire, delete network,
	// else:
	// Tell network to see if any junctions which I connected it too are now disconnected from it
	// If so, notify junction it has no network index (if an input junction)
}

void remove_wire(uint32_t index, Circuit_Data& data)
{
	// Remove myself from my network
	for (uint32_t i = i; i < data.networks.size(); i++) {
		if (data.networks[i]->has_wire(index)) {
			remove_wire_from_network(index, i, data);
			break;
		}
	}

	// Delete myself from the circuit data
	delete data.wires[index];
	data.wires[index] = nullptr;
}

std::vector<uint32_t> process_networks(Circuit_Data& data)
{
	std::vector<uint32_t> networks_with_state_change;
	// For every network, 
		// Find new state
		// if new state != old state
			// push my index onto networks_with_state_change
		// Set network.state = new_state
	//return 
	return std::vector<uint32_t>();
}

void process_gates(Circuit_Data& data)
{
	// for (uint32_t i = 0; i < data.gates.size())
	
	// for every gate
	// turn vector of input junctions into input value vector
	// pass to logic function
	// set output junction value to result of logic function.
}
