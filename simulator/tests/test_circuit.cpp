#include <catch2/catch_test_macros.hpp>
#include "circuit.hpp"

TEST_CASE("I can add a gate to the circuit", "[CIRCUIT]") {
	Circuit c;

	Event e;
	e.action = Action::Add;
	e.entity_type = Entities::AND;
	e.n_inputs = 8;
	e.req_id = 2;
	e.n_outputs = 1;
	e.state = Logic_Value::ZERO;

	auto ret = c.process(e);
	CHECK(ret.has_value());
	auto ret_e = ret.value();

	CHECK(ret_e.action == Action::Added);
	CHECK(ret_e.entity_type == Entities::AND);
	CHECK(ret_e.err == "");
}