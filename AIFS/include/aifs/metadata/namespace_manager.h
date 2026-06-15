#pragma once

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

#include "aifs/common/status.h"
#include "aifs/common/types.h"

namespace aifs::metadata {

class NamespaceManager {
public:
    common::Status CreateFile(common::FileMetadata metadata);
    std::optional<common::FileMetadata> GetFile(const std::string& path) const;
    common::Status DeleteFile(const std::string& path);
    std::size_t FileCount() const;

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, common::FileMetadata> files_;
};

}  // namespace aifs::metadata

