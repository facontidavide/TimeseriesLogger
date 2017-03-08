#include "timeseries_logger.h"



tsl::TimeseriesLoggerRoot::TimeseriesLoggerRoot():
    LoggerNode(nullptr, this, "ROOT", 0)
{
    _nodes.reserve(256);
    _raw_buffer.reserve(256*8);
}


tsl::LoggerNode::LoggerNode(LoggerNode *parent, TimeseriesLoggerRoot *root, const char *name, uint32_t offset):
    _name(name),
    _parent(parent),
    _root(root),
    _offset(offset)
{

}


