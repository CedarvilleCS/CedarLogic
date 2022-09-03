#pragma once

#include "junction.hpp"
#include "logic.hpp"
#include "wire.hpp"
#include <memory>
#include <set>

/**
 * Network class
 *
 * A network is a contiguous grouping of wires and junctions.
 *
 */
class Network {
public:
  /**
   * @brief All the internal data for a network.
   *
   * Encapsulated so it's easier to hand to the parameterized constructor.
   */
  struct Data {
    Logic_Value state =
        Logic_Value::HI_Z;             //! The network's state, defaults to HI_Z
    std::set<Input *> input_junctions; //! The network's input junctions, will
                                       //! set them every refresh
    std::set<Output *>
        output_junctions; //! The network's output junctions, will determine new
                          //! state from their values during refresh.
    std::set<const Wire *> wires; //! The network's wires
  };

  /**
   * Default constructor results in an un-allowed state and is therefore
   * deleted.
   *
   * To create network instances, use static functions.
   */
  Network() = delete;

  /**
   * @brief Construct a new Network object
   *
   * @param d Data, the internal data for the network.
   */
  Network(Data d) : data(d){};

  /**
   * Get network state.
   *
   * This state is set every time refresh_state is called.
   */
  const Logic_Value get_state() const { return data.state; }

  /**
   * Refresh the network state.
   *
   * Network will poll all it's output junctions and from their values determine
   * what Logic_Value the network has.
   */
  void refresh_state();

  /**
   * Get all wires in network.
   *
   * This is useful when say the network state has changed and the caller wants
   * to update every GUI wire to show it's new state.
   *
   * I could pass this by reference but that'd be a pointer which could be saved
   * beyond it's life-time.
   */
  std::set<const Wire *> get_wires() const { return data.wires; }

  /**
   * Does network contain junction?
   */
  bool has_junction(Junction *j_ptr) const;

  /**
   * Does network contains wire ?
   */
  bool has_wire(Wire *wire_ptr) const;

private:
  Data data;
};

/**
 * @brief A collection of networks
 *
 * This class's job, for the circuit, is to provide an elegant solution for:
 * 1. Getting every wire who's state changed.
 * 2. Making sure the input junctions always have the correct state.
 * 
 * The circuit does not interact with the Network struct above. Further, the circuit
 * only requires the network interface to remain the same. Anything else can change.
 * 
 * Memory:
 * 	- Networks and all it's functions may not `new` or `delete` Junction or Wire instances
 *  - Networks may not change any attribute of Junction or Wire instances with one exception:
 * 		it must and is expected to set the `state` attribute of every Input junction which is
 * 		in a network.
 */
struct Networks {
public:

  /**
   * @brief Notify networks of newly created wire.
   * 
   * The circuit calls this function when it creates a new wire. The 
   * Networks object probably wants to record the pointer somewhere.
   * 
   * The circuit does not give Networks permission to ever delete or modify this wire.
   */
  void created_wire(const Wire *);

  /**
   * @brief Notify networks of junction about to be removed.
   *
   * The circuit calls this function when it is about to delete a junction.
   * For the duration of the function call (until you return) that pointer
   * is valid. While Networks may keep a copy of the pointer afterwards (it may
   * be a useful key in a map or set) Networks promises not to dereference that
   * pointer - doing so may cause a seg fault. Get any information you need about
   * the junction out during this function call.
   *
   * @param j to-be deleted junction pointer, not valid after return
   */
  void deleting_junction(Junction *j);

  /**
   * @brief Notify networks of wire about to be removed
   *
   * The circuit calls this function when it is about to delete a wire.
   * For the duration of the function call (until you return) that pointer
   * is valid. While Networks may keep a copy of the pointer afterwards (it may
   * be a useful key in a map or set) Networks promises not to dereference that
   * pointer - doing so may cause a seg fault. Get any information you need about
   * the wire (like what it's junctions are) out during this function call.
   *
   * @param ws to-be deleted wire pointer, not valid after return
   */
  void deleting_wire(const Wire *ws);
  
  /**
   * @brief Simulate one step in the network.
   * 
   * Do not confuse a step here with a clock signal "step". It just means:
   * 1. Fix the networks in response to add/delete actions (if not already done)
   * 2. For every network, check all it's output junctions and determine it's state as a result
   * 3. For every network, for every input junction, set state to network's state.
   * 4. For every network whose state changed, add all it's wire's id numbers to a vector with their new state.
   * 5. Return that vector with the id number/state pairs of every wire with a state change.
   * 
   * Note, if you aren't much familiar with std::pairs, search Simulator for other usages 
   * and just know that to make a pair you call `std::make_pair(id_number, logic_value)`. 
   *
   * @return std::vector<std::pair<uint32_t, Logic_Value>> (wire id number, it's logic state)
   */
  std::vector<std::pair<uint32_t, Logic_Value>> step();

private:
  std::set<Network*> nets;
};
=======
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
