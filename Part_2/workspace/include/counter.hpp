#pragma once

#include <atomic>
#include <cstdint>

namespace mac {

template<typename... Ts>
class counter;

template<typename T>
class counter<T> {
 private:
  std::atomic<T>& value_;

 public:
  counter(std::atomic<T>& value) : value_(value) {}

  std::uint64_t fetch_add() noexcept {
    return value_.fetch_add(1U, std::memory_order_acq_rel);
  }

  void add() noexcept {
    value_.fetch_add(1U, std::memory_order_acq_rel);
  }

  std::uint64_t load() const noexcept {
    return value_.load(std::memory_order_acquire);
  }

  void reset(const std::uint64_t value = 0) noexcept {
    value_.store(value, std::memory_order_release);
  }
};

template<typename Low, typename... High>
class counter<Low, High...> {
 private:
  std::atomic<Low>& low_;
  counter<High...> high_;
  
  static constexpr std::uint64_t shift_ = sizeof(Low) * 8 - 1;
  static constexpr std::uint64_t mask_ = (1ULL << shift_) - 1;

 public:
  counter(std::atomic<Low>& low, std::atomic<High>&... high) : low_(low), high_(high...) {}

  std::uint64_t fetch_add() noexcept {
    std::uint64_t crt_high = high_.load();
    Low old_low = low_.fetch_add(1U, std::memory_order_acq_rel);

    Low crt_low = old_low + 1U;

    if (old_low >> shift_ != crt_low >> shift_) {
      high_.add();
    }

    crt_high = crt_high + ((old_low >> shift_) != (crt_high & 1ULL));

    return (crt_high << shift_) | (old_low & mask_);
  }

  void add() noexcept {
    Low old_low = low_.fetch_add(1U, std::memory_order_acq_rel);
    Low crt_low = old_low + 1U;

    if (old_low >> shift_ != crt_low >> shift_) {
      high_.add();
    }
  }

  std::uint64_t load() const noexcept {
    std::uint64_t crt_high = high_.load();
    Low old_low = low_.load(std::memory_order_acquire);

    crt_high = crt_high + ((old_low >> shift_) != (crt_high & 1ULL));

    return (crt_high << shift_) | (old_low & mask_);
  }

  void reset(const std::uint64_t value = 0) noexcept {
    std::uint64_t high = value >> shift_;
    Low low = static_cast<Low>((value & mask_) | ((high & 1ULL) << shift_));

    low_.store(low, std::memory_order_release);
    high_.reset(high);
  }
};

template<typename... Ts>
counter(std::atomic<Ts>&...) -> counter<Ts...>;

}  // mac - my atomic counter