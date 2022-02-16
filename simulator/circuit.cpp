#include "circuit.hpp"
#include <string>

std::vector<Event> Circuit::simulate(const std::vector<Event>& n_plus_1) {

	std::vector<Event> output_e;
	
	for (uint32_t i = 0; i < n_plus_1.size(); i++) {
		auto v = this->process(n_plus_1[i]);
		output_e.insert(output_e.end(), v.begin(), v.end());
	}

	return output_e;
}

std::vector<Event> Circuit::process(const Event e)
{
	switch (e.action) {
	case (Action::Add): {
		if (is_gate(e.entity_type)) return std::vector<Event>{add_gate(e, data)};
		else return std::vector<Event>{error_event("Unimplemented Add: " + to_string(e.entity_type), e)};
	}
	case(Action::Remove): {
		if (is_gate(e.entity_type)) return remove_gate(e, data);
		else return std::vector<Event>{error_event("Unimplemented Remove: " + to_string(e.entity_type), e)};
	}
	default:
		return std::vector<Event>();
	}
}
