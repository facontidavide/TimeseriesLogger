#ifndef TIMESERIES_LOGGER_H
#define TIMESERIES_LOGGER_H

#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <array>
#include <memory>
#include <string>
#include <cstring>

namespace tsl{

class TimeseriesLoggerRoot;

class LoggerNode
{
public:
    const std::string& name() const { return _name; }

    const std::vector<uint16_t>& childrenIndexes() const { return _children_index; }

    LoggerNode* getRoot();
    virtual ~LoggerNode(){

    }

protected:
    LoggerNode(LoggerNode* _parent, const char* name, uint16_t index, uint32_t offset):
        _name(name), _parent(_parent), _index(index), _offset(offset)
    {}
    const std::string _name;
    std::vector< uint16_t > _children_index;
    uint16_t    _index;
    LoggerNode* _parent;
    uint32_t _offset;

    friend class TimeseriesLoggerRoot;
};

//----------------------------------------------

template <typename T>class NumericValue: public LoggerNode
{
public:
    virtual ~NumericValue(){}

    NumericValue& operator =( T val)
    {
        *reinterpret_cast<double*>( _root_buffer.data() + _offset ) = val;
    }

    virtual void set(T val) {
        *reinterpret_cast<double*>( _root_buffer.data() + _offset ) = val;
    }

    T get() const {
        return *reinterpret_cast<double*>( _root_buffer.data() + _offset );
    }

    const char* getName();

    template <typename OtherType> std::shared_ptr<NumericValue<OtherType>> createChild(const char* name);

protected:

    NumericValue(LoggerNode* parent, const char* name, uint16_t index, uint32_t offset, std::vector<uint8_t> &raw_buffer);
    std::vector<uint8_t>& _root_buffer;
    friend class TimeseriesLoggerRoot;
};
//----------------------------------------------
class TimeseriesLoggerRoot: public LoggerNode
{
public:
    TimeseriesLoggerRoot();

    ~TimeseriesLoggerRoot(){}

    template <typename T> std::shared_ptr<NumericValue<T>> createChild(const char* name) {
        return createChild<T>(this, name);
    }

    template <typename T> std::shared_ptr<NumericValue<T>> createChild(LoggerNode* parent, const char* name);

    LoggerNode* getNode(int16_t index);
    int16_t nodesCount() const { return _nodes.size(); }

protected:
    std::vector<uint8_t> _raw_buffer;
    std::vector<LoggerNode*> _nodes;

};

//----------------------------------------------
//  Declarations
//----------------------------------------------


inline LoggerNode *LoggerNode::getRoot() {
    LoggerNode* rt = this;
    while(rt->_parent != nullptr ) rt = rt->_parent;
    return rt;
}

template <typename Type> inline
std::shared_ptr<NumericValue<Type>> TimeseriesLoggerRoot::createChild(LoggerNode* parent, const char* name)
{
    TimeseriesLoggerRoot* root = this;
    uint16_t last_index = root->_index;
    for(auto& index: parent->_children_index)
    {
        if( strcmp( _nodes[index]->name().c_str(),name ) == 0)
        {
            throw std::runtime_error("Child node exists already");
        }
    }

    parent->_children_index.push_back(last_index);
    auto offset = root->_offset;
    root->_offset += sizeof(Type);
    auto ptr= ( new NumericValue<Type>( parent, name, last_index, offset, root->_raw_buffer ) );
    _nodes.push_back( ptr );
    root->_index++;
    return std::shared_ptr<NumericValue<Type>>(ptr);
}

template<typename T> inline
NumericValue<T>::NumericValue(tsl::LoggerNode *parent, const char *name, uint16_t index, uint32_t offset, std::vector<uint8_t> &raw_buffer):
    LoggerNode(parent,name,index, offset),
    _root_buffer(raw_buffer)
{

}

template<typename T> template <typename OtherType> inline
std::shared_ptr<NumericValue<OtherType> > NumericValue<T>::createChild(const char* name)
{
    TimeseriesLoggerRoot* root = static_cast<TimeseriesLoggerRoot*>(getRoot());
    return root->createChild<OtherType>(this,name);
}


} // end namespace

#endif // TIMESERIES_LOGGER_H
