#pragma once

#include <cstdint>
#include <atomic>

class counter {
 private:
  std::atomic<uint32_t>& low_;
  std::atomic<uint32_t>& high_;
 public:
  counter(std::atomic<uint32_t>& low, std::atomic<uint32_t>& high);

  void reset(uint64_t value = 0);
  uint64_t fetch_add();
};