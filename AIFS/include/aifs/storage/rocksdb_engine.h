#pragma once

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

#include "aifs/common/status.h"

namespace aifs::storage {

class RocksDBEngine {
public:
    common::Status Put(const std::string& key, std::string value);
    std::optional<std::string> Get(const std::string& key) const;
    common::Status Delete(const std::string& key);
    std::size_t Size() const;

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::string> kv_;
};

}  // namespace aifs::storage

