#include "aifs/metadata/placement_manager.h"

#include <algorithm>
#include <functional>
#include <sstream>

namespace aifs::metadata {

PlacementManager::PlacementManager(std::uint64_t chunk_size, int replica_count)
    : chunk_size_(chunk_size), replica_count_(replica_count) {}

std::vector<common::ChunkLocation> PlacementManager::PlanChunks(
    const std::string& path,
    std::uint64_t file_size,
    const std::vector<std::string>& storage_nodes) const {
    std::vector<common::ChunkLocation> locations;
    if (storage_nodes.empty() || file_size == 0) {
        return locations;
    }

    const auto chunk_count = (file_size + chunk_size_ - 1) / chunk_size_;
    const auto replicas = std::min<int>(replica_count_, static_cast<int>(storage_nodes.size()));
    const auto hash = std::hash<std::string>{}(path);

    for (std::uint64_t chunk_index = 0; chunk_index < chunk_count; ++chunk_index) {
        std::ostringstream chunk_id;
        chunk_id << "chunk-" << hash << "-" << chunk_index;
        const auto offset = chunk_index * chunk_size_;
        const auto chunk_bytes = std::min<std::uint64_t>(chunk_size_, file_size - offset);

        for (int replica = 0; replica < replicas; ++replica) {
            const auto node_index = (hash + chunk_index + replica) % storage_nodes.size();
            locations.push_back(common::ChunkLocation{
                chunk_id.str(),
                storage_nodes[node_index],
                common::StorageTier::kHot,
                chunk_bytes,
                1,
            });
        }
    }
    return locations;
}

std::uint64_t PlacementManager::chunk_size() const {
    return chunk_size_;
}

int PlacementManager::replica_count() const {
    return replica_count_;
}

}  // namespace aifs::metadata

