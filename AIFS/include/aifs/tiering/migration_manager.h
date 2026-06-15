#pragma once

#include <string>
#include <vector>

#include "aifs/common/types.h"

namespace aifs::tiering {

struct MigrationPlan {
    std::string path;
    common::StorageTier target_tier;
    std::string reason;
};

class MigrationManager {
public:
    std::vector<MigrationPlan> Plan(const std::vector<std::string>& paths, const class TieringManager& tiering) const;
};

}  // namespace aifs::tiering

