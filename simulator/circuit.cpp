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
  // Set all output junctions given their input junction values
  process_gates();

  // Process all the incoming user-events

  // Get all the networks with a state change so we can create events with the
  // new state for each internal wire.
  auto v = process_networks();

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
