#include <catch2/catch_test_macros.hpp>
#include <stdint.h>
#include "junction.hpp"

TEST_CASE("to_values works", "[JUNCTION]")
{
    std::vector<Junction *> js;

    Input* ptr;

    for (int i = 0; i < 15; i++)
    {
        switch (i)
        {
        case 5:
            ptr = new Input(0);
            ptr->state = Logic_Value::ZERO;
            js.push_back(ptr);
            break;
        case 7:
            ptr = new Input(0);
            ptr->state = Logic_Value::ONE;
            js.push_back(ptr);
            break;
        case 13:
            ptr = new Input(0);
            ptr->state = Logic_Value::CONFLICT;
            js.push_back(ptr);
            break;
        default:
            js.push_back(new Input(0));
            break;
        }
    }

    auto res = Junction::to_values(js);

    for (int i = 0; i < 15; i++)
    {
        switch (i)
        {
        case 5:
            CHECK(res[i] == Logic_Value::ZERO);
            break;
        case 7:
            CHECK(res[i] == Logic_Value::ONE);
            break;
        case 13:
            CHECK(res[i] == Logic_Value::CONFLICT);
            break;
        default:
            CHECK(res[i] == Logic_Value::HI_Z);
            break;
        }
    }

    // Just to be clean, let's delete that memory now:
    for (int i = 0; i < 15; i++)
    {
        delete js[i];
    }
}