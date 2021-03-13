#include <iostream>

#include "schmit/scheduler.hpp"

// Notes and critique:
// If the task contains the coroutine, then the taskPool should allocate the coroutine
// Which means the stack size is now part of the task type?

int main()
{
    using Scheduler                 = schmit::TScheduler<1>;
    using SchedulerTaskGraphPoolSet = typename Scheduler::TaskGraphPoolSet;
    using PoolTaskChar              = typename Scheduler::TTaskPool    <char>;
    using PoolWorkChar              = typename Scheduler::TWorkPool    <char>;
    using PoolMessageChar           = typename Scheduler::TMessagePool <char>;
    using PoolCoroutine             = typename Scheduler::TCoroutinePool<0x2000 /*stack size*/>;
    using Dependency                = typename Scheduler::Dependency;

    // Create the scheduler
    SchedulerTaskGraphPoolSet schedulerTaskGraphPoolSet;
    Scheduler scheduler{schedulerTaskGraphPoolSet};

    // Create the tasks
    PoolTaskChar  poolTaskChar;
    PoolCoroutine poolCoroutine;

    auto taskA = scheduler.makeTask<char, 0x2000>(poolTaskChar, poolCoroutine);
    auto taskB = scheduler.makeTask<char, 0x2000>(poolTaskChar, poolCoroutine);
    auto taskC = scheduler.makeTask<char, 0x2000>(poolTaskChar, poolCoroutine);

    PoolMessageChar poolMessageChar;

    auto& mesgA = poolMessageChar.make();
    auto& mesgB = poolMessageChar.make();
    auto& mesgC = poolMessageChar.make();

    // Create the works
    PoolWorkChar poolWorkChar;

    auto& workA = poolWorkChar.make([](){ std::cout << "A\n"; return 'A'; }, mesgA);
    auto& workB = poolWorkChar.make([](){ std::cout << "B\n"; return 'B'; }, mesgB);
    auto& workC = poolWorkChar.make([](){ std::cout << "C\n"; return 'C'; }, mesgC);

    // Assign works to tasks
    taskA().as<char>().assignWork(workA);
    taskB().as<char>().assignWork(workB);
    taskC().as<char>().assignWork(workC);

    //workA._message.send();

    // Declare dependencies
    Dependency dependency;

    taskA().attach(taskB, dependency);
    taskC().attach(taskA, dependency);

    scheduler.run();

    std::cout << mesgA.retrieve() << '\n';
    std::cout << mesgB.retrieve() << '\n';
    std::cout << mesgC.retrieve() << '\n';

    return 0;
}
