#include <async/executor.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <future>
#include <memory>

namespace Conjur { namespace Async { namespace Tests {

  using namespace ::testing;

  class ExecutorTests
    : public Test
  {
  protected:

    virtual void SetUp() override;
    virtual void TearDown() override;

    std::unique_ptr<ExecutorInterface> Executor;
  };

  void ExecutorTests::SetUp()
  {
    Executor = std::make_unique<Async::Executor<>>(1);
    Executor->Start();
  }

  void ExecutorTests::TearDown()
  {
    Executor->Stop();
    Executor = nullptr;
  }

  TEST_F(ExecutorTests, TestJobPost)
  {
    MockFunction<ExecutorInterface::Work()> work;
    auto job = work.AsStdFunction();
    std::promise<void> promise;
    auto future = promise.get_future();

    EXPECT_CALL(work, Call()).Times(1);

    Executor->Post(
      [&promise, &job] ()
      {
        job();
        promise.set_value();
      });

    future.get();
  }

} } }
