#include "aifs/metadata/metadata_server.h"

namespace aifs::metadata {

MetadataServer::MetadataServer(PlacementManager placement)
    : placement_manager_(placement) {}

common::Status MetadataServer::RegisterManifest(common::DatasetManifest manifest) {
    return manifest_manager_.RegisterManifest(std::move(manifest));
}

common::Status MetadataServer::CreateFile(
    const std::string& path,
    std::uint64_t size,
    const common::RequestContext& context,
    const std::vector<std::string>& storage_nodes,
    common::FileMetadata* out_metadata) {
    if (out_metadata == nullptr) {
        return common::Status::InvalidArgument("out_metadata is null");
    }
    auto chunks = placement_manager_.PlanChunks(path, size, storage_nodes);
    if (chunks.empty() && size > 0) {
        return common::Status::InvalidArgument("no storage node available");
    }

    common::FileMetadata metadata;
    metadata.path = path;
    metadata.size = size;
    metadata.owner_tenant = context.tenant_id;
    metadata.version = 1;
    metadata.chunks = std::move(chunks);

    auto status = namespace_manager_.CreateFile(metadata);
    if (!status.ok()) {
        return status;
    }
    *out_metadata = std::move(metadata);
    return common::Status::Ok();
}

std::optional<common::FileMetadata> MetadataServer::GetFile(const std::string& path) const {
    return namespace_manager_.GetFile(path);
}

std::optional<common::DatasetManifest> MetadataServer::GetManifest(const std::string& dataset_id) const {
    return manifest_manager_.GetManifest(dataset_id);
}

std::size_t MetadataServer::FileCount() const {
    return namespace_manager_.FileCount();
}

}  // namespace aifs::metadata

