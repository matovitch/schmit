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
class TEdge;

namespace edge
{

template <class, std::size_t>
struct TTraits;

template <class Type, std::size_t SIZE>
using TMake = TEdge<TTraits<Type, SIZE>>;

} // namespace edge

template <class Traits>
class TNode
{
    template <class>
    friend class topo::TGraph;

    template <class>
    friend class TEdge;

public:

    using Type           = typename Traits::Type;
    using EdgeListItList = typename Traits::EdgeListItList;
    using EdgeListItPool = typename Traits::EdgeListItPool;

    template <class... Args>
    TNode(EdgeListItPool& edgeListItPool, Args&&... args) :
        _value{args...},
        _dependees{edgeListItPool},
        _dependers{edgeListItPool}
    {}

    bool isPending() const
    {
        return _dependees.empty();
    }

    Type _value;

private:

    EdgeListItList _dependees;
    EdgeListItList _dependers;
};

namespace node
{

template <class TypeTraits, std::size_t SIZE>
struct TTraits
{
    using Type = TypeTraits;

    using EdgeListIt     = list::iterator::TMake<edge::TMake<Type, SIZE>>;
    using EdgeListItList = list::TMake<EdgeListIt, SIZE>;

    using EdgeListItPool = typename EdgeListItList::CellPool;
};

} // namespace node
} // namespace graph
} // namespace topo
