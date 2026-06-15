#pragma once

#include <string>
#include <vector>

#include "aifs/common/types.h"

namespace aifs::client {

class PrefetchManager {
public:
    std::vector<std::string> BuildPlan(
        const common::DatasetManifest& manifest,
        const std::string& current_path,
        std::size_t window) const;
};

}  // namespace aifs::client

