#include "event.hpp"

std::string to_string(Entities e)
{
	switch (e) {
	case(Entities::AND):
		return "AND";
	case(Entities::GUI_JUNCTION):
		return "GUI_JUNCTION";
	case(Entities::NAND):
		return "NAND";
	case(Entities::OR):
		return "OR";
	case(Entities::NOR):
		return "NOR";
	case(Entities::XOR):
		return "XOR";
	case(Entities::NOT):
		return "NOT";
	case(Entities::WIRE):
		return "WIRE";
	default:
		return "Unimplemented: to_string";
	}
}


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