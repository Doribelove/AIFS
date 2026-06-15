#include "aifs/metadata/namespace_manager.h"

namespace aifs::metadata {

common::Status NamespaceManager::CreateFile(common::FileMetadata metadata) {
    if (metadata.path.empty() || metadata.path.front() != '/') {
        return common::Status::InvalidArgument("file path must be absolute");
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (files_.find(metadata.path) != files_.end()) {
        return common::Status::AlreadyExists("file already exists: " + metadata.path);
    }
    files_.emplace(metadata.path, std::move(metadata));
    return common::Status::Ok();
}

std::optional<common::FileMetadata> NamespaceManager::GetFile(const std::string& path) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = files_.find(path);
    if (it == files_.end()) {
        return std::nullopt;
    }
    return it->second;
}

common::Status NamespaceManager::DeleteFile(const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (files_.erase(path) == 0) {
        return common::Status::NotFound("file not found: " + path);
    }
    return common::Status::Ok();
}

std::size_t NamespaceManager::FileCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return files_.size();
}

}  // namespace aifs::metadata

