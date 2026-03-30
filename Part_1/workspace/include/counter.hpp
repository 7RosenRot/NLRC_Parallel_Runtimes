#pragma once

#include <cstdint>
#include <atomic>

class counter {
 private:
  std::atomic<uint32_t>& high_;
  std::atomic<uint32_t>& low_;
 
 public:
  counter(std::atomic<uint32_t>& high, std::atomic<uint32_t>& low);

  void reset(uint64_t value = 0);
  uint64_t fetch_add() noexcept;
};