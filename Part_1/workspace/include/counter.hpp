#pragma once

#include <atomic>
#include <cstdint>

inline std::uint64_t fetch_add(std::atomic<std::uint32_t>& low, std::atomic<std::uint32_t>& high) noexcept {
  std::uint32_t crt_high = high.load(std::memory_order_acquire);
  std::uint32_t old_low = low.fetch_add(1U, std::memory_order_acq_rel);

  std::uint32_t crt_low = old_low + 1U;

  if (old_low >> 31 != crt_low >> 31) {
    high.fetch_add(1U, std::memory_order_acquire);
  }

  crt_high = crt_high + ((old_low >> 31) - (crt_high & 1U));

  return static_cast<std::uint64_t>(crt_high) << 31 | static_cast<std::uint64_t>(old_low & 0x7FFFFFFF);
}