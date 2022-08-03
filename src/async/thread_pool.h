#pragma once

#include <async/executor_interface.h>
#include <async/thread_wrapper.h>
#include <async/thread_pool_interface.h>
#include <atomic>
#include <condition_variable>
#include <list>
#include <mutex>
#include <shared_mutex>

namespace Conjur { namespace Async {

  /**
   * @brief Implementation of an ThreadPool which manages a pool of threads.
   * @tparam ThreadT thread wrapper implementation
   */
  template <typename ThreadT = ThreadWrapper>
  class ThreadPool
    : public ThreadPoolInterface
  {
  public:

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = default;
    ThreadPool& operator=(ThreadPool&&) = default;

    using MutexT = std::mutex;
    using SharedMutexT = std::shared_mutex;
    using WorkerT = std::function<void()>;
    using JobAcceptorT = std::function<ExecutorInterface::Work()>;
    using ThreadFactoryT = std::function<ThreadT(WorkerT)>;

    /**
     * @brief Construct a new ThreadPool
     * @param jobAcceptor function to accept job
     */
    ThreadPool(JobAcceptorT jobAcceptor);

    /**
     * @brief Construct a new ThreadPool
     * @param jobAcceptor function to accept job
     * @param threadFactory function to construct Worker Thread
     */
    ThreadPool(
      JobAcceptorT jobAcceptor,
      ThreadFactoryT threadFactory);

    virtual ~ThreadPool() override;

    /**
     * @brief Sets the threads pool size.
     * @param poolSize number of threads
     */
    virtual void ScaleThreads(size_t poolSize) override;

    /**
     * @brief Notify new job.
     */
    virtual void Notify() override;

  private:

    using ThreadCollection = std::list<ThreadT>;

    /**
     * @brief Worker function.
     */
    void Worker(size_t threadNumber);

    size_t PoolSize{0};
    SharedMutexT PoolSizeMutex;
    JobAcceptorT JobAcceptor;
    ThreadFactoryT ThreadFactory;
    std::condition_variable Awaiting;
    MutexT AwaitingMutex;
    ThreadCollection Threads;
    MutexT ThreadsMutex;
  };

} }

#include <async/thread_pool_impl.h>
