#pragma once

#include <chrono>
#include <cstdint>

namespace aifs::qos {

class TokenBucket {
public:
    TokenBucket(std::uint64_t capacity, std::uint64_t refill_per_second);

    bool TryConsume(std::uint64_t tokens);
    std::uint64_t Available();

private:
    void Refill();

    std::uint64_t capacity_;
    std::uint64_t refill_per_second_;
    std::uint64_t tokens_;
    std::chrono::steady_clock::time_point last_refill_;
};

}  // namespace aifs::qos

