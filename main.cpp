#include "timeseries_logger.h"
#include <iostream>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using namespace TimeSeriesLogger;

TEST_CASE( "Build tree", "TimeSeriesLogger" )
//int main(int argc, char** argv)
{
    TreeRoot logger;
    REQUIRE( logger.children().size() == 0);

    auto first = logger.addChild<uint16_t>("first");
    REQUIRE( logger.children().size() == 1);
    REQUIRE( logger.rawBuffer().size() == 2);

    auto second = logger.addChild<int16_t>("second");
    REQUIRE( logger.children().size() == 2);
    REQUIRE( logger.rawBuffer().size() == 4);

    // it is not allowed to add children with the same name
    REQUIRE_THROWS( logger.addChild<int>("second") );
    REQUIRE( logger.children().size() == 2);
    REQUIRE( logger.rawBuffer().size() == 4);

    auto A = logger.addChild<int32_t>("A", first);
    auto B = logger.addChild<int32_t>("B", first);
    auto C = logger.addChild<int32_t>("C", first);

    REQUIRE( logger.allNodes().size() == 5);
    REQUIRE( logger.children().size() == 2);
    REQUIRE( first->children().size()  == 3);
    REQUIRE( second->children().size() == 0);
    REQUIRE( logger.rawBuffer().size() == 16);

    first->set(0x1111);
    second->set(0x2222);
    A->set(0x33334444);
    B->set(0x55556666);
    C->set(0x77778888);

    REQUIRE( first->get() == (0x1111));
    REQUIRE( second->get() ==(0x2222));
    REQUIRE( A->get() == (0x33334444));
    REQUIRE( B->get() == (0x55556666));
    REQUIRE( C->get() == (0x77778888));
}
