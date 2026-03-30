#include <counter.hpp>

#include <ctime>
#include <cstdlib>
#include <Windows.h>

counter::counter(std::atomic<uint32_t>& high,
  std::atomic<uint32_t>& low) : high_(high), low_(low) {}

void counter::reset(std::uint64_t value) {
  uint32_t high = static_cast<uint32_t>(value >> 31);
  uint32_t low = static_cast<uint32_t>(value & 0x7FFFFFFF);

  high_.store(high, std::memory_order_relaxed);
  low_.store(low, std::memory_order_relaxed);
}

// 0 ... 00 <- 0 ... 0
// 0 ... 01 <- 1 ... 0
// 0 ... 10 <- 0 ... 0
// 0 ... 11 <- 1 ... 0

uint64_t counter::fetch_add() noexcept {
  uint32_t crt_high = high_.load(std::memory_order_acquire);
  uint32_t old_low = low_.fetch_add(1U, std::memory_order_acq_rel);

  uint32_t crt_low = old_low + 1U;
  
  std::srand(std::time(0));
  if (old_low >> 31 != crt_low >> 31) {
    Sleep(rand() % 1000);
    high_.fetch_add(1U, std::memory_order_acquire);
  }

  crt_high = crt_high + ((crt_high & 1U) != (old_low >> 31));

  return static_cast<uint64_t>(crt_high) << 31 | static_cast<uint64_t>(old_low & 0x7FFFFFFF);
}