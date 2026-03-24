#include <counter.hpp>

counter::counter(std::atomic<uint32_t>& low, std::atomic<uint32_t>& high) : low_(low), high_(high) {}

void counter::reset(uint64_t value) {
  uint32_t low = static_cast<uint32_t>(value);
  uint32_t high = static_cast<uint32_t>(value >> 32);

  low_.store(low, std::memory_order_relaxed);
  high_.store(high, std::memory_order_relaxed);
}

uint64_t counter::fetch_add() {
  while (true) {
    uint32_t crt_high = high_.load(std::memory_order_acquire);
    uint32_t crt_low = low_.load(std::memory_order_acquire);
    
    uint32_t check = high_.load(std::memory_order_acquire);

    uint32_t actual_high;
    if (crt_high == check) {
      actual_high = crt_high;
    } else {
      actual_high = (crt_low < 0x80000000 ? check : crt_high);
    }

    uint64_t crt_value = (static_cast<uint64_t>(actual_high) << 32) | crt_low;

    uint32_t upd_low = crt_low + 1;

    if (low_.compare_exchange_weak(crt_low, upd_low, std::memory_order_acq_rel)) {
      if (upd_low == 0) {
        high_.fetch_add(1, std::memory_order_release);
      }

      return crt_value;
    }
  }
}