#pragma once

#include <cstdint>
#include <atomic>
#include <tuple>
#include <limits>

template <typename... Args>
class counter {
 private:
  std::tuple<std::atomic<Args>&...> parts_;

  uint64_t recursive_read(uint64_t) const { return 0; }

  template <typename T, typename... Other>
  uint64_t recursive_read(uint64_t shift, std::atomic<T>& current, std::atomic<Other>&... other) const {
    uint64_t val = current.load(std::memory_order_acquire);
    
    uint64_t result = (static_cast<uint64_t>(val) << shift) | recursive_read(shift + sizeof(T) * 8, other...);
    
    return result;
  }

  void recursive_propagate() {}

  template <typename T, typename... Other>
  void recursive_propagate(std::atomic<T>& current, std::atomic<Other>&... other) {
    T prev = current.fetch_add(1, std::memory_order_acq_rel);

    if (prev == std::numeric_limits<T>::max()) {
      recursive_propagate(other...);
    }
  }

  void recursive_reset(uint64_t, uint64_t) {}

  template <typename T, typename... Other>
  void recursive_reset(uint64_t value, uint64_t shift, std::atomic<T>& current, std::atomic<Other>&... other) {
    current.store(static_cast<T>(value >> shift), std::memory_order_release);
    
    recursive_reset(value, shift + sizeof(T) * 8, other...);
  }

  uint64_t snapshot() const {
    while (true) {
      auto read_ref = [this](auto&... args) {
        return this->recursive_read(0, args...);
      };

      uint64_t value = std::apply(read_ref, parts_);
      uint64_t check = std::apply(read_ref, parts_);

      if (value == check) {
        return value;
      }
    }
  }

 public:
  static constexpr uint64_t total_bits = (0 + ... + (sizeof(Args) * 8));
  static constexpr uint64_t minValue = 0;
  static constexpr uint64_t maxValue = (total_bits >= 64) ? 0xFFFFFFFFFFFFFFFFULL : (1ULL << total_bits) - 1;

  counter(std::atomic<Args>&... args) : parts_(args...) {}

  void reset(uint64_t newValue = 0) {
    auto reset_ref = [this, newValue](auto&... args) {
      this->recursive_reset(newValue, 0, args...);
    };
    
    std::apply(reset_ref, parts_);
  }

  uint64_t fetch_add() {
    while (true) {
      uint64_t crt_value = snapshot();
      
      auto& low_atomic = std::get<0>(parts_);
      using type = std::remove_reference_t<decltype(low_atomic.load())>;
      
      type crt_low = static_cast<type>(crt_value);
      type upd_low = crt_low + 1;

      auto propagate_ref = [this](auto& first, auto&... other) { 
        (void)first;
        this->recursive_propagate(other...); 
      };
      
      if (low_atomic.compare_exchange_weak(crt_low, upd_low, std::memory_order_acq_rel)) {
        if (upd_low == 0) {
          std::apply(propagate_ref, parts_);
        }
        
        return crt_value;
      }
    }
  }
};