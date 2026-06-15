#include "aifs/common/types.h"

namespace aifs::common {

std::string ToString(StorageTier tier) {
    switch (tier) {
        case StorageTier::kHot:
            return "HOT";
        case StorageTier::kWarm:
            return "WARM";
        case StorageTier::kCold:
            return "COLD";
    }
    return "UNKNOWN";
}

StorageTier TierFromString(const std::string& value) {
    if (value == "HOT") {
        return StorageTier::kHot;
    }
    if (value == "COLD") {
        return StorageTier::kCold;
    }
    return StorageTier::kWarm;
}

}  // namespace aifs::common
