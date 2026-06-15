#pragma once

#include <queue>
#include <string>

#include "aifs/common/status.h"
#include "aifs/common/types.h"
#include "aifs/qos/token_bucket.h"

namespace aifs::qos {

class CheckpointScheduler {
public:
    explicit CheckpointScheduler(TokenBucket bucket);

    common::Status Admit(
        const common::RequestContext& context,
        const std::string& path,
        std::uint64_t bytes);
    std::size_t PendingCount() const;

private:
    struct Request {
        int priority;
        std::string path;
        std::uint64_t bytes;
    };

    TokenBucket bucket_;
    std::queue<Request> pending_;
};

}  // namespace aifs::qos

