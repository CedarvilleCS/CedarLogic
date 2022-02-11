#include <catch2/catch_test_macros.hpp>
#include "entities.hpp"

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