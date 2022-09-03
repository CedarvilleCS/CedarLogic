#pragma once

#include "junction.hpp"
#include "logic.hpp"
#include <stdint.h>
#include <vector>

/**
 * @brief All gate types, used during Gate creation to pass the right
 * logic_function to it.
 *
 */
enum class Gates {
  AND,
  XOR,
  OR,
  NAND,
  NOR // XNOR not implemented
};

/**
 * @brief The logic gate, all have multiple inputs and one output.
 */
struct Gate {
public:
  /**
   * @brief Construct a new Gate object - deleted because all gate objects must
   * have their junctions and logic function ptr.
   */
  Gate() = delete;

  /**
   * @brief gets input junction states and sets output junction state.
   *
   */
  void process();

  /**
   * @brief Get the input junction ids
   *
   * @return std::vector<uint32_t>
   */
  std::vector<uint32_t> get_input_j_ids() const;

  /**
   * @brief Get the output junction id
   *
   * @return uint32_t
   */
  uint32_t get_output_j_id() const;

protected:
  /**
   * @brief Construct a new Gate object
   *
   * @param logicFunc implements the logic function taking a vector of input
   * values and producing the output value.
   * @param input_junction_ptrs
   * @param output_junction_ptr
   */
  Gate(Logic_Value (*logicFunc)(std::vector<Logic_Value>),
       std::vector<Input *> input_junction_ptrs, Output *output_junction_ptr);

private:
  Logic_Value (*logicFunction)(std::vector<Logic_Value>);

  /**
   * @brief Input junctions
   *
   * @note LSB is index 0, MSB is index = size() - 1.
   */
  const std::vector<Input *> input_junction_ptrs;

  /**
   * @brief Output junctions
   *
   * @note the Junction pointers are not const, because Output junction state is
   * set in the process function.
   */
  Output *output_junction_ptr;
};

class GateAND : public Gate {
public:
  GateAND() = delete;

  GateAND(std::vector<Input *> input_junction_ptrs, Output *output_junction_ptr)
      : Gate(logic::AND, input_junction_ptrs, output_junction_ptr) {}
};

class GateOR : public Gate {
public:
  GateOR() = delete;

  GateOR(std::vector<Input *> input_junction_ptrs, Output *output_junction_ptr)
      : Gate(logic::OR, input_junction_ptrs, output_junction_ptr) {}
};

class GateNAND : public Gate {
public:
  GateNAND() = delete;

  GateNAND(std::vector<Input *> input_junction_ptrs,
           Output *output_junction_ptr)
      : Gate(logic::NAND, input_junction_ptrs, output_junction_ptr) {}
};

class GateNOR : public Gate {
public:
  GateNOR() = delete;

  GateNOR(std::vector<Input *> input_junction_ptrs, Output *output_junction_ptr)
      : Gate(logic::NOR, input_junction_ptrs, output_junction_ptr) {}
};

class GateXOR : public Gate {
public:
  GateXOR() = delete;

  GateXOR(std::vector<Input *> input_junction_ptrs, Output *output_junction_ptr)
      : Gate(logic::XOR, input_junction_ptrs, output_junction_ptr) {}
};
