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
  
  std::atomic<std::uint16_t> segment_1{0};
  std::atomic<std::uint8_t>  segment_2{0};
  std::atomic<std::uint16_t> segment_3{0};
  std::atomic<std::uint8_t>  segment_4{0};
  std::atomic<std::uint16_t> segment_5{0};
  
  mac::counter obj_counter(segment_1, segment_2, segment_3, segment_4, segment_5);

  obj_counter.reset(0x00FEFFFE);

  std::vector<std::uint64_t> results(total);

  {
    std::vector<std::jthread> threads(num_threads);

    for (size_t i = 0; i < num_threads; i += 1) {
      auto increment = [&, i]() {
        for (size_t j = 0; j < increment_per_thread; j += 1) {
          results[i * increment_per_thread + j] = obj_counter.fetch_add();
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