#include "aifs/tiering/migration_manager.h"

#include "aifs/tiering/tiering_manager.h"

namespace aifs::tiering {

std::vector<MigrationPlan> MigrationManager::Plan(
    const std::vector<std::string>& paths,
    const TieringManager& tiering) const {
    std::vector<MigrationPlan> plans;
    for (const auto& path : paths) {
        const auto tier = tiering.Classify(path);
        std::string reason;
        switch (tier) {
            case common::StorageTier::kHot:
                reason = "high access frequency, keep on NVMe/SSD";
                break;
            case common::StorageTier::kWarm:
                reason = "moderate access frequency, keep on SSD/HDD";
                break;
            case common::StorageTier::kCold:
                reason = "low access frequency, migrate to object storage";
                break;
        }
        plans.push_back(MigrationPlan{path, tier, reason});
    }
    return plans;
}

}  // namespace aifs::tiering

