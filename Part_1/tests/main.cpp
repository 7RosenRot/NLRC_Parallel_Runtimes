#include <gtest/gtest.h>

#include <algorithm>
#include <atomic>
#include <thread>
#include <vector>

#include <counter.hpp>

TEST(CounterTest, Multithread) {
  const int num_threads = 8;
  const int increment_per_thread = 100000;
  const int total = num_threads * increment_per_thread;
  
  std::atomic<std::uint32_t> low{0x7FFFFFFF};
  std::atomic<std::uint32_t> high{0};

  std::vector<std::uint64_t> results(total);

  {
    std::vector<std::jthread> threads;

    for (size_t i = 0; i < num_threads; i += 1) {
      auto increment = [&, i]() {
        for (size_t j = 0; j < increment_per_thread; j += 1) {
          results[i * increment_per_thread + j] = fetch_add(low, high);
        }
      };

      threads.emplace_back(increment);
    }
  }

  std::sort(results.begin(), results.end());

  for (size_t i = 1; i < total; i += 1) {
    ASSERT_EQ(results[i], results[i - 1] + 1);
  }
}