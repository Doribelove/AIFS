#include "aifs/qos/token_bucket.h"

#include <algorithm>

namespace aifs::qos {

TokenBucket::TokenBucket(std::uint64_t capacity, std::uint64_t refill_per_second)
    : capacity_(capacity),
      refill_per_second_(refill_per_second),
      tokens_(capacity),
      last_refill_(std::chrono::steady_clock::now()) {}

bool TokenBucket::TryConsume(std::uint64_t tokens) {
    Refill();
    if (tokens > tokens_) {
        return false;
    }
    tokens_ -= tokens;
    return true;
}

std::uint64_t TokenBucket::Available() {
    Refill();
    return tokens_;
}

void TokenBucket::Refill() {
    const auto now = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_refill_).count();
    if (elapsed <= 0) {
        return;
    }
    const auto refill = static_cast<std::uint64_t>(elapsed) * refill_per_second_;
    tokens_ = std::min(capacity_, tokens_ + refill);
    last_refill_ = now;
}

}  // namespace aifs::qos

