#include <catch2/catch_test_macros.hpp>
#include<type_traits>
#include "entities.hpp"

TEST_CASE("Default initialized Input Junction is valid", "[ENTITIES]")
{
	Input a;

	CHECK_NOTHROW(a.get_state());

	CHECK(a.get_state() == Logic_Value::HI_Z);
}

TEST_CASE("Default initialized Output Junction is valid", "[ENTITIES]")
{
	Output a;

	CHECK_NOTHROW(a.get_state());

	CHECK(a.get_state() == Logic_Value::HI_Z);
}

TEST_CASE("error_event creates event identical plus error string & failed action", "[ENTITIES]") {
	std::string blank = "";
	std::string err_string = "Error String 62";
	REQUIRE(blank != err_string);

	Event input;

	input.entity_type = Entities::NOR;
	input.req_id = 32;
	input.circuit_ref = 57;
	input.action = Action::Remove;
	input.state = Logic_Value::CONFLICT;
	input.n_inputs = 89;
	input.n_outputs = 13;
	input.err = blank;

	Event out = error_event(err_string, input);

	CHECK(out.action == Action::Error);
	CHECK(out.circuit_ref == input.circuit_ref);
	CHECK(out.entity_type == input.entity_type);
	CHECK(out.err == err_string); // the only different piece.
	CHECK(out.n_inputs == input.n_inputs);
	CHECK(out.n_outputs == input.n_outputs);
	CHECK(out.state == input.state);
}

TEST_CASE("added_event creates event mostly identical with action Added", "[ENTITIES]") {
	uint32_t new_index = 73;

	Event input;

	input.entity_type = Entities::XOR;
	input.req_id = 33;
	input.circuit_ref = 58;
	input.action = Action::Add;
	input.state = Logic_Value::HI_Z;
	input.n_inputs = 90;
	input.n_outputs = 14;
	input.err = "This should be blank but might not be... never know";

	REQUIRE(input.circuit_ref != new_index);
	Event out = added_event(new_index, input);

	CHECK(out.action == Action::Added);
	CHECK(out.circuit_ref == new_index);
	CHECK(out.entity_type == input.entity_type);
	CHECK(out.err.empty()); 
	CHECK(out.n_inputs == input.n_inputs);
	CHECK(out.n_outputs == input.n_outputs);
	CHECK(out.state == input.state);
}

TEST_CASE("removed_event creates event mostly identical with action Added", "[ENTITIES]") {
	Event input;

	input.entity_type = Entities::NAND;
	input.req_id = 34;
	input.circuit_ref = 59;
	input.action = Action::Remove;
	input.state = Logic_Value::ONE;
	input.n_inputs = 91;
	input.n_outputs = 15;
	input.err = "This should be blank but might not be... never know";

	Event out = removed_event(input);

	CHECK(out.action == Action::Removed);
	CHECK(out.circuit_ref == input.circuit_ref);
	CHECK(out.entity_type == input.entity_type);
	CHECK(out.err.empty());
	CHECK(out.n_inputs == input.n_inputs);
	CHECK(out.n_outputs == input.n_outputs);
	CHECK(out.state == input.state);
}

TEST_CASE("Test add_x_input_junctions", "[ENTITES]") {
	Circuit_Data data;
	
	uint32_t initial_num = 5;
	auto ret = add_x_input_junctions(initial_num, data);

	// Make sure it gave us as many as we wanted.
	CHECK(ret.size() == initial_num);
};

TEST_CASE("Test add_x_output_junctions", "[ENTITES]") {
	Circuit_Data data;

	uint32_t initial_num = 5;
	auto ret = add_x_output_junctions(initial_num, data);

	// Make sure it gave us as many as we wanted.
	CHECK(ret.size() == initial_num);
};

TEST_CASE("is_gate is right", "[ENTITIES]") {
	CHECK(is_gate(Entities::AND));
	CHECK(is_gate(Entities::NAND));
	CHECK(is_gate(Entities::OR));
	CHECK(is_gate(Entities::NOR));
	CHECK(is_gate(Entities::XOR));
	CHECK(!is_gate(Entities::GUI_JUNCTION));
	CHECK(!is_gate(Entities::NOT));
	CHECK(!is_gate(Entities::WIRE));
}

TEST_CASE("add_gate / remove_gate works") {
	Circuit_Data data;

	Event add_gate_e;
	add_gate_e.action = Action::Add;
	add_gate_e.entity_type = Entities::AND;
	add_gate_e.n_inputs = 5;

	Event added_gate = add_gate(added_gate, data);
	CHECK(added_gate.action == Action::Added);

	CHECK(data.gates.size() == 1);
	CHECK(data.junctions.size() == 6); // plus one for output

	// Now remove it.
	Event remove_gate_e;
	remove_gate_e.action = Action::Remove;
	remove_gate_e.circuit_ref = added_gate.circuit_ref;

	Event removed_gate = remove_gate(remove_gate_e, data)[0];
	CHECK(removed_gate.action == Action::Removed);

	// Verify every old entity is now a nullptr
	CHECK(data.gates[0] == nullptr);
	CHECK(data.junctions[0] == nullptr);
	CHECK(data.junctions[1] == nullptr);
	CHECK(data.junctions[2] == nullptr);
	CHECK(data.junctions[3] == nullptr);
	CHECK(data.junctions[4] == nullptr);
	CHECK(data.junctions[5] == nullptr);
}

//TEST_CASE("Removing a junction removes its wires too") {
//	Circuit_Data data;
//
//
//}
//
//TEST_CASE("Removing a junction dirties its network.") {
//
//}
//
//TEST_CASE("Removing a wire dirties its network.") {
//
//}