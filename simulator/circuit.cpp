#include "circuit.hpp"
#include <string>

std::vector<Event> Circuit::simulate(const std::vector<Event>& n_plus_1) {

	std::vector<Event> output_e;
	
	for (uint32_t i = 0; i < n_plus_1.size(); i++) {
		auto e = this->process(n_plus_1[i]);
		if (e.has_value()) output_e.push_back(e.value());
	}

	return output_e;
}

std::optional<Event> Circuit::process(const Event e)
{
	switch (e.action) {
	case (Action::Add): {
		if (is_gate(e.entity_type)) return add_gate(e, data);
		else return error_event("Unimplemented Add: " + to_string(e.entity_type), e);
	}
	case(Action::Remove): {
		if (is_gate(e.entity_type)) return remove_gate(e, data);
		else return error_event("Unimplemented Remove: " + to_string(e.entity_type), e);
	}
	default:
		return std::nullopt;
	}
}
