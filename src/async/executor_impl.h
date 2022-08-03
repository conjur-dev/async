#pragma once

#include <async/executor.h>

namespace Conjur { namespace Async {

  template <typename ThreadPoolT>
  Executor<ThreadPoolT>::Executor(size_t poolSize)
    : ThreadPool{
        [this] ()
        {
          return JobAcceptor();
        }}
    , PoolSize{poolSize}
  {
  }

  template <typename ThreadPoolT>
  void Executor<ThreadPoolT>::Start()
  {
    ThreadPool.ScaleThreads(PoolSize);
  }

  template <typename ThreadPoolT>
  void Executor<ThreadPoolT>::Stop()
  {
    ThreadPool.ScaleThreads(0);
  }

  template <typename ThreadPoolT>
  void Executor<ThreadPoolT>::Post(Work&& work)
  {
    std::unique_lock<SharedMutexT> guard(TasksMutex);
    Tasks.emplace(std::move(work));

    ThreadPool.Notify();
  }

  template <typename ThreadPoolT>
  ExecutorInterface::Work Executor<ThreadPoolT>::JobAcceptor()
  {
    {
      std::shared_lock<SharedMutexT> guard(TasksMutex);
      if (Tasks.empty())
        return Work{};
    }

    std::unique_lock<SharedMutexT> guard(TasksMutex);
    if (Tasks.empty())
      return Work{};

    auto work = std::move(Tasks.front());
    Tasks.pop();

    return work;
  }

} }
