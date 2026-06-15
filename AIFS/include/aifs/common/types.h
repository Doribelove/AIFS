#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace aifs::common {

enum class StorageTier {
    kHot,
    kWarm,
    kCold,
};

struct RequestContext {
    std::string tenant_id = "default";
    std::string job_id = "interactive";
    std::string dataset_id;
    int priority = 0;
    bool checkpoint = false;
};

struct ChunkLocation {
    std::string chunk_id;
    std::string node_id;
    StorageTier tier = StorageTier::kHot;
    std::uint64_t size = 0;
    std::uint64_t version = 1;
};

struct FileMetadata {
    std::string path;
    std::uint64_t size = 0;
    std::string owner_tenant;
    std::uint64_t version = 1;
    std::vector<ChunkLocation> chunks;
};

struct DatasetEntry {
    std::string path;
    std::uint64_t size = 0;
    std::string checksum;
    std::vector<std::string> labels;
};

struct DatasetManifest {
    std::string dataset_id;
    std::uint64_t version = 1;
    std::vector<DatasetEntry> entries;
};

std::string ToString(StorageTier tier);
StorageTier TierFromString(const std::string& value);

}  // namespace aifs::common

