#pragma once

#include <stdint.h>
#include <set>
#include <vector>
#include <memory>
#include <variant>
#include <optional>
#include "event.hpp"
#include "entities.hpp"

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
	std::vector<Event> simulate(const std::vector<Event>& n_plus_1);

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

	// Exposed for testing, may not leave so exposed.
	std::optional<Event> process(const Event e);

private:
	Circuit_Data data;
};
