#include "network.hpp"

// Take the difference between two sets
// for some reason not built into C++?
// This is not commutative.
std::set<uint32_t> difference(std::set<uint32_t>& a, std::set<uint32_t>& b) {
	std::set<uint32_t> ret;
	for (auto iter = a.begin(); iter != a.end(); iter++) {
		if (b.find(*iter) == b.end()) ret.insert(*iter);
	}

	return ret;
}


void remove_wire_from_network(uint32_t wire_index, uint32_t network_index, Circuit_Data& data) {
	// When removing a wire from a network, the network may be destroyed, reduced in size, split into 1..n more networks, or no effect on integrity.
	// Yes, 1..n, not just 2, because one wire connecting a set of junctions could have multiple satellite networks it's joining.
	// Unusual, but possible.

	// If this is the last wire in the network, the network no longer exists
	if (data.networks[network_index]->wire_indexes.size() == 1) {
		delete data.networks[network_index];
		data.networks[network_index] = nullptr;
	}
	// Else remove me from network and recompose it into network(s)
	else {
		// Remove the wire from the network.
		data.networks[network_index]->wire_indexes.erase(wire_index);

		// Get all the junctions the late wire had connection to.
		auto late_wire_js = data.wires[wire_index]->junction_indexes;

		// Get all junctions that late wire had which the network has no access to.
		// Those are either solitary junctions or belong to new network(s).
		std::set<uint32_t> late_wire_unique_js =
			difference(difference(late_wire_js, data.networks[network_index]->input_junction_indexes), data.networks[network_index]->input_junction_indexes);

		// for all unique junctions, we'll need to crawl the junctions in the larger circuit to find 
	}



	// Remove myself from network
	// If I was the last wire, delete network,
	// else:
	// Tell network to see if any junctions which I connected it too are now disconnected from it
	// If so, notify junction it has no network index (if an input junction)
}