#include <gtest/gtest.h>

#include <algorithm>
#include <atomic>
#include <thread>
#include <vector>

#include <counter.hpp>

TEST(CounterTest, RaceCondition) {
  std::atomic<uint16_t> value_1{0};
  std::atomic<uint16_t> value_2{0};
  std::atomic<uint32_t> value_3{0};
  counter obj_counter(value_1, value_2, value_3);
  
  uint64_t start_val = 0x00FEFFFE;
  obj_counter.reset(start_val);

  const int threadCount = 8;
  const int increments_per_thread = 10000;

  const int total_increments = threadCount * increments_per_thread;
  std::vector<uint64_t> results(total_increments);
  
  std::atomic<bool> start_signal{false};
  
  std::vector<std::jthread> threads;
  threads.reserve(threadCount);

  {
    for (int i = 0; i < threadCount; ++i) {
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

  start_signal.store(true);
  
  threads.clear();

  std::sort(results.begin(), results.end());

  ASSERT_EQ(results[0], start_val);
  ASSERT_EQ(results.back() + 1, start_val + total_increments);
}