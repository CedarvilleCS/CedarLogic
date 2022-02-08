#include "entities.hpp"

bool Network::has_junction(junction_ref j) const
{
    return outputs.find(j) != outputs.end() || inputs.find(j) != inputs.end();
}