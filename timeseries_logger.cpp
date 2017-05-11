#include "timeseries_logger.h"


namespace TimeSeriesLogger{

TreeRoot::TreeRoot(): Node(nullptr, *this, "ROOT", 0)
{
    _nodes.reserve(256);
    _raw_buffer.reserve(256*8);
}


Node::Node(Node *parent, TreeRoot& root, const char *name, size_t offset):
    _info( new NodeInfo(name, parent) ),
    _root(root),
    _offset(offset)
{

}

}
