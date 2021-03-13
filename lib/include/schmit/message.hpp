#pragma once

#include "pool/intrusive.hpp"

#include <optional>
#include <cstdint>
#include <utility>

namespace schmit
{

template <class Type, std::size_t SIZE>
class TMessage
{

public:

    using Pool = pool::intrusive::TMake<TMessage<Type, SIZE>, SIZE>;

    TMessage(Pool& pool) : _pool{pool} {}

    void send()
    {
        _refCount++;
    }

    void write(const Type& value)
    {
        _valueOpt = value;
    }

    void write(Type&& value)
    {
        _valueOpt = std::move(value);
    }

    Type& retrieve()
    {
        _refCount--;

        if (!_refCount)
        {
            _pool.recycle(*this);
        }

        return _valueOpt.value();
    }

private:

    std::optional<Type> _valueOpt;
    uint32_t            _refCount = 0;
    Pool&               _pool;
};

} // namespace schmit
