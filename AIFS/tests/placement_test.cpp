#include <cstdint>
#include <iostream>
#include <limits>

#include "aifs/metadata/placement_manager.h"

int main() {
    using aifs::metadata::PlacementManager;

    // Only chunk metadata is allocated, regardless of the logical file size.
    const auto max_size = std::numeric_limits<std::uint64_t>::max();
    PlacementManager placement(max_size - 1, 1);
    const auto chunks = placement.PlanChunks("/large", max_size, {"node-a"});
    if (chunks.size() != 2 || chunks[0].size != max_size - 1 || chunks[1].size != 1) {
        std::cerr << "Expected a full chunk and a one-byte tail\n";
        return 1;
    }
    if (!PlacementManager(0, 1).PlanChunks("/file", 1, {"node-a"}).empty()) {
        std::cerr << "A zero chunk size cannot produce a placement\n";
        return 1;
    }
    return 0;
}
