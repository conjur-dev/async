#pragma once

#include <functional>

namespace Conjur { namespace Async {

  /**
   * @brief Interface of an Executor.
   */
  class ExecutorInterface
  {
  public:

    using Work = std::function<void(void)>;

    virtual ~ExecutorInterface() = default;

    /**
     * @brief Starts the Executor.
     */
    virtual void Start() = 0;

    /**
     * @brief Stops the Executor.
     */
    virtual void Stop() = 0;

    /**
     * @brief Posts a job to the Executor.
     * @param work job
     */
    virtual void Post(Work&& work) = 0;
  };

} }
