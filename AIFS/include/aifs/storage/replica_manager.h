#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "aifs/common/status.h"
#include "aifs/common/types.h"
#include "aifs/storage/chunk_store.h"

namespace aifs::storage {

class ReplicaManager {
public:
    void AddStore(std::shared_ptr<ChunkStore> store);
    std::vector<std::string> NodeIds() const;

    common::Status PutReplicas(
        const std::vector<common::ChunkLocation>& locations,
        const std::string& chunk_id,
        const std::string& data);
    std::optional<std::string> ReadAnyReplica(const std::vector<common::ChunkLocation>& locations) const;
    std::size_t StoreCount() const;

private:
    std::unordered_map<std::string, std::shared_ptr<ChunkStore>> stores_;
};

}  // namespace aifs::storage

