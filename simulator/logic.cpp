#include "logic.hpp"
#include <cmath>
#include <cassert>

namespace logic {
	const Logic_Value AND(std::vector<Logic_Value> inputs)
	{
		bool hasLow = false;
		bool hasOdd = false;
		for (auto i = 0; i < inputs.size(); i++) {
			if (inputs[i] == Logic_Value::ZERO) hasLow = true;
			else if (inputs[i] != Logic_Value::ONE) hasOdd = true;
		}
		if (hasLow) return Logic_Value::ZERO;
		else if (hasOdd) return Logic_Value::UNKNOWN;
		else return Logic_Value::ONE; // all inputs were high
	}

	const Logic_Value OR(std::vector<Logic_Value> inputs)
	{
		bool hasHigh = false;
		bool hasOdd = false;
		for (auto i = 0; i < inputs.size(); i++) {
			if (inputs[i] == Logic_Value::ONE) hasHigh = true;
			else if (inputs[i] != Logic_Value::ZERO) hasOdd = true;
		}
		if (hasHigh) return Logic_Value::ONE;
		else if (hasOdd) return Logic_Value::UNKNOWN;
		else return Logic_Value::ZERO; // all inputs were low
	}

	Logic_Value xor_two(Logic_Value a, Logic_Value b) {
		bool a_is_valid = (a == Logic_Value::ONE || a == Logic_Value::ZERO);
		bool b_is_valid = (b == Logic_Value::ONE || b == Logic_Value::ZERO);
		if (a_is_valid && b_is_valid) {
			if (a != b) return Logic_Value::ONE;
			else return Logic_Value::ZERO;
		}
		else {
			return Logic_Value::UNKNOWN;
		}
	}

	const Logic_Value XOR(std::vector<Logic_Value> inputs)
	{
		// TODO: Clarify correct formula
		auto size = inputs.size();
		switch (size) {
		case 0:
			// TODO: log warning
			return Logic_Value::UNKNOWN;
		case 1:
			// Can be reasonably encountered if 3 inputs
			return inputs[0];
		case 2:
			return xor_two(inputs[0], inputs[1]);
		default:
			// Split input in half and solve recursively
			auto half_size = size / 2;
			std::vector<Logic_Value> first_half;
			for (std::size_t i = 0; i < half_size; i++) {
				first_half.push_back(inputs[i]);
			}
			std::vector<Logic_Value> second_half;
			for (std::size_t i = half_size; i < size; i++) {
				second_half.push_back(inputs[i]);
			}
			return xor_two(XOR(first_half), XOR(second_half));
		}
	}

	const Logic_Value NAND(std::vector<Logic_Value> inputs)
	{
		return NOT(AND(inputs));
	}

	const Logic_Value NOR(std::vector<Logic_Value> inputs)
	{
		return NOT(OR(inputs));
	}

	const Logic_Value NOT(Logic_Value input)
	{
		if (input == Logic_Value::ONE) return Logic_Value::ZERO;
		else if (input == Logic_Value::ZERO) return Logic_Value::ONE;
		else return Logic_Value::UNKNOWN;
	}

	const Logic_Value tri_state_buffer_high_active(Logic_Value input, Logic_Value enable)
	{
		if (enable == Logic_Value::ONE) {
			return input;
		}
		else {
			return Logic_Value::HI_Z;
		}
	}

	const Logic_Value tri_state_buffer_low_active(Logic_Value input, Logic_Value enable)
	{
		return tri_state_buffer_high_active(input, NOT(enable));
	}

	const Logic_Value JK_next_Q(Logic_Value j, Logic_Value k, Logic_Value q)
	{
		if (j == Logic_Value::ZERO && k == Logic_Value::ZERO) {
			// latch
			return Logic_Value::ZERO;
		}
		else if (j == Logic_Value::ONE && k != Logic_Value::ONE) {
			// set
			return Logic_Value::ONE;
		}
		else if (j != Logic_Value::ONE && k == Logic_Value::ONE) {
			// reset
			return Logic_Value::ZERO;
		}
		else if (j == Logic_Value::ONE && k == Logic_Value::ONE) {
			// toggle
			return NOT(q);
		}
		else {
			// To much undefined (UNKNOWN) input
			return Logic_Value::UNKNOWN;
		}
	}

