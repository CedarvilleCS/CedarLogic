#pragma once

#include <stdint.h>
#include <vector>
#include "logic.hpp"
#include "junction.hpp"

/**
 * @brief All gate types, used during Gate creation to pass the right logic_function to it.
 * 
 */
enum class Gates {
	AND, XOR, OR, NAND, NOR // XNOR not implemented
};

/**
 * @brief The logic gate, all have multiple inputs and one output.
 */
struct Gate {
public:
	
    /**
     * @brief Construct a new Gate object - deleted because all gate objects must have their junctions and logic function ptr. 
     */
	Gate() = delete;

	
    /**
     * @brief Construct a new Gate object
     * 
     * @param logicFunc implements the logic function taking a vector of input values and producing the output value.
     * @param input_junction_ptrs 
     * @param output_junction_ptr 
     */
	Gate(const Logic_Value(*logicFunc)(std::vector<Logic_Value>), std::vector<const Junction*> input_junction_ptrs, Junction* output_junction_ptr) :
		logicFunction(logicFunc),
		input_junction_ptrs(input_junction_ptrs),
		output_junction_ptr(output_junction_ptr)
		{}

	/**
	 * @brief gets input junction states and sets output junction state.
	 * 
	 */
    void process();

    /**
     * @brief Input junctions
     * 
     * LSB is index 0, MSB is index = size() - 1.
     * 
     * @note the vector is const, so it cannot be modified after construction.
     * @note the Junction pointers are also const, because input junctions are never modified by a gate.
     */
	const std::vector<const Junction*> input_junction_ptrs;
		
	/**
	 * @brief Output junctions
	 * 
     * @note the vector is const, so it cannot be modified after construction.
     * @note the Junction pointers are not const, because Output junction state is set in the process function.
	 */
	const Junction* output_junction_ptr;

private:
	const Logic_Value(*logicFunction)(std::vector<Logic_Value>);
};