#pragma once

#include "logic.hpp"
#include "entities.hpp"
#include <set>
#include <memory>

/**
* Network class
* 
* A network is a contiguous grouping of wires and junctions.
* 
* The network stores junction pointers
*
* Whenever a wire is added a new network may be created, or 2..n networks may be merged.
* Whenever a junction or wire is removed a network may be destroyed, split into 2..n sub-networks or only reduced by that wire and some subset of it's junctions.
*/
class Network {
public:
	/**
	* Default constructor results in an un-allowed state and is therefore deleted.
	* 
	* To create network instances, use static functions.
	*/
	Network() = delete;

	/**
	* Get network state.
	* 
	* This state is set every time refresh_state is called.
	*/
	const Logic_Value get_state() const { return data.state; }

	/**
	* Refresh the network state.
	* 
	* Network will poll all it's output junctions and from their values determine what
	* Logic_Value the network has.
	*/
	void refresh_state();

	/**
	* Get all wires in network.
	* 
	* This is useful when say the network state has changed and the caller wants to update every GUI wire
	* to show it's new state.
	* 
	* I could pass this by reference but that'd be a pointer which could be saved beyond it's life-time.
	*/
	std::set<const Wire*> get_wires() const { return data.wires; }

	/**
	* Does network contain junction?
	*/
	bool has_junction(Junction* j_ptr) const;

	/**
	* Does network contains wire ?
	*/
	bool has_wire(Wire* wire_ptr) const;

	/**
	* Remove given wire from given network and receive back all resulting networks.
	*
	* @param network: unique_ptr to a network, function takes ownership of network and will likely delete it.
	* @param wire_ptr: a const (so immutable) ptr to a wire.
	*
	* @return a vector of unique_ptrs to new Networks resulting from the removal. Per the meaning of the `unique_ptr`, the caller assumes ownership.
	*/
	static std::vector< std::unique_ptr<Network>> remove_wires_from_network(std::unique_ptr<Network> network, const Wire* wire_ptr);

	/**
	* Remove given junction from given network and receive back all resulting networks.
	* 
	* @param network: unique_ptr to a network, function takes ownership of network and will likely delete it.
	* @param junction_ptr: a const (so immutable) ptr to a junction.
	* 
	* @return a vector of unique_ptrs to new Networks resulting from the removal. Per the meaning of the `unique_ptr`, the caller assumes ownership.
	*/
	static std::vector<std::unique_ptr<Network>> remove_junction_from_network(std::unique_ptr<Network> network, const Junction* junction_ptr);

	/**
	* Compose all possible networks out of the given wires and junctions.
	* 
	* @param ws: const Wire pointers. New network(s) will store these, but cannot modify them (hence cost)
	* @param js: non-const Junction pointers. When refreshing, the new network(s) will set the input junction states.
	* 
	* @return a vector of unique_ptrs to new Networks resulting from the removal. Per the meaning of the `unique_ptr`, the caller assumes ownership.
	*/
	static std::vector<std::unique_ptr<Network>> compose_networks(const std::vector<const Wire*>& ws, const std::vector<Junction*>& js);

	/**
	* Compose all possible networks out of the given networks and wire. Useful when adding a wire that affects a set of networks. Function is valid 
	* regardless.
	*
	* @param networks: networks which this function takes ownership of. Ordinarily, these would be every network which has a junction a new wire touches.
	* @param wire: a const Wire pointer. Under the expected use-case, this wire has been added and affects the provided networks.
	*
	* @return a vector of unique_ptrs to new Networks resulting from the removal. Per the meaning of the `unique_ptr`, the caller assumes ownership.
	*/
	static std::vector<std::unique_ptr<Network>> compose_networks(std::vector<std::unique_ptr<Network>> networks, const Wire* wire);

private:
	struct Data {
		Logic_Value state = Logic_Value::HI_Z;
		std::set<Input*> input_junctions;
		std::set<Output*> output_junctions;
		std::set<const Wire*> wires;
	};

	Data data;

	Network(Data d) : data(d) {};
};