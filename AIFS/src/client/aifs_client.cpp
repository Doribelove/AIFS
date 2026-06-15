#include "aifs/client/aifs_client.h"

#include <algorithm>
#include <unordered_map>

namespace aifs::client {

AIFSClient::AIFSClient(
    metadata::MetadataServer& metadata,
    storage::ReplicaManager& replica_manager,
    qos::CheckpointScheduler& checkpoint_scheduler,
    std::size_t cache_capacity_bytes)
    : metadata_(metadata),
      replica_manager_(replica_manager),
      checkpoint_scheduler_(checkpoint_scheduler),
      cache_(cache_capacity_bytes),
      pipeline_(16) {}

common::Status AIFSClient::RegisterManifest(common::DatasetManifest manifest) {
    return metadata_.RegisterManifest(std::move(manifest));
}

common::Status AIFSClient::WriteFile(
    const std::string& path,
    const std::string& data,
    const common::RequestContext& context) {
    if (context.checkpoint) {
        auto admitted = checkpoint_scheduler_.Admit(context, path, data.size());
        if (!admitted.ok()) {
            return admitted;
        }
    }

    common::FileMetadata metadata;
    auto status = metadata_.CreateFile(
        path,
        data.size(),
        context,
        replica_manager_.NodeIds(),
        &metadata);
    if (!status.ok()) {
        return status;
    }

    std::vector<std::string> chunk_order;
    std::unordered_map<std::string, std::vector<common::ChunkLocation>> grouped;
    for (const auto& location : metadata.chunks) {
        if (grouped.find(location.chunk_id) == grouped.end()) {
            chunk_order.push_back(location.chunk_id);
        }
        grouped[location.chunk_id].push_back(location);
    }

    auto chunks = pipeline_.Split(data, metadata.chunks.empty() ? data.size() : metadata.chunks.front().size);
    std::size_t index = 0;
    for (const auto& chunk_id : chunk_order) {
        if (index >= chunks.size()) {
            break;
        }
        const auto& locations = grouped[chunk_id];
        status = replica_manager_.PutReplicas(locations, chunk_id, chunks[index]);
        if (!status.ok()) {
            return status;
        }
        cache_.Put(chunk_id, chunks[index]);
        ++index;
    }
    return common::Status::Ok();
}

common::Status AIFSClient::ReadFile(
    const std::string& path,
    const common::RequestContext&,
    std::string* out_data) {
    if (out_data == nullptr) {
        return common::Status::InvalidArgument("out_data is null");
    }
    auto metadata = metadata_.GetFile(path);
    if (!metadata.has_value()) {
        return common::Status::NotFound("file not found: " + path);
    }

    std::vector<std::string> chunk_order;
    std::unordered_map<std::string, std::vector<common::ChunkLocation>> grouped;
    for (const auto& location : metadata->chunks) {
        if (grouped.find(location.chunk_id) == grouped.end()) {
            chunk_order.push_back(location.chunk_id);
        }
        grouped[location.chunk_id].push_back(location);
    }

    std::vector<std::string> chunks;
    for (const auto& chunk_id : chunk_order) {
        auto cached = cache_.Get(chunk_id);
        if (cached.has_value()) {
            chunks.push_back(*cached);
            continue;
        }

        auto data = replica_manager_.ReadAnyReplica(grouped[chunk_id]);
        if (!data.has_value()) {
            return common::Status::NotFound("chunk has no readable replica: " + chunk_id);
        }
        cache_.Put(chunk_id, *data);
        chunks.push_back(*data);
    }
    *out_data = pipeline_.Join(chunks);
    return common::Status::Ok();
}

std::vector<std::string> AIFSClient::PrefetchPlan(
    const std::string& dataset_id,
    const std::string& current_path,
    std::size_t window) const {
    auto manifest = metadata_.GetManifest(dataset_id);
    if (!manifest.has_value()) {
        return {};
    }
    return prefetch_.BuildPlan(*manifest, current_path, window);
}

std::size_t AIFSClient::CacheEntries() const {
    return cache_.EntryCount();
}

}  // namespace aifs::client
