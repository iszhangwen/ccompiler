#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "func.h"


TEST_SUITE("test start")
{
    TEST_CASE("test hello world")
    {
        SUBCASE("test 001")
        {
            CHECK(fun() == 1);
        }
        SUBCASE("test 002")
        {
            CHECK(fun() != 1);
        }
    }
}

