#pragma once

#include "schmit/message.hpp"

#include "pool/intrusive.hpp"

#include <functional>
#include <cstdint>
#include <utility>

namespace schmit
{

template <class Type, std::size_t SIZE>
class TWork
{

public:

    using Pool = pool::intrusive::TMake<TWork<Type, SIZE>, SIZE>;
    using Message = TMessage<Type, SIZE>;

    template <class Function>
    TWork(Pool& pool, Function&& function, Message& message) :
        _pool{pool},
        _function{std::move(function)},
        _message{message}
    {}

    void run()
    {
        _message.write(_function());
        _pool.recycle(*this);
    }

private:

    Pool&                 _pool;
    std::function<Type()> _function;

public:

    Message& _message;
};

} // namespace schmit
