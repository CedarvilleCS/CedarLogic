#include <catch2/catch_test_macros.hpp>
#include "circuit.hpp"

TEST_CASE("Gates are added with unique refs and accurate req_ids", "[CIRCUIT:add_gate]") {
	WHEN("I add one gate to the circuit the req_id is passed back with a unique circuit ref.") {
		Circuit circuit;
		std::set<uint32_t> refs_used;
		for (uint32_t req_id = 0; req_id < 10; req_id++) {
			Event add_gate_e{ Entities::AND, 0, 0, Logic_Value::ZERO, 4, 1 };
			auto response = circuit.simulate(std::vector<Event>{add_gate_e});

			// Check the response exists!
			REQUIRE(response.size() != 0);

			// Check the req_id is the one for the event we sent
			CHECK(response[0].req_id == req_id);

			// Check the ref is not re-used
			CHECK(refs_used.insert(response[0].circuit_ref).second == true);
		}
	}
}