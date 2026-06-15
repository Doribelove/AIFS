#include "aifs/storage/replica_manager.h"

#include <set>

namespace aifs::storage {

void ReplicaManager::AddStore(std::shared_ptr<ChunkStore> store) {
    if (store) {
        stores_[store->node_id()] = std::move(store);
    }
}

std::vector<std::string> ReplicaManager::NodeIds() const {
    std::vector<std::string> ids;
    ids.reserve(stores_.size());
    for (const auto& [id, _] : stores_) {
        ids.push_back(id);
    }
    return ids;
}

common::Status ReplicaManager::PutReplicas(
    const std::vector<common::ChunkLocation>& locations,
    const std::string& chunk_id,
    const std::string& data) {
    std::set<std::string> written_nodes;
    for (const auto& location : locations) {
        if (location.chunk_id != chunk_id || written_nodes.count(location.node_id) > 0) {
            continue;
        }
        auto store_it = stores_.find(location.node_id);
        if (store_it == stores_.end()) {
            continue;
        }
        auto status = store_it->second->PutChunk(chunk_id, data);
        if (!status.ok()) {
            return status;
        }
        written_nodes.insert(location.node_id);
    }
    if (written_nodes.empty()) {
        return common::Status::NotFound("no target store for chunk: " + chunk_id);
    }
    return common::Status::Ok();
}

std::optional<std::string> ReplicaManager::ReadAnyReplica(
    const std::vector<common::ChunkLocation>& locations) const {
    for (const auto& location : locations) {
        auto store_it = stores_.find(location.node_id);
        if (store_it == stores_.end()) {
            continue;
        }
        auto data = store_it->second->GetChunk(location.chunk_id);
        if (data.has_value()) {
            return data;
        }
    }
    return std::nullopt;
}

std::size_t ReplicaManager::StoreCount() const {
    return stores_.size();
}

}  // namespace aifs::storage

