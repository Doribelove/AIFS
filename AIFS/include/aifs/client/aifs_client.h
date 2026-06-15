#pragma once

#include <string>
#include <vector>

#include "aifs/client/cache_manager.h"
#include "aifs/client/io_pipeline.h"
#include "aifs/client/prefetch_manager.h"
#include "aifs/common/status.h"
#include "aifs/common/types.h"
#include "aifs/metadata/metadata_server.h"
#include "aifs/qos/checkpoint_scheduler.h"
#include "aifs/storage/replica_manager.h"

namespace aifs::client {

class AIFSClient {
public:
    AIFSClient(
        metadata::MetadataServer& metadata,
        storage::ReplicaManager& replica_manager,
        qos::CheckpointScheduler& checkpoint_scheduler,
        std::size_t cache_capacity_bytes = 16 * 1024 * 1024);

    common::Status RegisterManifest(common::DatasetManifest manifest);
    common::Status WriteFile(
        const std::string& path,
        const std::string& data,
        const common::RequestContext& context);
    common::Status ReadFile(
        const std::string& path,
        const common::RequestContext& context,
        std::string* out_data);
    std::vector<std::string> PrefetchPlan(
        const std::string& dataset_id,
        const std::string& current_path,
        std::size_t window) const;

    std::size_t CacheEntries() const;

private:
    metadata::MetadataServer& metadata_;
    storage::ReplicaManager& replica_manager_;
    qos::CheckpointScheduler& checkpoint_scheduler_;
    CacheManager cache_;
    PrefetchManager prefetch_;
    IOPipeline pipeline_;
};

}  // namespace aifs::client

