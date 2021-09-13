#include <iostream>

#include "schmit/scheduler.hpp"

// Notes and critique:
// If the task contains the coroutine, then the taskPool should allocate the coroutine
// Which means the stack size is now part of the task type?

int main()
{
    using Scheduler                 = schmit::TScheduler<1>;
    using SchedulerTaskGraphPoolSet = typename Scheduler::TaskGraphPoolSet;
    using PoolTask                  = typename Scheduler::PoolTask;
    using PoolWork                  = typename Scheduler::PoolWork;
    using PoolCoroutine             = typename Scheduler::TCoroutinePool<0x2000 /*stack size*/>;
    using Dependency                = typename Scheduler::Dependency;

    // Create the scheduler
    SchedulerTaskGraphPoolSet schedulerTaskGraphPoolSet;
    Scheduler scheduler{schedulerTaskGraphPoolSet};

    // Create the tasks
    PoolTask      poolTask;
    PoolCoroutine poolCoroutine;

    auto taskA = scheduler.makeTask(poolTask, poolCoroutine);
    auto taskB = scheduler.makeTask(poolTask, poolCoroutine);
    auto taskC = scheduler.makeTask(poolTask, poolCoroutine);

    // Create the works
    PoolWork poolWork;

    auto& workA = poolWork.make([](){ std::cout << "A\n"; });
    auto& workB = poolWork.make([](){ std::cout << "B\n"; });
    auto& workC = poolWork.make([](){ std::cout << "C\n"; });

    // Assign works to tasks
    taskA().assignWork(workA);
    taskB().assignWork(workB);
    taskC().assignWork(workC);

    // Declare dependencies
    Dependency dependency;

    taskA().attach(taskB, dependency);
    taskC().attach(taskA, dependency);

    scheduler.run();

    return 0;
}
