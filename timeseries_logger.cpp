#include "timeseries_logger.h"



tsl::TimeseriesLoggerRoot::TimeseriesLoggerRoot():
    LoggerNode(nullptr,"ROOT", 0, 0)
{
    _nodes.reserve(256);
    _raw_buffer.reserve(256*8);
}

