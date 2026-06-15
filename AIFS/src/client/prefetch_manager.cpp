#include "aifs/client/prefetch_manager.h"

namespace aifs::client {

std::vector<std::string> PrefetchManager::BuildPlan(
    const common::DatasetManifest& manifest,
    const std::string& current_path,
    std::size_t window) const {
    std::vector<std::string> plan;
    bool seen_current = false;
    for (const auto& entry : manifest.entries) {
        if (seen_current && plan.size() < window) {
            plan.push_back(entry.path);
        }
        if (entry.path == current_path) {
            seen_current = true;
        }
    }
    return plan;
}

}  // namespace aifs::client

