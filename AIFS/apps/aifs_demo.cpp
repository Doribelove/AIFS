#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "aifs/client/aifs_client.h"
#include "aifs/common/types.h"
#include "aifs/metadata/metadata_server.h"
#include "aifs/qos/checkpoint_scheduler.h"
#include "aifs/qos/token_bucket.h"
#include "aifs/storage/chunk_store.h"
#include "aifs/storage/replica_manager.h"
#include "aifs/tiering/migration_manager.h"
#include "aifs/tiering/tiering_manager.h"

namespace {

std::string Repeat(const std::string& value, int times) {
    std::string result;
    for (int i = 0; i < times; ++i) {
        result += value;
    }
    return result;
}

void PrintStatus(const std::string& step, const aifs::common::Status& status) {
    std::cout << "[AIFS] " << step << ": " << status.ToString() << '\n';
}

}  // namespace

int main() {
    using namespace aifs;

    metadata::MetadataServer metadata(metadata::PlacementManager(16, 3));
    storage::ReplicaManager replica_manager;
    replica_manager.AddStore(std::make_shared<storage::ChunkStore>("node-a"));
    replica_manager.AddStore(std::make_shared<storage::ChunkStore>("node-b"));
    replica_manager.AddStore(std::make_shared<storage::ChunkStore>("node-c"));

    qos::CheckpointScheduler checkpoint_scheduler(qos::TokenBucket(1024 * 1024, 512 * 1024));
    client::AIFSClient client(metadata, replica_manager, checkpoint_scheduler);

    common::DatasetManifest manifest;
    manifest.dataset_id = "imagenet-mini";
    manifest.version = 1;
    manifest.entries = {
        {"/datasets/imagenet-mini/sample-0001.bin", 64, "sha256:demo-0001", {"image", "train"}},
        {"/datasets/imagenet-mini/sample-0002.bin", 64, "sha256:demo-0002", {"image", "train"}},
        {"/datasets/imagenet-mini/sample-0003.bin", 64, "sha256:demo-0003", {"image", "train"}},
    };
    PrintStatus("register manifest", client.RegisterManifest(manifest));

    common::RequestContext train_ctx;
    train_ctx.tenant_id = "tenant-ai";
    train_ctx.job_id = "pretrain-001";
    train_ctx.dataset_id = "imagenet-mini";
    train_ctx.priority = 10;

    const auto sample_path = "/datasets/imagenet-mini/sample-0001.bin";
    const auto sample_data = Repeat("AIFS_SAMPLE_BLOCK_", 8);
    PrintStatus("write training sample", client.WriteFile(sample_path, sample_data, train_ctx));

    std::string read_back;
    auto read_status = client.ReadFile(sample_path, train_ctx, &read_back);
    PrintStatus("read training sample", read_status);
    std::cout << "[AIFS] read bytes: " << read_back.size()
              << ", cache entries: " << client.CacheEntries() << '\n';

    const auto plan = client.PrefetchPlan("imagenet-mini", sample_path, 2);
    std::cout << "[AIFS] manifest-aware prefetch plan:\n";
    for (const auto& path : plan) {
        std::cout << "  - " << path << '\n';
    }

    common::RequestContext ckpt_ctx = train_ctx;
    ckpt_ctx.checkpoint = true;
    const auto checkpoint_data = Repeat("CHECKPOINT_BLOCK_", 32);
    PrintStatus(
        "write checkpoint with QoS",
        client.WriteFile("/checkpoints/pretrain-001/epoch-1.ckpt", checkpoint_data, ckpt_ctx));

    tiering::TieringManager tiering;
    for (int i = 0; i < 5; ++i) {
        tiering.RecordAccess(sample_path);
    }
    tiering.RecordAccess("/archive/raw/video-0001.mp4");

    tiering::MigrationManager migration;
    const auto migrations = migration.Plan(
        {sample_path, "/archive/raw/video-0001.mp4", "/cold/unused.bin"},
        tiering);
    std::cout << "[AIFS] tiering migration plan:\n";
    for (const auto& item : migrations) {
        std::cout << "  - " << item.path << " -> " << common::ToString(item.target_tier)
                  << " (" << item.reason << ")\n";
    }

    std::cout << "[AIFS] metadata files: " << metadata.FileCount()
              << ", storage nodes: " << replica_manager.StoreCount() << '\n';
    return read_status.ok() && read_back == sample_data ? 0 : 1;
}

