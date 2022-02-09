#pragma once

#include <stdint.h>
#include <set>
#include <vector>
#include <memory>
#include <variant>
#include <optional>
#include "event.hpp"
#include "entities.hpp"

// This circuit assumes that the rebuild networks
// function will be called at the end of any time-step
// where anything was removed?

class Circuit {
public:

	/**
	* Simulate the circuit for one frame. The main function or simulation class (whoever owns this) will manage ticking
	* and passing events between producers and consumers.
	* 
	* @param[in]	n_plus_1	Set of events belonging to frame N+1 which will now be simulated.
	* 
	* @return	Displayable simulation events from input event set and circuit state.
	*/
	std::vector<Event> simulate(const std::vector<Event>& n_plus_1) {
		//// Entity type
		//Entities entity_type;

		//// Request ID
		//req_id req_id;

		//// Circuit reference
		//reference circuit_ref;

		//// Logic state
		//Logic_Value state;
		Event e{Entities::AND, 0, 0, Logic_Value::HI_Z};
		std::vector<Event> v_e;
		v_e.push_back(e);
		return v_e;
	}

	/**
	* Simulate the circuit for one frame. The main function or simulation class (whoever owns this) will manage ticking
	* and passing events between producers and consumers.
	*
	* @param[in]	circuit		A pointer to the circuit simulating these events
	* @param[in]	n_plus_1	Set of events belonging to frame N+1 which will now be simulated. Recommend std::move() 
	*
	* @return	Displayable simulation events from input event set and circuit state.
	*/
	static std::vector<Event> simulate(Circuit* circuit, const std::vector<Event>& n_plus_1) {
		return circuit->simulate(n_plus_1);
	}

private:
	// Indicate if the circuit is dirty, that is, the structure has been mutated
	// since the last "clear" command.
	// There may be much prettier solutions than a full rebuild every time there's an add or delete.
	// For example, an added wire is going to be a network merge, a added junction is a lone network,
	// (for merge, overload the < operator in Network to merge the smaller into the larger), a removed 
	// wire or junction may be a split or reduction. A removed gate or other big entity would be a number
	// of lost junctions (re-use functionality). Rarely would I need to do a full rebuild.
	// bool dirty = false;

	std::vector<Gate> gates;
	std::vector<GUI_Junction> gui_junctions;
	std::vector<Junction> junctions;
	std::set<Network> networks;
	std::vector<Wire> wires;

	//// Rebuild all the networks and reset get_state func ptrs, and add events to queue.
	//void rebuild_networks();

	//// Add gate, returns index of gate.
	//gate_ref add_gate(Gates gateType, uint32_t num_inputs);

	//// Remove the gate with given index. 
	//// Returns false if gate did not exist.
	//bool remove_gate(gate_ref i);

	//// Add a GUI junction
	//gui_junction_ref add_gui_junction(uint32_t num_inputs = 4);
	//bool remove_gui_junction(gui_junction_ref i);

	//wire_ref add_wire(/* Need info here! */);
	//bool remove_wire(wire_ref i);

	//bool is_dirty() { return dirty; }

	//void clean() {
	//	rebuild_networks();
	//	dirty = false;
	//}
};