	const Logic_Value mux(std::vector<Logic_Value> values, std::vector<Logic_Value> select) {
		auto power = select.size();
		if (values.size() != pow(2, power)) {
			// TODO: log error
			// Possibly push error event for user to see too.
			return Logic_Value::UNKNOWN;
		}
		else {
			switch (power) {
			case 0:
				return Logic_Value::UNKNOWN;
			case 1:
				if (select[0] == Logic_Value::ONE) return values[1];
				else return values[0];
			default:
				std::vector<Logic_Value> top_half;
				std::vector<Logic_Value> bottom_half;
				std::vector<Logic_Value> new_select;
				double tmp = pow(2, (power - 1));
				assert(tmp < UINT16_MAX);
				uint16_t half_size = (uint16_t)tmp;
				for (int32_t i = 0; i < half_size; i++) {
					bottom_half.push_back(values[i]);
				}
				for (uint16_t i = half_size; i < pow(2, power); i++) {
					top_half.push_back(values[i]);
				}
				for (uint16_t i = 0; i < power - 1; i++) {
					new_select.push_back(select[i]);
				}
				auto top_result = mux(top_half, new_select);
				auto bottom_result = mux(bottom_half, new_select);
				return mux(std::vector<Logic_Value>{bottom_result, top_result}, std::vector<Logic_Value>{select[power - 1]});
			}
		}
	}

	const std::vector<Logic_Value> decode(std::vector<Logic_Value> input, Logic_Value enable)
	{
		auto tmp = pow(2, input.size());
		assert(tmp < INT32_MAX);
		int32_t output_size = (int32_t)tmp;
		std::vector<Logic_Value> output;
		for (int32_t i = 0; i < output_size; i++) {
			output.push_back(Logic_Value::ZERO);
		}
		if (enable == Logic_Value::ONE) {
			int32_t num = 0;
			for (int32_t i = 0; i < input.size(); i++) {
				if (input[i] == Logic_Value::ONE) {
					num += pow(2, i);
				}
			}
			output[num] = Logic_Value::ONE;
			return output;
		}
		else {
			return output;
		}
	}
	const Logic_Value simple_nMOS(Logic_Value source, Logic_Value gate)
	{
		return tri_state_buffer_high_active(source, gate);
	}
	const Logic_Value simple_pMOS(Logic_Value source, Logic_Value gate)
	{
		return tri_state_buffer_low_active(source, gate);
	}
	// Increment the bits, returns vector of equal size with carry-out bit
	// TODO: Confirm CedarLogic 2.3.5 is a bug when lights carry-out on F instead of 0 during increment
	std::pair<std::vector<Logic_Value>, Logic_Value> increment(std::vector <Logic_Value> inputs) {
		int32_t i = 0;
		bool set_one = false;
		auto carry_out = Logic_Value::ZERO;
		while (set_one == false) {
			if (inputs[i] != Logic_Value::ONE) {
				inputs[i] = Logic_Value::ONE;
				set_one = true;
			}
			else {
				inputs[i] = Logic_Value::ZERO;
			}
			if (!set_one && i == inputs.size() - 1) {
				carry_out = Logic_Value::ONE;
				set_one = true;
			}
			i++;
		}
		return std::make_pair(inputs, carry_out);
	}
	// Decrement the bits, returns vector of equal size with carry-out bit
	// TODO: Confirm CedarLogic 2.3.5 is bug when lights carry-out at zero during decrement
	// TODO: carry-out should never light up during decrement?
	std::pair<std::vector<Logic_Value>, Logic_Value> decrement(std::vector <Logic_Value> inputs) {
		int32_t i = 0;
		bool set_one = false;
		auto carry_out = Logic_Value::ZERO;
		while (set_one == false) {
			if (inputs[i] != Logic_Value::ZERO) {
				inputs[i] = Logic_Value::ZERO;
				set_one = true;
			}
			else {
				inputs[i] = Logic_Value::ONE;
			}
			if (i == inputs.size() - 1) {
				carry_out = Logic_Value::ZERO;
				set_one = true;
			}
			i++;
		}
		return std::make_pair(inputs, carry_out);
	}
	const std::pair<std::vector<Logic_Value>, Logic_Value> counting_register(std::pair<std::vector<Logic_Value>, Logic_Value> output, std::vector<Logic_Value> inputs, Logic_Value load, Logic_Value count, Logic_Value up)
	{
		if (load == Logic_Value::ONE) {
			return std::make_pair(inputs, Logic_Value::ZERO);
		}
		else if (count == Logic_Value::ONE) {
			if (up == Logic_Value::ONE) {
				return increment(output.first);
			}
			else {
				return decrement(output.first);
			}  
		}
		else {
			return output;
		}
	}
}
