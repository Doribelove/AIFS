#pragma once

#include <optional>
#include <string>

#include "aifs/common/status.h"
#include "aifs/storage/rocksdb_engine.h"

namespace aifs::storage {

class ChunkStore {
public:
    explicit ChunkStore(std::string node_id);

    const std::string& node_id() const;
    common::Status PutChunk(const std::string& chunk_id, const std::string& data);
    std::optional<std::string> GetChunk(const std::string& chunk_id) const;
    common::Status DeleteChunk(const std::string& chunk_id);
    std::size_t ChunkCount() const;

private:
    std::string node_id_;
    RocksDBEngine engine_;
};

}  // namespace aifs::storage

