#pragma once

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

#include "aifs/common/status.h"
#include "aifs/common/types.h"

namespace aifs::metadata {

class ManifestManager {
public:
    common::Status RegisterManifest(common::DatasetManifest manifest);
    std::optional<common::DatasetManifest> GetManifest(const std::string& dataset_id) const;

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, common::DatasetManifest> manifests_;
};

}  // namespace aifs::metadata

