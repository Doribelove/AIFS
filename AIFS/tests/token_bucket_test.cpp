#include <chrono>
#include <cstdint>
#include <iostream>
#include <limits>

#include "aifs/qos/token_bucket.h"

namespace {

int failures = 0;

void Check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << message << '\n';
        ++failures;
    }
}

}  // namespace

int main() {
    using aifs::qos::TokenBucket;
    using Clock = std::chrono::steady_clock;
    using namespace std::chrono_literals;

    {
        auto now = Clock::time_point{};
        TokenBucket bucket(100, 10, [&now] { return now; });
        Check(bucket.TryConsume(100), "initial capacity is available");
        now += 999ms;
        Check(bucket.Available() == 0, "partial seconds do not refill early");
        now += 501ms;
        Check(bucket.Available() == 10, "first whole second refills ten tokens");
        now += 600ms;
        Check(bucket.Available() == 20, "refill preserves the fractional-second remainder");
        now += 10s;
        Check(bucket.Available() == 100, "long idle periods saturate at capacity");
        Check(!bucket.TryConsume(101), "requests above capacity fail");
        Check(bucket.Available() == 100, "failed consumption preserves available tokens");
        Check(bucket.TryConsume(100), "a saturated bucket can be fully consumed");
        Check(bucket.Available() == 0, "elapsed time is not reused after saturation");
        Check(bucket.TryConsume(0), "zero-token requests succeed");
    }

    {
        auto now = Clock::time_point{};
        const auto maximum = std::numeric_limits<std::uint64_t>::max();
        TokenBucket bucket(maximum, 2, [&now] { return now; });
        Check(bucket.TryConsume(1), "consume one token before addition overflow test");
        now += 1s;
        Check(bucket.Available() == maximum, "refill addition saturates without overflow");
    }

    {
        auto now = Clock::time_point{};
        const auto maximum = std::numeric_limits<std::uint64_t>::max();
        TokenBucket bucket(maximum, maximum / 2 + 1, [&now] { return now; });
        Check(bucket.TryConsume(maximum), "empty the bucket before multiplication overflow test");
        now += 2s;
        Check(bucket.Available() == maximum, "refill multiplication saturates without overflow");
    }

    {
        auto now = Clock::time_point{};
        TokenBucket bucket(10, 0, [&now] { return now; });
        Check(bucket.TryConsume(3), "zero-rate buckets retain their initial capacity");
        now += 10s;
        Check(bucket.Available() == 7, "zero refill rate preserves the remaining balance");
        Check(!bucket.TryConsume(8), "insufficient-token requests fail");
        Check(bucket.Available() == 7, "failed requests do not consume tokens");
    }

    {
        auto now = Clock::time_point{};
        TokenBucket bucket(0, 10, [&now] { return now; });
        now += 10s;
        Check(bucket.Available() == 0, "zero-capacity buckets remain empty");
        Check(!bucket.TryConsume(1), "zero-capacity buckets reject positive requests");
        Check(bucket.TryConsume(0), "zero-capacity buckets allow zero-token requests");
    }

    return failures == 0 ? 0 : 1;
}
