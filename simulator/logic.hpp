#pragma once

#include <vector>

enum class Logic_Value {
	ZERO, ONE, HI_Z, CONFLICT, UNKNOWN
};

namespace logic {
	
	// Performs a logical AND across the input values
	Logic_Value AND(std::vector<Logic_Value> inputs);

	// Performs a logical OR across the input values
	Logic_Value OR(std::vector<Logic_Value> inputs);

	// Performs a logical XOR across the input values
	Logic_Value XOR(std::vector<Logic_Value> inputs);

	// Performs a logical NAND across the input values
	Logic_Value NAND(std::vector<Logic_Value> inputs);

	// Performs a logical NOR across the input values
	Logic_Value NOR(std::vector<Logic_Value> inputs);

	// Performs a logical NOT of the input value
	Logic_Value NOT(Logic_Value input);

	// Tri-state buffer with high-active enable
	Logic_Value tri_state_buffer_high_active(Logic_Value input, Logic_Value enable);

	// Tri-state buffer with low-active enable
	Logic_Value tri_state_buffer_low_active(Logic_Value input, Logic_Value enable);

	// Get the next Q value for a JK Flip-Flop given it's J, K, and Q values.
	Logic_Value JK_next_Q(Logic_Value j, Logic_Value k, Logic_Value q);

	// Get the output value for the mux. values.size() == pow(2, select.size())
	// Note that the 0th index is the 0th bit and so on. Same with select.
	Logic_Value mux(std::vector<Logic_Value> values, std::vector<Logic_Value> select);

	// Decoder, given binary number on the input, it lights up the ordinal output pin.
	// Returns vector with size == pow(2, input.size())
	std::vector<Logic_Value> decode(std::vector<Logic_Value> input, Logic_Value enable);

	// Simplified nMOS transistor
	Logic_Value simple_nMOS(Logic_Value source, Logic_Value gate);

	// Simplified pMOS transistor
	Logic_Value simple_pMOS(Logic_Value source, Logic_Value gate);

	// Increments the binary number, LSB is index 0
	// @return incremented binary number and carry out value
	std::pair<std::vector<Logic_Value>, Logic_Value> increment(std::vector <Logic_Value> inputs);

	// Decrements the binary number, LSB is index 0
	// @return decremented binary number and carry out value
	std::pair<std::vector<Logic_Value>, Logic_Value> decrement(std::vector <Logic_Value> inputs);

	// Register
	// Returns the outputs and the carry-out signal as a std::pair
	std::pair<std::vector<Logic_Value>, Logic_Value> counting_register(std::pair<std::vector<Logic_Value>, Logic_Value> output, std::vector<Logic_Value> inputs, Logic_Value load, Logic_Value count, Logic_Value up);
}