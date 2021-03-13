#pragma once

#include "schmit/details/coroutine.hpp"
#include "schmit/work.hpp"

#include "topo/graph.hpp"

#include "pool/intrusive.hpp"

#include <functional>
#include <optional>

namespace schmit
{

template <class, std::size_t>
class TTask;

template <std::size_t>
class TScheduler;

namespace task
{

template <std::size_t>
struct TAbstract;

template <std::size_t SIZE>
struct TNode
{
    using NodeItType = typename topo::graph::TMake<task::TAbstract<SIZE>*, SIZE>::NodeListIt;

    TNode(NodeItType value) : _value{value} {}

    task::TAbstract<SIZE>& operator()()
    {
        return *(_value->_value);
    }

    bool isPending() const
    {
        return _value->isPending();
    }

    bool isRunning() const
    {
        return _value->_value->isRunning();
    }

    NodeItType _value;
};

template <std::size_t SIZE>
struct TEntryPoint
{
    static void run()
    {
        _next.value()().run();
    }

    static void* const _value;

    static thread_local std::optional<TNode<SIZE>> _next;
};

template <std::size_t SIZE>
thread_local std::optional<TNode<SIZE>> TEntryPoint<SIZE>::_next;

template <std::size_t SIZE>
void* const TEntryPoint<SIZE>::_value = reinterpret_cast<void*>(&TEntryPoint<SIZE>::run);

template <std::size_t SIZE>
struct TAbstract
{
    friend TScheduler<SIZE>;
    
    using Dependency = typename topo::graph::TMake<TAbstract<SIZE>*, SIZE>::EdgeListIt;

    template <std::size_t STACK_SIZE>
    TAbstract(TScheduler<SIZE>& scheduler,
              pool::TIntrusive<pool::intrusive::TTraits<schmit_details::TCoroutine<STACK_SIZE, SIZE>, SIZE>> & coroutinePool) :
        _scheduler{scheduler},
        _coroutine{coroutinePool.make(TEntryPoint<SIZE>::_value)}
    {}

    void setNode(TNode<SIZE>& node)
    {
        _nodeOpt = node;
    }

    void attach(TNode<SIZE>& task, Dependency& dependency)
    {
        task().sendMessage();
        dependency = _scheduler.attach(_nodeOpt.value(), task);

        _scheduler.releaseThread(_nodeOpt.value());
    }

    bool isRunning() const
    {
        return _isRunning;
    }

    void run()
    {
        _isRunning = true;
        runImpl();
        _isRunning = false;

        _scheduler.pop(_nodeOpt.value());
        _coroutine.recycle();

    }

    template <class Type>
    TTask<Type, SIZE>& as()
    {
        return reinterpret_cast<TTask<Type, SIZE>&>(*this);
    }

    virtual void recycle()     = 0;
    virtual void runImpl()     = 0;
    virtual void sendMessage() = 0;

    virtual ~TAbstract() {}

private:

    TScheduler<SIZE>&                                        _scheduler;
    std::optional<TNode<SIZE>>                               _nodeOpt;
    schmit_details::coroutine::Abstract&                     _coroutine;
    bool                                                     _isRunning = false;
};

} // namespace task

template <class Type, std::size_t SIZE>
class TTask : public task::TAbstract<SIZE>
{

public:

    using Pool = pool::intrusive::TMake<TTask<Type, SIZE>, SIZE>;

    template <std::size_t STACK_SIZE>
    TTask(Pool& pool, 
          schmit::TScheduler<SIZE> & scheduler,
          pool::TIntrusive<pool::intrusive::TTraits<schmit_details::TCoroutine<STACK_SIZE, SIZE>, SIZE>> & coroutinePool) :
        task::TAbstract<SIZE>(scheduler, coroutinePool),
        _pool{pool}
    {}

    void assignWork(TWork<Type, SIZE>& work)
    {
        _workOpt = work;
    }

    void runImpl() override
    {
        if (_workOpt)
        {
            _workOpt.value().get().run();
        }
    }

    void recycle() override
    {
        _pool.recycle(*this);
    }

    void sendMessage() override
    {
        if (_workOpt)
        {
            _workOpt.value().get()._message.send();
        }
    }

private:

    std::optional<std::reference_wrapper<TWork<Type, SIZE>>> _workOpt;
    Pool&                                                    _pool;
};

} // namespace schmit
