#pragma once

#include <optional>
#include <string>
#include <vector>

#include "aifs/common/status.h"
#include "aifs/common/types.h"
#include "aifs/metadata/manifest_manager.h"
#include "aifs/metadata/namespace_manager.h"
#include "aifs/metadata/placement_manager.h"

namespace aifs::metadata {

class MetadataServer {
public:
    explicit MetadataServer(PlacementManager placement = PlacementManager{});

    common::Status RegisterManifest(common::DatasetManifest manifest);
    common::Status CreateFile(
        const std::string& path,
        std::uint64_t size,
        const common::RequestContext& context,
        const std::vector<std::string>& storage_nodes,
        common::FileMetadata* out_metadata);
    std::optional<common::FileMetadata> GetFile(const std::string& path) const;
    std::optional<common::DatasetManifest> GetManifest(const std::string& dataset_id) const;

    std::size_t FileCount() const;

private:
    NamespaceManager namespace_manager_;
    ManifestManager manifest_manager_;
    PlacementManager placement_manager_;
};

}  // namespace aifs::metadata

