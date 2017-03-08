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

class LoggerNode;

class LoggerNode
{
public:
    const std::string& name() const { return _name; }

    const std::vector<LoggerNode*>& children() const { return _children; }

    TimeseriesLoggerRoot* getRoot() { return _root; }

    virtual ~LoggerNode(){ }

protected:
    LoggerNode(LoggerNode* parent, TimeseriesLoggerRoot *root, const char* name, uint32_t offset);
    const std::string _name;
    std::vector<LoggerNode*> _children;
    LoggerNode* _parent;
    TimeseriesLoggerRoot* _root;
    uint32_t _offset;

    friend class TimeseriesLoggerRoot;
};

//----------------------------------------------

template <typename T>class NumericValue: public LoggerNode
{
public:
    virtual ~NumericValue(){}

    NumericValue& operator =( T val);

    virtual void set(T val);

    T get() const;

    template <typename OtherType> std::shared_ptr<NumericValue<OtherType>> createChild(const char* name);

protected:

    NumericValue(LoggerNode* parent, TimeseriesLoggerRoot* root, const char* name, uint32_t offset);
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

    std::shared_ptr<LoggerNode>& getNode(int16_t index) { return _nodes[index]; }
    const std::shared_ptr<LoggerNode>& getNode(int16_t index) const { return _nodes[index]; }

    int16_t nodesCount() const { return _nodes.size(); }

    std::vector<uint8_t>& rawBuffer()             { return _raw_buffer; }
    const std::vector<uint8_t>& rawBuffer() const { return _raw_buffer; }

protected:
    std::vector<uint8_t> _raw_buffer;
    std::vector< std::shared_ptr<LoggerNode>> _nodes;

};

//----------------------------------------------
//  Declarations
//----------------------------------------------


template <typename Type> inline
std::shared_ptr<NumericValue<Type>> TimeseriesLoggerRoot::createChild(LoggerNode* parent, const char* name)
{
    TimeseriesLoggerRoot* root = this;

    for(auto& child: parent->_children)
    {
        if( strcmp( child->name().c_str(), name ) == 0)
        {
            throw std::runtime_error("Child node exists already");
        }
    }

    auto ptr = ( new NumericValue<Type>( parent, root, name, root->_offset ) );
    std::shared_ptr<NumericValue<Type>> shr_ptr( ptr );

    // move the root offset;
    root->_offset += sizeof(Type);
    root->_raw_buffer.resize( root->_offset );

    //self register
    root->_nodes.push_back( shr_ptr );
    parent->_children.push_back(ptr);

    return shr_ptr;
}

template<typename T> inline
NumericValue<T> &NumericValue<T>::operator =(T val)
{
    *reinterpret_cast<T*>( _root->rawBuffer().data() + _offset ) = val;
}

template<typename T> inline
void NumericValue<T>::set(T val) {
    *reinterpret_cast<T*>( _root->rawBuffer().data() + _offset ) = val;
}

template<typename T> inline
T NumericValue<T>::get() const {
    return *reinterpret_cast<T*>( _root->rawBuffer().data() + _offset );
}



template<typename T> inline
NumericValue<T>::NumericValue(tsl::LoggerNode *parent, TimeseriesLoggerRoot *root, const char *name, uint32_t offset):
    LoggerNode(parent, root, name, offset)
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
