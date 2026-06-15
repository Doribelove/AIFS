#pragma once

#include <chrono>
#include <string>
#include <unordered_map>

#include "aifs/common/types.h"

namespace aifs::tiering {

class TieringManager {
public:
    void RecordAccess(const std::string& path);
    common::StorageTier Classify(const std::string& path) const;

private:
    struct AccessStat {
        std::uint64_t count = 0;
        std::chrono::steady_clock::time_point last_access;
    };

    std::unordered_map<std::string, AccessStat> stats_;
};

}  // namespace aifs::tiering

