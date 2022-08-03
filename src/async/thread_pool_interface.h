#pragma once

namespace Conjur { namespace Async {

  /**
   * @brief Interface of a Thread Pool.
   */
  class ThreadPoolInterface
  {
  public:

    virtual ~ThreadPoolInterface() = default;

    /**
     * @brief Sets the threads pool size.
     * @param poolSize number of threads
     */
    virtual void ScaleThreads(size_t poolSize) = 0;

    /**
     * @brief Notify new job.
     */
    virtual void Notify() = 0;
  };

} }
