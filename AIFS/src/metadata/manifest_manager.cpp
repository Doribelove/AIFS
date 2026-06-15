#include "aifs/metadata/manifest_manager.h"

namespace aifs::metadata {

common::Status ManifestManager::RegisterManifest(common::DatasetManifest manifest) {
    if (manifest.dataset_id.empty()) {
        return common::Status::InvalidArgument("dataset_id is required");
    }

    std::lock_guard<std::mutex> lock(mutex_);
    manifests_[manifest.dataset_id] = std::move(manifest);
    return common::Status::Ok();
}

std::optional<common::DatasetManifest> ManifestManager::GetManifest(const std::string& dataset_id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = manifests_.find(dataset_id);
    if (it == manifests_.end()) {
        return std::nullopt;
    }
    return it->second;
}

}  // namespace aifs::metadata

