#include "circuit.hpp"
#include <string>

/**
 * Return whether or not the given entity is a kind of gate.
 */
bool is_gate(Entities e) {
  switch (e) {
  case (Entities::AND):
  case (Entities::NAND):
  case (Entities::OR):
  case (Entities::NOR):
  case (Entities::XOR):
    return true;
  default:
    return false;
  }
}

std::vector<Event> Circuit::simulate(const std::vector<Event> &n_plus_1) {
  
  /* Add/remove everything the user asked for since last loop */

  /* Tell every entity in the circuit to update it's output j */
  process_gates();

  /* Get every wire with a change state and that wire's new state */
  std::vector<std::pair<uint32_t, Logic_Value>> v = networks.step();

  /* Turn all the updated wire pairs into update events */

  /* Combine updated wire events and added/removed events */

  /* Return all events from this step to the GUI */
  return std::vector<Event>(); 
}

std::pair<bool, std::vector<uint32_t>> Circuit::delete_gate(uint32_t index) {
  // the order of this boolean check matters to prevent segfault
  if (gates.size() > index && gates[index] != nullptr) {
    auto ret = std::make_pair(true, std::vector<uint32_t>());

    /* Delete the input junctions and save IDs of any deleted wires. */
    auto in_js = gates[index]->get_input_j_ids();
    for (uint i = 0; i < in_js.size(); i++) {
      auto v = this->delete_junction(in_js[i]).second;
      ret.second.insert(ret.second.end(), v.begin(), v.end());
    }

    /* Delete the output junctions and save IDs of any deleted wires. */
    auto v = this->delete_junction(gates[index]->get_output_j_id()).second;
    ret.second.insert(ret.second.end(), v.begin(), v.end());

    /* Delete the gate itself */
    delete gates[index];
    gates[index] = nullptr;

    /* return statement that gate existed and vector of deleted wire indexes. */
    return ret;
  } else {
    /* return statement that gate did not exist. */
    return std::make_pair(false, std::vector<uint32_t>());
  }
}

std::pair<bool, std::vector<uint32_t>>
Circuit::delete_junction(uint32_t index) {
  if (junctions.size() > index && junctions[index] != nullptr) {
    auto ret = std::make_pair(true, std::vector<uint32_t>());

    /* get all wires with only two junctions where this is one of them */
    std::vector<uint32_t> connected_wires_two_js;
    for (uint i = 0; i < wires.size(); i++) {
      if (wires[i]->has_junction(junctions[index]) &&
          wires[i]->junction_ptrs.size() <= 2) {
        connected_wires_two_js.push_back(i);
      }
    }

    /* Delete those wires since every wire must have at least 2 junctions */
    this->delete_wires(connected_wires_two_js);

    /* Notify the networks that this junction will soon be deleted. */
    networks.deleting_junction(junctions[index]);

    /* Delete the junction itself */
    delete junctions[index];
    junctions[index] = nullptr;

	/* Return the result */
	return ret;
  } else {
    /* Junction did not exist */
    return std::make_pair(false, std::vector<uint32_t>());
  }
}

void Circuit::delete_wires(std::vector<uint32_t> indexes) {
  /* Just loop through the vector and delete them individually. */
  for (uint32_t i = 0; i < indexes.size(); i++) {
    this->delete_wire(indexes[i]);
  }
}

bool Circuit::delete_wire(uint32_t index) {
  if (wires.size() > index && wires[index] != nullptr) {
    /* Notify the networks this wire sill soon be deleted. */
    networks.deleting_wire(wires[index]);

    /* Delete the wire itself */
    delete wires[index];
    wires[index] = nullptr;

    /* return that the wire existed */
    return true;
  } else {
    /* return that the wire did not exist */
    return false;
  }
}

uint32_t Circuit::new_gate(uint32_t n_inputs, Gates type) {
  /* Get the input and output junctions for the gate */
  auto i_v = this->new_input_junctions(n_inputs);
  Output *o_j =
      static_cast<Output *>(junctions[this->new_output_junctions(1)[0]]);

  /* Turn the input indexes into ptrs */
  std::vector<Input *> i_v_ptrs;
  for (uint32_t i = 0; i < i_v.size(); i++) {
	auto index = i_v[i];
    i_v_ptrs.push_back(static_cast<Input *>(junctions[index]));
  }

  /* Create the gate itself with the junctions as parameters */
  switch (type) {
  case Gates::AND:
    gates.push_back(new GateAND(i_v_ptrs, o_j));
    break;
  case Gates::XOR:
    gates.push_back(new GateAND(i_v_ptrs, o_j));
    break;
  case Gates::OR:
    gates.push_back(new GateAND(i_v_ptrs, o_j));
    break;
  case Gates::NAND:
    gates.push_back(new GateAND(i_v_ptrs, o_j));
    break;
  case Gates::NOR:
    gates.push_back(new GateAND(i_v_ptrs, o_j));
    break;
  }

  /* Return the gate's index */
  return gates.size() - 1;
}

std::vector<uint32_t> Circuit::new_input_junctions(uint32_t x) {
	std::vector<uint32_t> ret;
	for (uint32_t i = 0; i < x; i++) {
		auto index = junctions.size();
		junctions.push_back(new Input(index));
		ret.push_back(index);
	}
	return ret;
}

std::vector<uint32_t> Circuit::new_output_junctions(uint32_t x) {
	std::vector<uint32_t> ret;
	for (uint32_t i = 0; i < x; i++) {
		auto index = junctions.size();
		junctions.push_back(new Output(index));
		ret.push_back(index);
	}
	return ret;
}

uint32_t Circuit::new_wire(std::vector<uint32_t> junction_indexes) {
    /* Create the wire instance */
	uint32_t index = wires.size();
	Wire* w = new Wire(index);

	/* Add all the wire's junctions */
	for (uint32_t i = 0; i < junction_indexes.size(); i++) {
		auto index = junction_indexes[i];
		auto ptr = junctions[index];
		w->junction_ptrs.insert(ptr);
	}

	/* Notify network of new wire so it can update it's internal networks. */
	networks.created_wire(w);

	/* Store it and return the index. */
	wires.push_back(w);
	return index;
}

void Circuit::process_gates() {
	for (uint i = 0; i < gates.size(); i++) {
		if (gates[i] != nullptr) { // filter out the deleted gates
			gates[i]->process();
		}
	}
}
