#pragma once

#include <logic.hpp>
#include <optional>
#include <stdint.h>
#include <string>

enum class Entities { AND, OR, XOR, NAND, NOR, NOT, WIRE, GUI_JUNCTION };

std::string to_string(Entities e);

enum class Action { Add, Remove, Update, Added, Removed, Updated, Error };

struct Event {
  // Entity type
  Entities entity_type;

  // Request ID
  uint32_t req_id;

  // Circuit reference
  uint32_t circuit_ref;

  Action action;

  // Logic state
  Logic_Value state;

  uint8_t n_inputs;

  uint8_t n_outputs;

  std::string err;
};