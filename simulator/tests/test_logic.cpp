#include <catch2/catch_test_macros.hpp>
#include "logic.hpp"

using namespace logic;

typedef std::vector<Logic_Value> vec;

TEST_CASE("OR 2x logic is sound", "[LOGIC]") {
	CHECK(OR(vec{ Logic_Value::ZERO, Logic_Value::ZERO }) == Logic_Value::ZERO);
	CHECK(OR(vec{ Logic_Value::ONE, Logic_Value::ONE }) == Logic_Value::ONE);
	CHECK(OR(vec{ Logic_Value::HI_Z, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);
	CHECK(OR(vec{ Logic_Value::CONFLICT, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);
	CHECK(OR(vec{ Logic_Value::UNKNOWN, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);

	CHECK(OR(vec{ Logic_Value::ONE, Logic_Value::ZERO }) == Logic_Value::ONE);
	CHECK(OR(vec{ Logic_Value::HI_Z, Logic_Value::ZERO }) == Logic_Value::UNKNOWN);
	CHECK(OR(vec{ Logic_Value::CONFLICT, Logic_Value::ZERO }) == Logic_Value::UNKNOWN);
	CHECK(OR(vec{ Logic_Value::UNKNOWN, Logic_Value::ZERO }) == Logic_Value::UNKNOWN);

	CHECK(OR(vec{ Logic_Value::ZERO, Logic_Value::ONE }) == Logic_Value::ONE);
	CHECK(OR(vec{ Logic_Value::HI_Z, Logic_Value::ONE }) == Logic_Value::ONE);
	CHECK(OR(vec{ Logic_Value::CONFLICT, Logic_Value::ONE }) == Logic_Value::ONE);
	CHECK(OR(vec{ Logic_Value::UNKNOWN, Logic_Value::ONE }) == Logic_Value::ONE);

	CHECK(OR(vec{ Logic_Value::ZERO, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);
	CHECK(OR(vec{ Logic_Value::ONE, Logic_Value::HI_Z }) == Logic_Value::ONE);
	CHECK(OR(vec{ Logic_Value::CONFLICT, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);
	CHECK(OR(vec{ Logic_Value::UNKNOWN, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);

	CHECK(OR(vec{ Logic_Value::ZERO, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);
	CHECK(OR(vec{ Logic_Value::ONE, Logic_Value::CONFLICT }) == Logic_Value::ONE);
	CHECK(OR(vec{ Logic_Value::HI_Z, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);
	CHECK(OR(vec{ Logic_Value::UNKNOWN, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);

	CHECK(OR(vec{ Logic_Value::ZERO, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);
	CHECK(OR(vec{ Logic_Value::ONE, Logic_Value::UNKNOWN }) == Logic_Value::ONE);
	CHECK(OR(vec{ Logic_Value::HI_Z, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);
	CHECK(OR(vec{ Logic_Value::CONFLICT, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);
}

TEST_CASE("OR 3x logic is sound", "[LOGIC]") {
	CHECK(OR(vec{ Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO }) == Logic_Value::ZERO);
	CHECK(OR(vec{ Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ONE }) == Logic_Value::ONE);
	CHECK(OR(vec{ Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ZERO }) == Logic_Value::ONE);
	CHECK(OR(vec{ Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ONE }) == Logic_Value::ONE);
	CHECK(OR(vec{ Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ZERO }) == Logic_Value::ONE);
	CHECK(OR(vec{ Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ONE }) == Logic_Value::ONE);
	CHECK(OR(vec{ Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ZERO }) == Logic_Value::ONE);
	CHECK(OR(vec{ Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ONE }) == Logic_Value::ONE);
	CHECK(OR(vec{ Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE }) == Logic_Value::ONE);
}

TEST_CASE("AND 2x logic is sound", "[LOGIC]") {
	CHECK(AND(vec{ Logic_Value::ZERO, Logic_Value::ZERO }) == Logic_Value::ZERO);
	CHECK(AND(vec{ Logic_Value::ONE, Logic_Value::ONE }) == Logic_Value::ONE);
	CHECK(AND(vec{ Logic_Value::HI_Z, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);
	CHECK(AND(vec{ Logic_Value::CONFLICT, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);
	CHECK(AND(vec{ Logic_Value::UNKNOWN, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);

	CHECK(AND(vec{ Logic_Value::ONE, Logic_Value::ZERO }) == Logic_Value::ZERO);
	CHECK(AND(vec{ Logic_Value::HI_Z, Logic_Value::ZERO }) == Logic_Value::ZERO);
	CHECK(AND(vec{ Logic_Value::CONFLICT, Logic_Value::ZERO }) == Logic_Value::ZERO);
	CHECK(AND(vec{ Logic_Value::UNKNOWN, Logic_Value::ZERO }) == Logic_Value::ZERO);

	CHECK(AND(vec{ Logic_Value::ZERO, Logic_Value::ONE }) == Logic_Value::ZERO);
	CHECK(AND(vec{ Logic_Value::HI_Z, Logic_Value::ONE }) == Logic_Value::UNKNOWN);
	CHECK(AND(vec{ Logic_Value::CONFLICT, Logic_Value::ONE }) == Logic_Value::UNKNOWN);
	CHECK(AND(vec{ Logic_Value::UNKNOWN, Logic_Value::ONE }) == Logic_Value::UNKNOWN);

	CHECK(AND(vec{ Logic_Value::ZERO, Logic_Value::HI_Z }) == Logic_Value::ZERO);
	CHECK(AND(vec{ Logic_Value::ONE, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);
	CHECK(AND(vec{ Logic_Value::CONFLICT, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);
	CHECK(AND(vec{ Logic_Value::UNKNOWN, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);

	CHECK(AND(vec{ Logic_Value::ZERO, Logic_Value::CONFLICT }) == Logic_Value::ZERO);
	CHECK(AND(vec{ Logic_Value::ONE, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);
	CHECK(AND(vec{ Logic_Value::HI_Z, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);
	CHECK(AND(vec{ Logic_Value::UNKNOWN, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);

	CHECK(AND(vec{ Logic_Value::ZERO, Logic_Value::UNKNOWN }) == Logic_Value::ZERO);
	CHECK(AND(vec{ Logic_Value::ONE, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);
	CHECK(AND(vec{ Logic_Value::HI_Z, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);
	CHECK(AND(vec{ Logic_Value::CONFLICT, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);
}

TEST_CASE("AND 3x logic is sound", "[LOGIC]") {
	CHECK(AND(vec{ Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO }) == Logic_Value::ZERO);
	CHECK(AND(vec{ Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ONE }) == Logic_Value::ZERO);
	CHECK(AND(vec{ Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ZERO }) == Logic_Value::ZERO);
	CHECK(AND(vec{ Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ONE }) == Logic_Value::ZERO);
	CHECK(AND(vec{ Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ZERO }) == Logic_Value::ZERO);
	CHECK(AND(vec{ Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ONE }) == Logic_Value::ZERO);
	CHECK(AND(vec{ Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ZERO }) == Logic_Value::ZERO);
	CHECK(AND(vec{ Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ONE }) == Logic_Value::ZERO);
	CHECK(AND(vec{ Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE }) == Logic_Value::ONE);
}

TEST_CASE("XOR 2x logic is sound", "[LOGIC]") {
	CHECK(XOR(vec{ Logic_Value::ZERO, Logic_Value::ZERO }) == Logic_Value::ZERO);
	CHECK(XOR(vec{ Logic_Value::ONE, Logic_Value::ONE }) == Logic_Value::ZERO);
	CHECK(XOR(vec{ Logic_Value::HI_Z, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);
	CHECK(XOR(vec{ Logic_Value::CONFLICT, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);
	CHECK(XOR(vec{ Logic_Value::UNKNOWN, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);

	CHECK(XOR(vec{ Logic_Value::ONE, Logic_Value::ZERO }) == Logic_Value::ONE);
	CHECK(XOR(vec{ Logic_Value::HI_Z, Logic_Value::ZERO }) == Logic_Value::UNKNOWN);
	CHECK(XOR(vec{ Logic_Value::CONFLICT, Logic_Value::ZERO }) == Logic_Value::UNKNOWN);
	CHECK(XOR(vec{ Logic_Value::UNKNOWN, Logic_Value::ZERO }) == Logic_Value::UNKNOWN);

	CHECK(XOR(vec{ Logic_Value::ZERO, Logic_Value::ONE }) == Logic_Value::ONE);
	CHECK(XOR(vec{ Logic_Value::HI_Z, Logic_Value::ONE }) == Logic_Value::UNKNOWN);
	CHECK(XOR(vec{ Logic_Value::CONFLICT, Logic_Value::ONE }) == Logic_Value::UNKNOWN);
	CHECK(XOR(vec{ Logic_Value::UNKNOWN, Logic_Value::ONE }) == Logic_Value::UNKNOWN);

	CHECK(XOR(vec{ Logic_Value::ZERO, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);
	CHECK(XOR(vec{ Logic_Value::ONE, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);
	CHECK(XOR(vec{ Logic_Value::CONFLICT, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);
	CHECK(XOR(vec{ Logic_Value::UNKNOWN, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);

	CHECK(XOR(vec{ Logic_Value::ZERO, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);
	CHECK(XOR(vec{ Logic_Value::ONE, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);
	CHECK(XOR(vec{ Logic_Value::HI_Z, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);
	CHECK(XOR(vec{ Logic_Value::UNKNOWN, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);

	CHECK(XOR(vec{ Logic_Value::ZERO, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);
	CHECK(XOR(vec{ Logic_Value::ONE, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);
	CHECK(XOR(vec{ Logic_Value::HI_Z, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);
	CHECK(XOR(vec{ Logic_Value::CONFLICT, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);
}

TEST_CASE("XOR 3x logic is sound", "[LOGIC]") {

	CHECK(XOR(vec{ Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO }) == Logic_Value::ZERO);
	CHECK(XOR(vec{ Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ONE }) == Logic_Value::ONE);
	CHECK(XOR(vec{ Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ZERO }) == Logic_Value::ONE);
	CHECK(XOR(vec{ Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ONE }) == Logic_Value::ZERO);
	CHECK(XOR(vec{ Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ZERO }) == Logic_Value::ONE);
	CHECK(XOR(vec{ Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ONE }) == Logic_Value::ZERO);
	CHECK(XOR(vec{ Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ZERO }) == Logic_Value::ZERO);
	CHECK(XOR(vec{ Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE }) == Logic_Value::ONE);
}

TEST_CASE("XOR 4x logic is sound", "[LOGIC]") {

	CHECK(XOR(vec{ Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO }) == Logic_Value::ZERO);
	CHECK(XOR(vec{ Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ONE }) == Logic_Value::ONE);
	CHECK(XOR(vec{ Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ZERO }) == Logic_Value::ONE);
	CHECK(XOR(vec{ Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ONE }) == Logic_Value::ZERO);
	CHECK(XOR(vec{ Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ZERO }) == Logic_Value::ONE);
	CHECK(XOR(vec{ Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ONE }) == Logic_Value::ZERO);
	CHECK(XOR(vec{ Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ZERO }) == Logic_Value::ZERO);
	CHECK(XOR(vec{ Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE }) == Logic_Value::ONE);
	CHECK(XOR(vec{ Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO }) == Logic_Value::ONE);
	CHECK(XOR(vec{ Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ONE }) == Logic_Value::ZERO);
	CHECK(XOR(vec{ Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ZERO }) == Logic_Value::ZERO);
	CHECK(XOR(vec{ Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ONE }) == Logic_Value::ONE);
	CHECK(XOR(vec{ Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ZERO }) == Logic_Value::ZERO);
	CHECK(XOR(vec{ Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ONE }) == Logic_Value::ONE);
	CHECK(XOR(vec{ Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ZERO }) == Logic_Value::ONE);
	CHECK(XOR(vec{ Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE }) == Logic_Value::ZERO);
}

TEST_CASE("NOR 2x logic is sound", "[LOGIC]") {
	CHECK(NOR(vec{ Logic_Value::ZERO, Logic_Value::ZERO }) == Logic_Value::ONE);
	CHECK(NOR(vec{ Logic_Value::ONE, Logic_Value::ONE }) == Logic_Value::ZERO);
	CHECK(NOR(vec{ Logic_Value::HI_Z, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);
	CHECK(NOR(vec{ Logic_Value::CONFLICT, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);
	CHECK(NOR(vec{ Logic_Value::UNKNOWN, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);

	CHECK(NOR(vec{ Logic_Value::ONE, Logic_Value::ZERO }) == Logic_Value::ZERO);
	CHECK(NOR(vec{ Logic_Value::HI_Z, Logic_Value::ZERO }) == Logic_Value::UNKNOWN);
	CHECK(NOR(vec{ Logic_Value::CONFLICT, Logic_Value::ZERO }) == Logic_Value::UNKNOWN);
	CHECK(NOR(vec{ Logic_Value::UNKNOWN, Logic_Value::ZERO }) == Logic_Value::UNKNOWN);

	CHECK(NOR(vec{ Logic_Value::ZERO, Logic_Value::ONE }) == Logic_Value::ZERO);
	CHECK(NOR(vec{ Logic_Value::HI_Z, Logic_Value::ONE }) == Logic_Value::ZERO);
	CHECK(NOR(vec{ Logic_Value::CONFLICT, Logic_Value::ONE }) == Logic_Value::ZERO);
	CHECK(NOR(vec{ Logic_Value::UNKNOWN, Logic_Value::ONE }) == Logic_Value::ZERO);

	CHECK(NOR(vec{ Logic_Value::ZERO, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);
	CHECK(NOR(vec{ Logic_Value::ONE, Logic_Value::HI_Z }) == Logic_Value::ZERO);
	CHECK(NOR(vec{ Logic_Value::CONFLICT, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);
	CHECK(NOR(vec{ Logic_Value::UNKNOWN, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);

	CHECK(NOR(vec{ Logic_Value::ZERO, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);
	CHECK(NOR(vec{ Logic_Value::ONE, Logic_Value::CONFLICT }) == Logic_Value::ZERO);
	CHECK(NOR(vec{ Logic_Value::HI_Z, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);
	CHECK(NOR(vec{ Logic_Value::UNKNOWN, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);

	CHECK(NOR(vec{ Logic_Value::ZERO, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);
	CHECK(NOR(vec{ Logic_Value::ONE, Logic_Value::UNKNOWN }) == Logic_Value::ZERO);
	CHECK(NOR(vec{ Logic_Value::HI_Z, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);
	CHECK(NOR(vec{ Logic_Value::CONFLICT, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);
}

TEST_CASE("NAND 2x logic is sound", "[LOGIC]") {
	CHECK(NAND(vec{ Logic_Value::ZERO, Logic_Value::ZERO }) == Logic_Value::ONE);
	CHECK(NAND(vec{ Logic_Value::ONE, Logic_Value::ONE }) == Logic_Value::ZERO);
	CHECK(NAND(vec{ Logic_Value::HI_Z, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);
	CHECK(NAND(vec{ Logic_Value::CONFLICT, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);
	CHECK(NAND(vec{ Logic_Value::UNKNOWN, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);

	CHECK(NAND(vec{ Logic_Value::ONE, Logic_Value::ZERO }) == Logic_Value::ONE);
	CHECK(NAND(vec{ Logic_Value::HI_Z, Logic_Value::ZERO }) == Logic_Value::ONE);
	CHECK(NAND(vec{ Logic_Value::CONFLICT, Logic_Value::ZERO }) == Logic_Value::ONE);
	CHECK(NAND(vec{ Logic_Value::UNKNOWN, Logic_Value::ZERO }) == Logic_Value::ONE);

	CHECK(NAND(vec{ Logic_Value::ZERO, Logic_Value::ONE }) == Logic_Value::ONE);
	CHECK(NAND(vec{ Logic_Value::HI_Z, Logic_Value::ONE }) == Logic_Value::UNKNOWN);
	CHECK(NAND(vec{ Logic_Value::CONFLICT, Logic_Value::ONE }) == Logic_Value::UNKNOWN);
	CHECK(NAND(vec{ Logic_Value::UNKNOWN, Logic_Value::ONE }) == Logic_Value::UNKNOWN);

	CHECK(NAND(vec{ Logic_Value::ZERO, Logic_Value::HI_Z }) == Logic_Value::ONE);
	CHECK(NAND(vec{ Logic_Value::ONE, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);
	CHECK(NAND(vec{ Logic_Value::CONFLICT, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);
	CHECK(NAND(vec{ Logic_Value::UNKNOWN, Logic_Value::HI_Z }) == Logic_Value::UNKNOWN);

	CHECK(NAND(vec{ Logic_Value::ZERO, Logic_Value::CONFLICT }) == Logic_Value::ONE);
	CHECK(NAND(vec{ Logic_Value::ONE, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);
	CHECK(NAND(vec{ Logic_Value::HI_Z, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);
	CHECK(NAND(vec{ Logic_Value::UNKNOWN, Logic_Value::CONFLICT }) == Logic_Value::UNKNOWN);

	CHECK(NAND(vec{ Logic_Value::ZERO, Logic_Value::UNKNOWN }) == Logic_Value::ONE);
	CHECK(NAND(vec{ Logic_Value::ONE, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);
	CHECK(NAND(vec{ Logic_Value::HI_Z, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);
	CHECK(NAND(vec{ Logic_Value::CONFLICT, Logic_Value::UNKNOWN }) == Logic_Value::UNKNOWN);
}

TEST_CASE("Tri-state buffer high-active test", "[LOGIC]") {
	CHECK(tri_state_buffer_high_active(Logic_Value::ZERO, Logic_Value::ZERO) == Logic_Value::HI_Z);
	CHECK(tri_state_buffer_high_active(Logic_Value::ZERO, Logic_Value::ONE) == Logic_Value::ZERO);
	CHECK(tri_state_buffer_high_active(Logic_Value::ONE, Logic_Value::ZERO) == Logic_Value::HI_Z);
	CHECK(tri_state_buffer_high_active(Logic_Value::ONE, Logic_Value::ONE) == Logic_Value::ONE);
}

TEST_CASE("Tri-state buffer low-active test", "[LOGIC]") {
	CHECK(tri_state_buffer_low_active(Logic_Value::ZERO, Logic_Value::ZERO) == Logic_Value::ZERO);
	CHECK(tri_state_buffer_low_active(Logic_Value::ZERO, Logic_Value::ONE) == Logic_Value::HI_Z);
	CHECK(tri_state_buffer_low_active(Logic_Value::ONE, Logic_Value::ZERO) == Logic_Value::ONE);
	CHECK(tri_state_buffer_low_active(Logic_Value::ONE, Logic_Value::ONE) == Logic_Value::HI_Z);
}

TEST_CASE("Tri-state buffer high-active equivalent to low-active NOT(enable)", "[LOGIC]") {
	CHECK(tri_state_buffer_low_active(Logic_Value::ZERO, Logic_Value::ZERO) == tri_state_buffer_high_active(Logic_Value::ZERO, Logic_Value::ONE));
	CHECK(tri_state_buffer_low_active(Logic_Value::ZERO, Logic_Value::ONE) == tri_state_buffer_high_active(Logic_Value::ZERO, Logic_Value::ZERO));
	CHECK(tri_state_buffer_low_active(Logic_Value::ONE, Logic_Value::ZERO) == tri_state_buffer_high_active(Logic_Value::ONE, Logic_Value::ONE));
	CHECK(tri_state_buffer_low_active(Logic_Value::ONE, Logic_Value::ONE) == tri_state_buffer_high_active(Logic_Value::ONE, Logic_Value::ZERO));
}

TEST_CASE("JK next Q logic is sound", "[LOGIC]") {
	CHECK(JK_next_Q(Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO) == Logic_Value::ZERO);
	CHECK(JK_next_Q(Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ONE) == Logic_Value::ZERO);
	CHECK(JK_next_Q(Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ZERO) == Logic_Value::ZERO);
	CHECK(JK_next_Q(Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ONE) == Logic_Value::ZERO);
	CHECK(JK_next_Q(Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ZERO) == Logic_Value::ONE);
	CHECK(JK_next_Q(Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ONE) == Logic_Value::ONE);
	CHECK(JK_next_Q(Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ZERO) == Logic_Value::ONE);
	CHECK(JK_next_Q(Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE) == Logic_Value::ZERO);
}

TEST_CASE("MUX 2x1 sets output correctly", "[LOGIC]") {
	CHECK(mux(vec{ Logic_Value::ONE, Logic_Value::ZERO }, vec{ Logic_Value::ZERO }) == Logic_Value::ONE);
	CHECK(mux(vec{ Logic_Value::ONE, Logic_Value::ZERO }, vec{ Logic_Value::ONE }) == Logic_Value::ZERO);
}

TEST_CASE("MUX 4x2 sets output correctly", "[LOGIC]") {
	vec all_high = { Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE };
	vec all_low = { Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO };
	vec select_0 = { Logic_Value::ZERO, Logic_Value::ZERO };
	vec select_3 = { Logic_Value::ONE, Logic_Value::ONE };

	CHECK(mux(all_low, select_0) == Logic_Value::ZERO);
	CHECK(mux(all_low, select_3) == Logic_Value::ZERO);
	all_low[0] = Logic_Value::ONE;
	CHECK(mux(all_low, select_0) == Logic_Value::ONE);
	all_low[0] = Logic_Value::ZERO;
	all_low[3] = Logic_Value::ONE;
	CHECK(mux(all_low, select_3) == Logic_Value::ONE);

	CHECK(mux(all_high, select_0) == Logic_Value::ONE);
	CHECK(mux(all_high, select_3) == Logic_Value::ONE);
	all_high[0] = Logic_Value::ZERO;
	CHECK(mux(all_high, select_0) == Logic_Value::ZERO);
	all_high[0] = Logic_Value::ONE;
	all_high[3] = Logic_Value::ZERO;
	CHECK(mux(all_high, select_3) == Logic_Value::ZERO);
}

TEST_CASE("MUX 4x2 MSB/LSB is interpreted correctly", "[LOGIC]") {
	vec all_low = { Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO };
	vec select_1 = { Logic_Value::ONE, Logic_Value::ZERO }
	;
	CHECK(mux(all_low, select_1) == Logic_Value::ZERO);
	all_low[1] = Logic_Value::ONE;
	CHECK(mux(all_low, select_1) == Logic_Value::ONE);
}

TEST_CASE("MUX 16x4 sets output correctly", "[LOGIC]") {
	vec all_high = { Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE,
					Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE,
					Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE,
					Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE };

	vec all_low = { Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO,
					Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO,
					Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO,
					Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO };

	vec select_0 = { Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO };
	vec select_7 = { Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ZERO };
	vec select_A = { Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ONE };
	vec select_F = { Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE };


	CHECK(mux(all_high, select_0) == Logic_Value::ONE);
	all_high[0] = Logic_Value::ZERO;
	CHECK(mux(all_high, select_0) == Logic_Value::ZERO);
	all_high[0] = Logic_Value::ONE;

	CHECK(mux(all_high, select_7) == Logic_Value::ONE);
	all_high[7] = Logic_Value::ZERO;
	CHECK(mux(all_high, select_7) == Logic_Value::ZERO);
	all_high[7] = Logic_Value::ONE;

	CHECK(mux(all_high, select_A) == Logic_Value::ONE);
	all_high[10] = Logic_Value::ZERO;
	CHECK(mux(all_high, select_A) == Logic_Value::ZERO);
	all_high[10] = Logic_Value::ONE;

	CHECK(mux(all_high, select_F) == Logic_Value::ONE);
	all_high[15] = Logic_Value::ZERO;
	CHECK(mux(all_high, select_F) == Logic_Value::ZERO);
	all_high[15] = Logic_Value::ONE;
}

TEST_CASE("Decoder lights up the right pin", "[LOGIC]") {
	CHECK(decode(vec{ Logic_Value::ZERO, Logic_Value::ZERO }, Logic_Value::ONE)[0] == Logic_Value::ONE);
	CHECK(decode(vec{ Logic_Value::ONE, Logic_Value::ZERO }, Logic_Value::ONE)[1] == Logic_Value::ONE);
	CHECK(decode(vec{ Logic_Value::ZERO, Logic_Value::ONE }, Logic_Value::ONE)[2] == Logic_Value::ONE);
	CHECK(decode(vec{ Logic_Value::ONE, Logic_Value::ONE }, Logic_Value::ONE)[3] == Logic_Value::ONE);
	CHECK(decode(vec{ Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ZERO }, Logic_Value::ONE)[3] == Logic_Value::ONE);
	CHECK(decode(vec{ Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE }, Logic_Value::ONE)[7] == Logic_Value::ONE);
	CHECK(decode(vec{ Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE }, Logic_Value::ZERO)[7] == Logic_Value::ZERO);
}

TEST_CASE("Decoder enable pin works", "[LOGIC]") {
	CHECK(decode(vec{ Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE }, Logic_Value::ONE)[7] == Logic_Value::ONE);
	CHECK(decode(vec{ Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE }, Logic_Value::ZERO)[7] == Logic_Value::ZERO);
	CHECK(decode(vec{ Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE }, Logic_Value::HI_Z)[7] == Logic_Value::ZERO);
	CHECK(decode(vec{ Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE }, Logic_Value::CONFLICT)[7] == Logic_Value::ZERO);
	CHECK(decode(vec{ Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE }, Logic_Value::UNKNOWN)[7] == Logic_Value::ZERO);
}

TEST_CASE("nMOS transistor test", "[LOGIC]") {
	CHECK(simple_nMOS(Logic_Value::ZERO, Logic_Value::ZERO) == Logic_Value::HI_Z);
	CHECK(simple_nMOS(Logic_Value::ZERO, Logic_Value::ONE) == Logic_Value::ZERO);
	CHECK(simple_nMOS(Logic_Value::ONE, Logic_Value::ZERO) == Logic_Value::HI_Z);
	CHECK(simple_nMOS(Logic_Value::ONE, Logic_Value::ONE) == Logic_Value::ONE);
}

TEST_CASE("pMOS transistor test", "[LOGIC]") {
	CHECK(simple_pMOS(Logic_Value::ZERO, Logic_Value::ZERO) == Logic_Value::ZERO);
	CHECK(simple_pMOS(Logic_Value::ZERO, Logic_Value::ONE) == Logic_Value::HI_Z);
	CHECK(simple_pMOS(Logic_Value::ONE, Logic_Value::ZERO) == Logic_Value::ONE);
	CHECK(simple_pMOS(Logic_Value::ONE, Logic_Value::ONE) == Logic_Value::HI_Z);
}

TEST_CASE("increment counts up", "[LOGIC]") {
	vec seven = { Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE };
	vec six = { Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ONE };
	vec five = { Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ONE };
	vec four = { Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ONE };
	vec three = { Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ZERO };
	vec two = { Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ZERO };
	vec one = { Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ZERO };
	vec zero = { Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO };

	auto ret = increment(zero);
	CHECK(ret.first == one);
	CHECK(ret.second == Logic_Value::ZERO);

	ret = increment(one);
	CHECK(ret.first == two);
	CHECK(ret.second == Logic_Value::ZERO);

	ret = increment(two);
	CHECK(ret.first == three);
	CHECK(ret.second == Logic_Value::ZERO);

	ret = increment(three);
	CHECK(ret.first == four);
	CHECK(ret.second == Logic_Value::ZERO);

	ret = increment(four);
	CHECK(ret.first == five);
	CHECK(ret.second == Logic_Value::ZERO);

	ret = increment(five);
	CHECK(ret.first == six);
	CHECK(ret.second == Logic_Value::ZERO);

	ret = increment(six);
	CHECK(ret.first == seven);
	CHECK(ret.second == Logic_Value::ZERO);

	ret = increment(seven);
	CHECK(ret.first == zero);
	CHECK(ret.second == Logic_Value::ONE);
}

TEST_CASE("decrement counts down", "[LOGIC]") {
	vec seven = { Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE };
	vec six = { Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ONE };
	vec five = { Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ONE };
	vec four = { Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ONE };
	vec three = { Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ZERO };
	vec two = { Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ZERO };
	vec one = { Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ZERO };
	vec zero = { Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO };

	auto ret = decrement(zero);
	CHECK(ret.first == seven);
	CHECK(ret.second == Logic_Value::ZERO);

	ret = decrement(seven);
	CHECK(ret.first == six);
	CHECK(ret.second == Logic_Value::ZERO);

	ret = decrement(six);
	CHECK(ret.first == five);
	CHECK(ret.second == Logic_Value::ZERO);

	ret = decrement(five);
	CHECK(ret.first == four);
	CHECK(ret.second == Logic_Value::ZERO);

	ret = decrement(four);
	CHECK(ret.first == three);
	CHECK(ret.second == Logic_Value::ZERO);

	ret = decrement(three);
	CHECK(ret.first == two);
	CHECK(ret.second == Logic_Value::ZERO);

	ret = decrement(two);
	CHECK(ret.first == one);
	CHECK(ret.second == Logic_Value::ZERO);

	ret = decrement(one);
	CHECK(ret.first == zero);
	CHECK(ret.second == Logic_Value::ZERO);
}

TEST_CASE("Counting Register responds to inputs and controlls", "[LOGIC]") {
	// From increment test we know that the roll-over behavior is correct
	vec seven = { Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE };
	vec six = { Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ONE };
	vec five = { Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ONE };
	vec four = { Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ONE };
	vec three = { Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ZERO };
	vec two = { Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ZERO };
	vec one = { Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ZERO };
	vec zero = { Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO };

	// Show no signal -> hold value for both output and carry-out
	CHECK(counting_register(std::make_pair(five, Logic_Value::ZERO), three, Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO).first == five);
	CHECK(counting_register(std::make_pair(five, Logic_Value::ZERO), three, Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO).second == Logic_Value::ZERO);
	CHECK(counting_register(std::make_pair(five, Logic_Value::ONE), three, Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO).first == five);
	CHECK(counting_register(std::make_pair(five, Logic_Value::ONE), three, Logic_Value::ZERO, Logic_Value::ZERO, Logic_Value::ZERO).second == Logic_Value::ONE);

	// Show load
	CHECK(counting_register(std::make_pair(five, Logic_Value::ONE), three, Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ZERO).first == three);
	// Show carry-out cleared with load
	CHECK(counting_register(std::make_pair(five, Logic_Value::ZERO), three, Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ZERO).second == Logic_Value::ZERO);
	CHECK(counting_register(std::make_pair(five, Logic_Value::ONE), three, Logic_Value::ONE, Logic_Value::ZERO, Logic_Value::ZERO).second == Logic_Value::ZERO);
	// Show load takes priority over other count w or w/o up
	CHECK(counting_register(std::make_pair(five, Logic_Value::ONE), three, Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ZERO).first == three);
	CHECK(counting_register(std::make_pair(five, Logic_Value::ZERO), three, Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ZERO).second == Logic_Value::ZERO);
	CHECK(counting_register(std::make_pair(five, Logic_Value::ONE), three, Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ZERO).second == Logic_Value::ZERO);
	CHECK(counting_register(std::make_pair(five, Logic_Value::ONE), three, Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE).first == three);
	CHECK(counting_register(std::make_pair(five, Logic_Value::ZERO), three, Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE).second == Logic_Value::ZERO);
	CHECK(counting_register(std::make_pair(five, Logic_Value::ONE), three, Logic_Value::ONE, Logic_Value::ONE, Logic_Value::ONE).second == Logic_Value::ZERO);

	// Show counting if load off goes up or down according to up value
	CHECK(counting_register(std::make_pair(five, Logic_Value::ZERO), three, Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ZERO).first == four);
	CHECK(counting_register(std::make_pair(five, Logic_Value::ZERO), three, Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ZERO).second == Logic_Value::ZERO);

	CHECK(counting_register(std::make_pair(five, Logic_Value::ONE), three, Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ONE).first == six);
	CHECK(counting_register(std::make_pair(five, Logic_Value::ONE), three, Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ONE).second == Logic_Value::ZERO);

	// Show carry-out if rolled over top
	CHECK(counting_register(std::make_pair(seven, Logic_Value::ZERO), three, Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ONE).first == zero);
	CHECK(counting_register(std::make_pair(seven, Logic_Value::ZERO), three, Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ONE).second == Logic_Value::ONE);

	// Show no carry-out if rolled under
	CHECK(counting_register(std::make_pair(zero, Logic_Value::ZERO), three, Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ZERO).first == seven);
	CHECK(counting_register(std::make_pair(zero, Logic_Value::ZERO), three, Logic_Value::ZERO, Logic_Value::ONE, Logic_Value::ZERO).second == Logic_Value::ZERO);

}	
