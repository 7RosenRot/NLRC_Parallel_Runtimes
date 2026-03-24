#include <gtest/gtest.h>

#include <algorithm>
#include <atomic>
#include <thread>
#include <vector>

#include <counter.hpp>

TEST(CounterTest, RaceCondition) {
  std::atomic<uint32_t> low{0xFFFFFFFF};
  std::atomic<uint32_t> high{0};
  counter obj_counter(low, high);
  
  const int threadCount = 8;
  const int increments_per_thread = 100000;
  
  std::vector<uint64_t> results(threadCount * increments_per_thread);
  
  std::atomic<bool> start_signal{false};

  std::vector<std::jthread> threads;
  threads.reserve(threadCount);

  {
    for (size_t i = 0; i < threadCount; i += 1) {
      auto increment = [&, i]() {
        while (!start_signal) {
          std::this_thread::yield();
        }

        for (int j = 0; j < increments_per_thread; ++j) {
          results[i * increments_per_thread + j] = obj_counter.fetch_add();
        }
      };

      threads.emplace_back(increment);
    }
  }

  start_signal = true;

  threads.clear();
  
  std::sort(results.begin(), results.end());

  for (size_t i = 1; i < results.size(); i += 1) {
    ASSERT_EQ(results[i], results[i - 1] + 1);
  }
}