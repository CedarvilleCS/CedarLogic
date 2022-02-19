#include "junction.hpp"

#include <stdint.h>

std::vector<Logic_Value>
Junction::to_values(const std::vector<Junction *> &js) {
  std::vector<Logic_Value> ret;

  for (uint32_t i = 0; i < js.size(); i++) {
    ret.push_back(js[i]->state);
  }

  return ret;
}

std::vector<Logic_Value> Input::to_values(const std::vector<Input *> &js) {
  std::vector<Logic_Value> ret;

  for (uint32_t i = 0; i < js.size(); i++) {
    ret.push_back(js[i]->state);
  }

  return ret;
}

std::vector<Logic_Value> Output::to_values(const std::vector<Output *> &js) {
  std::vector<Logic_Value> ret;

  for (uint32_t i = 0; i < js.size(); i++) {
    ret.push_back(js[i]->state);
  }

  return ret;
}