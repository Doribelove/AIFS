#include "aifs/qos/checkpoint_scheduler.h"

namespace aifs::qos {

CheckpointScheduler::CheckpointScheduler(TokenBucket bucket) : bucket_(bucket) {}

common::Status CheckpointScheduler::Admit(
    const common::RequestContext& context,
    const std::string& path,
    std::uint64_t bytes) {
    if (!context.checkpoint) {
        return common::Status::Ok();
    }
    if (bucket_.TryConsume(bytes)) {
        return common::Status::Ok();
    }

    pending_.push(Request{context.priority, path, bytes});
    return common::Status::ResourceExhausted("checkpoint queued by QoS token bucket: " + path);
}

std::size_t CheckpointScheduler::PendingCount() const {
    return pending_.size();
}

}  // namespace aifs::qos

