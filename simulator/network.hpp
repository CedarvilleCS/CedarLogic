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
 * The network stores junction pointers
 *
 * Whenever a wire is added a new network may be created, or 2..n networks may
 * be merged. Whenever a junction or wire is removed a network may be destroyed,
 * split into 2..n sub-networks or only reduced by that wire and some subset of
 * it's junctions.
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

  /**
   * Remove given wire from given network and receive back all resulting
   * networks.
   *
   * @param network: unique_ptr to a network, function takes ownership of
   * network and will likely delete it.
   * @param wire_ptr: a const (so immutable) ptr to a wire.
   *
   * @return a vector of unique_ptrs to new Networks resulting from the removal.
   * Per the meaning of the `unique_ptr`, the caller assumes ownership.
   */
  static std::vector<std::unique_ptr<Network>>
  remove_wires_from_network(std::unique_ptr<Network> network,
                            const Wire *wire_ptr);

  /**
   * Remove given junction from given network and receive back all resulting
   * networks.
   *
   * @param network: unique_ptr to a network, function takes ownership of
   * network and will likely delete it.
   * @param junction_ptr: a const (so immutable) ptr to a junction.
   *
   * @return a vector of unique_ptrs to new Networks resulting from the removal.
   * Per the meaning of the `unique_ptr`, the caller assumes ownership.
   */
  static std::vector<std::unique_ptr<Network>>
  remove_junction_from_network(std::unique_ptr<Network> network,
                               const Junction *junction_ptr);

  /**
   * Compose all possible networks out of the given wires and junctions.
   *
   * @param ws: const Wire pointers. New network(s) will store these, but cannot
   * modify them (hence cost)
   * @param js: non-const Junction pointers. When refreshing, the new network(s)
   * will set the input junction states.
   *
   * @return a vector of unique_ptrs to new Networks resulting from the removal.
   * Per the meaning of the `unique_ptr`, the caller assumes ownership.
   */
  static std::vector<std::unique_ptr<Network>>
  compose_networks(const std::vector<const Wire *> &ws,
                   const std::vector<Junction *> &js);

  /**
   * Compose all possible networks out of the given networks and wire. Useful
   * when adding a wire that affects a set of networks. Function is valid
   * regardless.
   *
   * @param networks: networks which this function takes ownership of.
   * Ordinarily, these would be every network which has a junction a new wire
   * touches.
   * @param wire: a const Wire pointer. Under the expected use-case, this wire
   * has been added and affects the provided networks.
   *
   * @return a vector of unique_ptrs to new Networks resulting from the removal.
   * Per the meaning of the `unique_ptr`, the caller assumes ownership.
   */
  static std::vector<std::unique_ptr<Network>>
  compose_networks(std::vector<std::unique_ptr<Network>> networks,
                   const Wire *wire);

private:
  Data data;
};

/**
 * @brief A collection of networks
 *
 * Useful to encapsulate complicated and expensive function calls.
 *
 */
struct Networks {
public:
  /**
   * @brief Notify networks of junctions about to be removed
   *
   * Networks are responsible to ensure networks are solid before
   * they are used, but may not immediately update them.
   *
   * @param j to-be deleted junction pointer, not valid after return
   */
  void deleting_junction(Junction *j);

  /**
   * @brief Notify networks of wire about to be removed
   *
   * Networks is responsible to ensure its networks are solid before
   * they are used, but may not immediately update them.
   *
   * @param ws to-be deleted wire pointer, not valid after return
   */
  void deleting_wire(Wire *ws);

private:
  /**
   * @note They are encapsulated in unique_ptrs so it can be obvious when
   * ownership is transferred to Network's static functions which will new and
   * delete Network instances.
   *
   * @note They are held in a set rather than vector because they are created
   * and deleted more frequently than any other object and so a vector would be
   * expected to result in much more wasted space.
   */
  std::set<std::unique_ptr<Network>> nets;
};
