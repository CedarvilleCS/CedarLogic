#include "entities.hpp"

bool Network::has_junction(uint32_t j) const
{
    return output_junction_indexes.find(j) != output_junction_indexes.end() || 
        input_junction_indexes.find(j) != input_junction_indexes.end();
}