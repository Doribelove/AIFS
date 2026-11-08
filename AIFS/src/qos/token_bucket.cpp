#include "aifs/qos/token_bucket.h"

#include <utility>

namespace aifs::qos {

TokenBucket::TokenBucket(
    std::uint64_t capacity,
    std::uint64_t refill_per_second,
    std::function<std::chrono::steady_clock::time_point()> now)
    : capacity_(capacity),
      refill_per_second_(refill_per_second),
      tokens_(capacity),
      now_(std::move(now)),
      last_refill_(now_()) {}

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
    const auto now = now_();
    const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_refill_).count();
    if (elapsed <= 0) {
        return;
    }
    const auto remaining = capacity_ - tokens_;
    const auto elapsed_seconds = static_cast<std::uint64_t>(elapsed);
    if (refill_per_second_ > 0) {
        if (elapsed_seconds > remaining / refill_per_second_) {
            tokens_ = capacity_;
        } else {
            tokens_ += elapsed_seconds * refill_per_second_;
        }
    }
    // Retain the fractional second for the next refill.
    last_refill_ += std::chrono::seconds(elapsed);
}

}  // namespace aifs::qos

