#pragma once

#include <chrono>
#include <cstdint>
#include <functional>

namespace aifs::qos {

class TokenBucket {
public:
    TokenBucket(
        std::uint64_t capacity,
        std::uint64_t refill_per_second,
        std::function<std::chrono::steady_clock::time_point()> now = std::chrono::steady_clock::now);

    bool TryConsume(std::uint64_t tokens);
    std::uint64_t Available();

private:
    void Refill();

    std::uint64_t capacity_;
    std::uint64_t refill_per_second_;
    std::uint64_t tokens_;
    std::function<std::chrono::steady_clock::time_point()> now_;
    std::chrono::steady_clock::time_point last_refill_;
};

}  // namespace aifs::qos

