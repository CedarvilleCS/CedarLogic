#include "circuit.hpp"


// For reference
//void Gate::process()
//{
//    std::vector<Logic_Value> logic_inputs;
//    for (int i = 0; i < inputs.size(); i++)
//    {
//        logic_inputs.push_back(inputs[i]->get_state());
//    }
//    output->set_state(logicFunction(logic_inputs));
//}

//std::vector<std::unique_ptr<Outgoing_Event>> Circuit::simulate(std::vector<std::unique_ptr<Incoming_Event>> n_plus_1)
//{
//	std::vector<std::unique_ptr<Outgoing_Event>> events;
//	std::unique_ptr<Added_Gate> e = std::make_unique<Added_Gate>(0, 0);
//
//	events.push_back(std::move(e));
//	return std::move(events);
//}
