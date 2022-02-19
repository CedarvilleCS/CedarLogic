#pragma once

#include <stdint.h>
#include <optional>
#include <logic.hpp>
#include <string>

enum class Entities {
	AND, OR, XOR, NAND, NOR, NOT, WIRE, GUI_JUNCTION
};

std::string to_string(Entities e);

typedef uint32_t req_id;	// An incoming request ID, used in response requests
typedef uint32_t reference; // Reference to a circuit object instance

enum class Action {
	Add, Remove, Update, Added, Removed, Updated, Error
};

struct Event {
	// Entity type
	Entities entity_type;

	// Request ID
	req_id req_id;

	// Circuit reference
	reference circuit_ref;

	Action action;

	// Logic state
	Logic_Value state;

	uint8_t n_inputs;

	uint8_t n_outputs;

	std::string err;
};