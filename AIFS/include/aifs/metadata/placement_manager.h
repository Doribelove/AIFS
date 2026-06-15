#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "aifs/common/types.h"

namespace aifs::metadata {

class PlacementManager {
public:
    explicit PlacementManager(std::uint64_t chunk_size = 4 * 1024 * 1024, int replica_count = 3);

    std::vector<common::ChunkLocation> PlanChunks(
        const std::string& path,
        std::uint64_t file_size,
        const std::vector<std::string>& storage_nodes) const;

    std::uint64_t chunk_size() const;
    int replica_count() const;

private:
    std::uint64_t chunk_size_;
    int replica_count_;
};

}  // namespace aifs::metadata

