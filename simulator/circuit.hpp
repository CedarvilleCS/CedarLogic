#pragma once

#include "event.hpp"
#include "gate.hpp"
#include "junction.hpp"
#include "network.hpp"
#include "wire.hpp"
#include <memory>
#include <set>
#include <stdint.h>
#include <vector>

/**
 * @brief The circuit class simulates and maintains the circuit.
 *
 * @note Circuit may never expose a pointer. A uint32_t id number is fine but
 * it's a weak reference.
 * @note Circuit may never throw an exception.
 * @note Circuit may not be made to respond in real-time to a function call.
 * Rather, everything is message queue based.
 * @note Circuit is entirely responsible for it's own state and memory
 * management.
 */
class Circuit {
public:
  /**
   * Simulate the circuit for one frame. The main function or simulation class
   * (whoever owns this) will manage ticking and passing events between
   * producers and consumers.
   *
   * @param[in]	n_plus_1	Set of events belonging to frame N+1 which will
   * now be simulated.
   *
   * @return	Displayable simulation events from input event set and circuit
   * state.
   */
  std::vector<Event> simulate(const std::vector<Event> &n_plus_1);

  /**
   * Simulate the circuit for one frame. The main function or simulation class
   * (whoever owns this) will manage ticking and passing events between
   * producers and consumers.
   *
   * @param[in]	circuit		A pointer to the circuit simulating these events
   * @param[in]	n_plus_1	Set of events belonging to frame N+1 which will
   * now be simulated. Recommend std::move()
   *
   * @return	Displayable simulation events from input event set and circuit
   * state.
   */
  static std::vector<Event> simulate(Circuit *circuit,
                                     const std::vector<Event> &n_plus_1) {
    return circuit->simulate(n_plus_1);
  }

  // Exposed for testing, may not leave so exposed.
  std::vector<Event> process(const Event e);

private:
  /**
   * @brief All the circuit's gates.
   *
   */
  std::vector<Gate *> gates;

  /**
   * @brief All the circuit's junctions.
   *
   * @note at the circuit level they aren't distinguished between Input and
   * Output.
   */
  std::vector<Junction *> junctions;

  /**
   * @brief All circuit's networks.
   *
   * @note They are encapsulated in unique_ptrs so it can be obvious when
   * ownership is transferred to Network's static functions which will new and
   * delete Network instances.
   *
   * @note They are held in a set rather than vector because they are created
   * and deleted more frequently than any other object and so a vector would be
   * expected to result in much more wasted space.
   */
  std::set<std::unique_ptr<Network>> networks;

  /**
   * @brief All the circuit's wires.
   *
   */
  std::vector<Wire *> wires;

  /**
   * @brief delete the gate identified by the index.
   *
   * @param index
   *
   * @return (bool, wire_indexes)
   * 		bool indicates if idetified junction existed prior to call.
   * 		wire_indexes are all those wires deleted in this call.
   *
   * @note will propogate function calls to remove all references to
   * gate in circuit, whether by index or by direct reference (ptr).
   *
   * @note will propogate function calls to ensure whole circuit is in a
   * good state before returning.
   *
   * @note will call delete_junction
   */
  std::pair<bool, std::vector<uint32_t>> delete_gate(uint32_t index);

  /**
   * @brief delete the junction identified by the index.
   *
   * @param index
   *
   * @return (bool, wire_indexes)
   * 		bool indicates if idetified junction existed prior to call.
   * 		wire_indexes are all those wires deleted in this call.
   *
   * @note will propogate function calls to remove all references to
   * junction in circuit, whether by index or by direct reference (ptr).
   *
   * @note will propogate function calls to ensure whole circuit is in a
   * good state before returning.
   *
   * @note may call delete_wire
   */
  std::pair<bool, std::vector<uint32_t>> delete_junction(uint32_t index);

  /**
   * @brief delete the wire identified by the index.
   *
   * @param index
   *
   * @return bool indicating if idetified wire existed prior to call.
   *
   * @note will propogate function calls to remove all references to
   * wire in circuit, whether by index or by direct reference (ptr).
   *
   * @note will propogate function calls to ensure whole circuit is in a
   * good state before returning.
   */
  bool delete_wire(uint32_t index);

  /**
   * @brief Constructs a gate
   *
   * @param n_inputs The number of inputs it has.
   * @param type The type of gate.
   * @return uint32_t The index of the gate.
   */
  uint32_t new_gate(uint32_t n_inputs, Gates type);

  /**
   * @brief Constructs x input junctions and return their identifiers
   *
   * @param x The number of input junctions to make
   * @return std::vector<uint32_t> new input junction identifiers
   */
  std::vector<uint32_t> new_input_junctions(uint32_t x);

  /**
   * @brief Constructs x output junctions and return their identifiers
   *
   * @param x The number of output junctions to make
   * @return std::vector<uint32_t> new output junction identifiers
   */
  std::vector<uint32_t> new_output_junctions(uint32_t x);

  /**
   * @brief Constructs a wire
   *
   * @param junction_indexes The junctions this wire connects to.
   * @return uint32_t The index of the wire.
   */
  uint32_t new_wire(std::set<uint32_t> junction_indexes);

  /**
   * @brief Execute every gate's process function.
   *
   * @note call this before process_networks()
   */
  void process_gates();

  /**
   * @brief Execute every network's refresh function and get every wire id
   * whose state changed (so we can update the GUI).
   *
   * @note only call this after every entity (such as the gates) have
   * updated their output junctions.
   *
   * @return std::vector<uint32_t> changed_wires_indexes
   */
  std::vector<uint32_t> process_networks();
};
