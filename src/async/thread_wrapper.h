#pragma once

#include <thread>

namespace Conjur { namespace Async {

  /**
   * @brief Implementation of a thread wrapper.
   */
  class ThreadWrapper
  {
  public:

    ThreadWrapper(const ThreadWrapper&) = delete;
    ThreadWrapper(ThreadWrapper&&) = default;
    ThreadWrapper& operator=(const ThreadWrapper&) = delete;
    ThreadWrapper& operator=(ThreadWrapper&&) = default;

    /**
     * @brief Constructs a new simple thread wrapper. 
     */
    template <typename... ArgsT>
    ThreadWrapper(ArgsT&&... args);

    ~ThreadWrapper();

  private:

    std::thread Thread;
  };

  template <typename... ArgsT>
  inline ThreadWrapper::ThreadWrapper(ArgsT&&... args)
    : Thread{std::forward<ArgsT>(args)...}
  {
  }

  inline ThreadWrapper::~ThreadWrapper()
  {
    if (Thread.joinable())
      Thread.join();
  }

} }
