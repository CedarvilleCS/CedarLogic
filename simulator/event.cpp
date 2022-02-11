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
