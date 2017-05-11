#ifndef TIMESERIES_LOGGER_H
#define TIMESERIES_LOGGER_H

#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <array>
#include <memory>
#include <string>
#include <cstring>
#include <type_traits>
#include <boost/type_traits.hpp>

#define IS_TRIVIALLY_COPYABLE(T) (boost::has_trivial_copy<T>::value && boost::has_trivial_destructor<T>::value)


namespace TimeSeriesLogger{

class Node;
template <typename T> class PodNode;
class TreeRoot;

struct NodeInfo
{
    NodeInfo( const char* name, Node* parent): _name(name), _parent(parent) {}
    const std::string  _name;
    std::vector<Node*> _children;
    Node* _parent;
};

//----------------------------------------------

class Node
{
public:
    const std::string& name() const { return _info->_name; }

    const std::vector<Node*>& children() const { return _info->_children; }

    const TreeRoot& root() const { return _root; }

    virtual ~Node(){ }

   // template <typename T> PodNode<T>* addChild(const char* name);

protected:

    Node(Node* parent, TreeRoot& root, const char* name, size_t offset);

    TreeRoot& _root;
    std::shared_ptr<NodeInfo> _info;
    size_t  _offset;

    friend class TreeRoot;
};


//----------------------------------------------

template <typename POD>
struct Serialize
{
    static size_t size()
    {
        static_assert( IS_TRIVIALLY_COPYABLE(POD), "Non trivially Copyable" );
        return sizeof(POD);
    }
    static void serialize( const POD& source, uint8_t* destination )
    {
        static_assert( IS_TRIVIALLY_COPYABLE(POD), "Non trivially Copyable" );
        std::memcpy( destination, &source, size() );
    }
    static size_t deserialize( const uint8_t* source, POD& destination )
    {
        static_assert( IS_TRIVIALLY_COPYABLE(POD), "Non trivially Copyable" );
        std::memcpy( &destination, source, size()  );
        return size();
    }
};



template <typename T>
class PodNode: public Node
{
public:
    virtual ~PodNode(){}

    PodNode& operator =(const T& val);

    PodNode& operator *();

    const PodNode& operator *() const;

    virtual void set(const T& val);

    T get() const;

protected:

    PodNode(Node* parent, TreeRoot& root, const char* name, size_t offset);
    friend class TreeRoot;
};
//----------------------------------------------
class TreeRoot: public Node
{
public:
    TreeRoot();
    ~TreeRoot(){}

    const std::vector<Node*>& allNodes() const     { return _nodes; }

    std::vector<uint8_t>&       rawBuffer()       { return _raw_buffer; }
    const std::vector<uint8_t>& rawBuffer() const { return _raw_buffer; }

    template <typename T>
    PodNode<T>* addChild(const char* name, Node* parent = nullptr);

protected:

    std::vector<uint8_t> _raw_buffer;
    std::vector<Node*>   _nodes;
    size_t _cursor;

};

//----------------------------------------------
//  Declarations
//----------------------------------------------


template <typename T> inline
PodNode<T>* TreeRoot::addChild(const char* name, Node* parent)
{
    for(auto& child: parent->children() )
    {
        if( strcmp( child->name().c_str(), name ) == 0)
        {
            throw std::runtime_error("Child node exists already");
        }
    }

    _raw_buffer.resize(  _cursor + Serialize<T>::size() );

    PodNode<T>* ptr = new PodNode<T>( parent, *this, name, _cursor );

    // move the root offset;
    _cursor += Serialize<T>::size();

    //self register
    _nodes.push_back( ptr );
    parent->_info->_children.push_back(ptr);

    return ptr;
}

//template <typename T> inline
//PodNode<T>* Node::addChild(const char* name)
//{
//    return _root.addChild<T>(name);
//}


template<typename T> inline
PodNode<T> &PodNode<T>::operator =(const T& val)
{
    Serialize<T>::serialize(val, _root.rawBuffer().data() + _offset );
}

template<typename T> inline
void PodNode<T>::set(const T &val)
{
    Serialize<T>::serialize(val, _root.rawBuffer().data() + _offset );
}

template<typename T> inline
T PodNode<T>::get() const
{
    T val;
    Serialize<T>::deserialize(_root.rawBuffer().data() + _offset, val );
    return val;
}

template<typename T> inline
PodNode<T>& PodNode<T>::operator *()
{
    return *reinterpret_cast<T*>( _root.rawBuffer().data() + _offset );
}

template<typename T> inline
const PodNode<T>& PodNode<T>::operator *() const
{
    return *reinterpret_cast<T*>( _root.rawBuffer().data() + _offset );
}

template<typename T> inline
PodNode<T>::PodNode(TimeSeriesLogger::Node *parent, TreeRoot &root,
                    const char *name, size_t offset):
    Node(parent, root, name, offset)
{

}



} // end namespace

#endif // TIMESERIES_LOGGER_H
