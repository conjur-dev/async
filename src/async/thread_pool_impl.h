#pragma once

#include <async/thread_pool.h>

namespace Conjur { namespace Async {

  template <typename ThreadT>
  inline ThreadPool<ThreadT>::ThreadPool(
    JobAcceptorT jobAcceptor)
    : ThreadPool<ThreadT>{
        std::move(jobAcceptor),
        [] (WorkerT worker)
        {
          return ThreadT{worker};
        }}
  {
  }

  template <typename ThreadT>
  inline ThreadPool<ThreadT>::ThreadPool(
    JobAcceptorT jobAcceptor,
    ThreadFactoryT threadFactory)
    : JobAcceptor{std::move(jobAcceptor)}
    , ThreadFactory{std::move(threadFactory)}
  {
  }

  template <typename ThreadT>
  inline ThreadPool<ThreadT>::~ThreadPool()
  {
    ScaleThreads(0);
  }

  template <typename ThreadT>
  inline void ThreadPool<ThreadT>::ScaleThreads(size_t poolSize)
  {
    {
      std::unique_lock<SharedMutexT> guard(PoolSizeMutex);
      PoolSize = poolSize;

      Awaiting.notify_all();
    }

    auto poolSizeMutexLock = std::shared_lock<SharedMutexT>(PoolSizeMutex, std::defer_lock);
    auto threadsMutexLock = std::unique_lock<MutexT>(ThreadsMutex, std::defer_lock);
    std::lock(poolSizeMutexLock, threadsMutexLock);

    if (PoolSize > Threads.size())
    {
      for (size_t i = Threads.size(); i != PoolSize; ++i)
      {
        Threads.emplace_back(
          ThreadFactory(
            [this, i] ()
            {
              Worker(i);
            }));
      }

      return;
    }

    while (Threads.size() > PoolSize)
      Threads.pop_back();
  }

  template <typename ThreadT>
  inline void ThreadPool<ThreadT>::Notify()
  {
    Awaiting.notify_one();
  }

  template <typename ThreadT>
  inline void ThreadPool<ThreadT>::Worker(size_t threadNumber)
  {
    while (true)
    {
      ExecutorInterface::Work work;

      {
        std::unique_lock<MutexT> lock{AwaitingMutex};
        Awaiting.wait(
          lock,
          [this, &work, threadNumber] ()
          {
            work = JobAcceptor();
            if (work)
              return true;

            std::shared_lock<SharedMutexT> guard(PoolSizeMutex);
            return threadNumber >= PoolSize;
          });
      }

      if (work)
        work();

      {
        std::shared_lock<SharedMutexT> guard(PoolSizeMutex);
        if (threadNumber >= PoolSize)
          break;
      }
    }
  }

} }
