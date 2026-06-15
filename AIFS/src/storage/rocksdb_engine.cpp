#include "aifs/storage/rocksdb_engine.h"

namespace aifs::storage {

common::Status RocksDBEngine::Put(const std::string& key, std::string value) {
    if (key.empty()) {
        return common::Status::InvalidArgument("key is empty");
    }
    std::lock_guard<std::mutex> lock(mutex_);
    kv_[key] = std::move(value);
    return common::Status::Ok();
}

std::optional<std::string> RocksDBEngine::Get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = kv_.find(key);
    if (it == kv_.end()) {
        return std::nullopt;
    }
    return it->second;
}

common::Status RocksDBEngine::Delete(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (kv_.erase(key) == 0) {
        return common::Status::NotFound("key not found: " + key);
    }
    return common::Status::Ok();
}

std::size_t RocksDBEngine::Size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return kv_.size();
}

}  // namespace aifs::storage

