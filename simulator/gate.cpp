#include "gate.hpp"

Gate::Gate(Logic_Value (*logicFunc)(std::vector<Logic_Value>), std::vector<Input *> input_junction_ptrs, Output *output_junction_ptr)
    : logicFunction(logicFunc),
      input_junction_ptrs(input_junction_ptrs),
      output_junction_ptr(output_junction_ptr)
{
}

void Gate::process()
{
    output_junction_ptr->state = logicFunction(Input::to_values(input_junction_ptrs));
}
