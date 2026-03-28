#include <counter.hpp>

counter::counter(std::atomic<uint32_t>& high,
  std::atomic<uint32_t>& low) : high_(high), low_(low) {}

void counter::reset(std::uint64_t value) {
  uint32_t high = static_cast<uint32_t>(value >> 31);
  uint32_t low = static_cast<uint32_t>(value & 0x7FFFFFFF);

  high_.store(high, std::memory_order_relaxed);
  low_.store(low, std::memory_order_relaxed);
}

uint64_t counter::fetch_add() {
  uint32_t crt_high = high_.load(std::memory_order_acquire);
  uint32_t old_low = low_.fetch_add(1U, std::memory_order_acq_rel);  
  
  uint32_t crt_low = old_low + 1U;

  if (crt_low >> 31) {
    if (low_.compare_exchange_strong(crt_low, crt_low & 0x7FFFFFFF, std::memory_order_acq_rel)) {
      high_.fetch_add(1U, std::memory_order_release);
    }
  }

  return static_cast<uint64_t>(crt_high) << 31 | static_cast<uint64_t>(old_low & 0x7FFFFFFF);
}