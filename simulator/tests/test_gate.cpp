#include "gate.hpp"
#include <catch2/catch_test_macros.hpp>
#include <stdint.h>

// Only bother to test out the AND gate since the logic is tested elsewhere.
TEST_CASE("process works", "[GATE]") {
  std::vector<Input *> in_js;

  for (int i = 0; i < 4; i++) {
    Input *in = new Input();
    in->state = Logic_Value::ZERO;
    in_js.push_back(in);
  }

  Output *out_j = new Output();

  GateAND and_g(in_js, out_j);

  for (int i = 0; i < in_js.size(); i++) {
    and_g.process();
    CHECK(out_j->state == Logic_Value::ZERO);
    in_js[i]->state = Logic_Value::ONE;
  }
  and_g.process();
  CHECK(out_j->state == Logic_Value::ONE);

  for (int i = 0; i < 4; i++) {
    delete in_js[i];
    in_js[i] = nullptr;
  }
}