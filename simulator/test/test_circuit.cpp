#include <catch2/catch_test_macros.hpp>
#include "circuit.hpp"

TEST_CASE("Gates are added with unique refs and accurate req_ids", "[CIRCUIT:add_gate]") {
	Circuit circuit;

	WHEN("I add one gate to the circuit it succeeds") {
		std::vector<std::unique_ptr<Incoming_Event>> events_in;
		events_in.push_back(std::make_unique<Add_Gate>(1, Gates::AND, 2));
		auto ret = circuit.simulate(events_in);
		CHECK(ret.size() == 1);
		CHECK(ret[0]->type == Outgoing_Events::Added_Gate);

		auto e = ret[0];
		Added_Gate& casted_e = dynamic_cast<Added_Gate*>(&std::move(ret[0]));

		CHECK(ret[0].req_id == 2);
	}
}