#pragma once

#include <async/executor_interface.h>
#include <async/thread_pool.h>
#include <mutex>
#include <queue>

namespace Conjur { namespace Async {

  /**
   * @brief Implementation of an Executor.
   * @tparam ThreadPoolT Thread Pool implementation
   */
  template <typename ThreadPoolT = ThreadPool<>>
  class Executor
    : public ExecutorInterface
  {
  public:

    Executor(const Executor&) = delete;
    Executor& operator=(const Executor&) = delete;
    Executor(Executor&&) = default;
    Executor& operator=(Executor&&) = default;

    using SharedMutexT = std::shared_mutex;

    /**
     * @brief Construct a new Executor
     * @param poolSize number of threads
     */
    Executor(size_t poolSize);

    /**
     * @brief Starts the Executor.
     */
    virtual void Start() override;

    /**
     * @brief Stops the Executor.
     */
    virtual void Stop() override;

    /**
     * @brief Posts a job to the Executor.
     * @param work job
     */
    virtual void Post(Work&& work) override;

  private:

    using TaskCollection = std::queue<Work>;

    Work JobAcceptor();

    ThreadPoolT ThreadPool;
    size_t PoolSize;
    TaskCollection Tasks;
    SharedMutexT TasksMutex;
  };

} }

#include <async/executor_impl.h>
