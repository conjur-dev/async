#include <async/thread_pool.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <future>
#include <memory>

namespace Conjur { namespace Async { namespace Tests {

  using namespace ::testing;

  class ThreadPoolTests
    : public Test
  {
  protected:

    virtual void SetUp() override;
    virtual void TearDown() override;

    std::unique_ptr<ThreadPoolInterface> ThreadPool;
    MockFunction<ExecutorInterface::Work()> JobAcceptor;
  };

  void ThreadPoolTests::SetUp()
  {
    ThreadPool = std::make_unique<Async::ThreadPool<>>(JobAcceptor.AsStdFunction());

    EXPECT_CALL(JobAcceptor, Call()).WillRepeatedly(Return(ExecutorInterface::Work{}));
    ThreadPool->ScaleThreads(1);
  }

  void ThreadPoolTests::TearDown()
  {
    EXPECT_CALL(JobAcceptor, Call()).WillRepeatedly(Return(ExecutorInterface::Work{}));
    ThreadPool = nullptr;
  }

  TEST_F(ThreadPoolTests, TestJobAcceptor)
  {
    std::promise<void> promise;
    auto future = promise.get_future();

    EXPECT_CALL(JobAcceptor, Call())
      .WillOnce(Return(
        [&promise] ()
        {
          promise.set_value();
        }))
      .WillRepeatedly(Return(ExecutorInterface::Work{}));

    future.get();
  }

} } }
