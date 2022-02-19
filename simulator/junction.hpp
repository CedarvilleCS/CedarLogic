#pragma once

#include "logic.hpp"

/**
 * @brief Junctions are what connect wires, provide circuit enties with their input 
 * value(s) and hold the entities output value(s).
 * 
 * Junction is an abstract struct, which is implemented as either an Input or Output
 * junction.
 */
struct Junction {
public:

    /**
     * @brief All subclass types.
     * 
     */
	enum class Type {
		Input, Output
	};

	/**
	 * @brief Forbids constructing a new Junction object without specifying it's type
	 * 
	 */
	Junction() = delete;

	/**
	 * @brief Get the type object
	 * 
     * Use when you need to static_cast a Junction ptr to an Input or Output
     * or any other time you need to know which child class it is.
     * 
	 * @return const Type - The Junction subclass type.
	 */
	const Type get_type() const {
		return type;
	}

    /**
     * @brief the logic state of the network
     * 
     * If an input junction, then this is set when the network is refreshed.
     * If an output junction, then it is set by whoever owns the output (like a gate).
     * 
     * @note it is up to the Junction instance owner to protect access to the junctin state.
     */
	Logic_Value state;

    /**
     * @brief the junction's circuit index 
     * 
     * Useful to provide a quick ptr -> index mapping.
     * 
     * @note it is const so it cannot be modified after construction.
     */
    const uint32_t index_id;

protected:

	/**
	 * @brief Construct a new Junction object.
     * 
     * Note that only child-classes can call this constructor.
	 * 
	 * @param type the type of junction, used so we can always safely downcast.
	 */
	Junction(Type type, uint32_t id) : type(type), state(Logic_Value::HI_Z), index_id(id) {}

private:
    
    /**
     * @brief The type attribute facillitates safe static downcasting.
     * 
     */
	Type type;
};

/**
 * @brief An Output junction is one who's state is set by a gate, register, etc.
 * 
 * An Output junction's value is considered driven and therefore if two Output 
 * junctions on the same wire network are driving opposite values the network 
 * can enter the Logic_Value::CONFLICT state.
 */
struct Output : public Junction {
public:

    /**
     * @brief Construct a new Output object - deleted so we can always have id.
     * 
     */
    Output() = delete;

	/**
	 * @brief Construct a new Output object
	 * 
	 * @param id the junction's numerical id 
	 */
	Output(uint32_t id) : Junction(Junction::Type::Output, id) {};
};

/**
 * @brief An input junction is one who's state is set by it's network.
 * 
 * The input junction's state is passive, being set by the network rather than 
 * having any part in determining the network's state.
 */
struct Input : public Junction {
public:

    /**
     * @brief Construct a new Input object - deleted so we can alway shave id.
     * 
     */
    Input() = delete;

	/**
	 * @brief Construct a new Input object
	 * 
	 * @param id the junction's numerical id 
	 */
	Input(uint32_t id) : Junction(Junction::Type::Input, id) {};
};
