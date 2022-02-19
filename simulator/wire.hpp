#pragma once

#include "junction.hpp"
#include <set>
#include <stdint.h>

/**
 * @brief Wires connect [2..n] junctions
 *
 */
struct Wire {

  /**
   * @brief Determine if wire is connected to the given junction.
   *
   * @param ptr const junction ptr (will not modify it)
   * @return true
   * @return false
   */
  bool has_junction(const Junction *ptr) {
    return junction_ptrs.find(ptr) != junction_ptrs.end();
  }

  /**
   * @brief The junctions this wire connects to.
   *
   * @note that the wire class never modifies it's junctions (hence const).
   */
  std::set<const Junction *> junction_ptrs;

  /**
   * @brief the wire's circuit index
   *
   * Useful to provide a quick ptr -> index mapping.
   */
  uint32_t index_id;
};