#pragma once

#include "list/iterator.hpp"
#include "list/list.hpp"

#include <cstdint>

namespace topo
{

template <class>
class TGraph;

namespace graph
{

template <class>
class TNode;

namespace node
{

template <class, std::size_t>
struct TTraits;

template <class Type, std::size_t SIZE>
using TMake = TNode<TTraits<Type, SIZE>>;

} // namespace node

template <class Traits>
class TEdge
{
    template <class>
    friend class topo::TGraph;

    using NodeListIt       = typename Traits::NodeListIt;
    using EdgeListItListIt = typename Traits::EdgeListItListIt;

public:

    TEdge(const NodeListIt lhs,
          const NodeListIt rhs) :
        _dependerNode{lhs},
        _dependeeNode{rhs}
    {}

    void update()
    {
        _dependerEdge = _dependerNode->_dependees.begin();
        _dependeeEdge = _dependeeNode->_dependers.begin();
    }

    void detach()
    {
        _dependerNode->_dependees.erase(_dependerEdge);
        _dependeeNode->_dependers.erase(_dependeeEdge);
    }

private:

    NodeListIt _dependerNode;
    NodeListIt _dependeeNode;

    EdgeListItListIt _dependerEdge;
    EdgeListItListIt _dependeeEdge;
};

namespace edge
{

template <class Type, std::size_t SIZE>
using TMake = TEdge<TTraits<Type, SIZE>>;

template <class Type, std::size_t SIZE>
struct TTraits
{
    using NodeListIt       = list::iterator::TMake<node::TMake<Type, SIZE> >;
    using EdgeListIt       = list::iterator::TMake<edge::TMake<Type, SIZE> >;
    using EdgeListItListIt = list::iterator::TMake<EdgeListIt              >;
};

} // namespace edge
} // namespace graph
} // namespace topo
