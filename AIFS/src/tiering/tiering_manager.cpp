#include "aifs/tiering/tiering_manager.h"

namespace aifs::tiering {

void TieringManager::RecordAccess(const std::string& path) {
    auto& stat = stats_[path];
    stat.count += 1;
    stat.last_access = std::chrono::steady_clock::now();
}

common::StorageTier TieringManager::Classify(const std::string& path) const {
    auto it = stats_.find(path);
    if (it == stats_.end()) {
        return common::StorageTier::kCold;
    }
    if (it->second.count >= 5) {
        return common::StorageTier::kHot;
    }
    if (it->second.count >= 2) {
        return common::StorageTier::kWarm;
    }
    return common::StorageTier::kCold;
}

}  // namespace aifs::tiering

