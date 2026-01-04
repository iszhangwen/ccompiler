#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <iostream>
#include <doctest/doctest.h>

int fun()
{
    std::cout << "hello world\n";
    return 0;
}

TEST_SUITE("test start")
{
    TEST_CASE("test hello world")
    {
        SUBCASE("test 001")
        {
            CHECK(fun() == 0);
        }
        SUBCASE("test 002")
        {
            CHECK(fun() != 1);
        }
    }
}

