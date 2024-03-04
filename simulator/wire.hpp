#pragma once

#include "junction.hpp"
#include <set>
#include <stdint.h>

/**
 * @brief Wires connect [2..n] junctions
 *
 */
struct Wire {

  Wire(uint32_t id) : id(id) {} 

  /**
   * @brief Determine if wire is connected to the given junction.
   *
   * @param ptr const junction ptr (will not modify it)
   * @return true
   * @return false
   */
  bool has_junction(const Junction *ptr) const {
    return junction_ptrs.find(ptr) != junction_ptrs.end();
  }

  /**
   * @brief The junctions this wire connects to.
   *
   * @note that the wire class never modifies it's junctions (hence const).
   */
  std::set<const Junction *> junction_ptrs;

  /**
   * @brief the wire's unique id number
   *
   * Useful to provide a quick ptr -> index mapping when talking with Circuit.
   */
  const uint32_t id;
};