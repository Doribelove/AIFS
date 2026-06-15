#include "aifs/storage/chunk_store.h"

namespace aifs::storage {

ChunkStore::ChunkStore(std::string node_id) : node_id_(std::move(node_id)) {}

const std::string& ChunkStore::node_id() const {
    return node_id_;
}

common::Status ChunkStore::PutChunk(const std::string& chunk_id, const std::string& data) {
    return engine_.Put(chunk_id, data);
}

std::optional<std::string> ChunkStore::GetChunk(const std::string& chunk_id) const {
    return engine_.Get(chunk_id);
}

common::Status ChunkStore::DeleteChunk(const std::string& chunk_id) {
    return engine_.Delete(chunk_id);
}

std::size_t ChunkStore::ChunkCount() const {
    return engine_.Size();
}

}  // namespace aifs::storage

