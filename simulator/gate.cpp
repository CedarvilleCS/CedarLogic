#include "gate.hpp"

Gate::Gate(Logic_Value (*logicFunc)(std::vector<Logic_Value>),
           std::vector<Input *> input_junction_ptrs,
           Output *output_junction_ptr)
    : logicFunction(logicFunc), input_junction_ptrs(input_junction_ptrs),
      output_junction_ptr(output_junction_ptr) {}

void Gate::process() {
  output_junction_ptr->state =
      logicFunction(Input::to_values(input_junction_ptrs));
}

std::vector<uint32_t> Gate::get_input_j_ids() const {
  std::vector<uint32_t> ret;
  for (uint32_t i = 0; i < input_junction_ptrs.size(); i++) {
    ret.push_back(input_junction_ptrs[i]->index_id);
  }
  return ret;
}

uint32_t Gate::get_output_j_id() const { return output_junction_ptr->index_id; }
