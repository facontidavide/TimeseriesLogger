#include "timeseries_logger.h"
#include <iostream>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using namespace tsl;

TEST_CASE( "Build tree", "TimeSeriesLogger" )
//int main(int argc, char** argv)
{
    TimeseriesLoggerRoot logger;
    REQUIRE( logger.nodesCount() == 0);

    auto first = logger.createChild<double>("first");
    REQUIRE( logger.nodesCount() == 1);

    auto second = logger.createChild<int>("second");
    REQUIRE( logger.nodesCount() == 2);

    // it is not allowed to add children with the same name
    REQUIRE_THROWS( logger.createChild<int>("second") );
    REQUIRE( logger.nodesCount() == 2);

    auto A = first->createChild<float>("A");
    auto B = first->createChild<float>("B");
    auto C = first->createChild<float>("C");

    REQUIRE( logger.nodesCount() == 5);
    REQUIRE( logger.childrenIndexes().size() == 2);
    REQUIRE( first->childrenIndexes().size()  == 3);
    REQUIRE( second->childrenIndexes().size() == 0);

}
