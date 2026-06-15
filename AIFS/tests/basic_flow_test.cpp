#include <cassert>
#include <memory>
#include <string>

#include "aifs/client/aifs_client.h"
#include "aifs/metadata/metadata_server.h"
#include "aifs/qos/checkpoint_scheduler.h"
#include "aifs/qos/token_bucket.h"
#include "aifs/storage/chunk_store.h"
#include "aifs/storage/replica_manager.h"

int main() {
    using namespace aifs;

    metadata::MetadataServer metadata(metadata::PlacementManager(8, 2));
    storage::ReplicaManager replicas;
    replicas.AddStore(std::make_shared<storage::ChunkStore>("node-a"));
    replicas.AddStore(std::make_shared<storage::ChunkStore>("node-b"));
    qos::CheckpointScheduler scheduler(qos::TokenBucket(4096, 4096));
    client::AIFSClient client(metadata, replicas, scheduler);

    common::DatasetManifest manifest;
    manifest.dataset_id = "unit-dataset";
    manifest.entries = {
        {"/dataset/a.bin", 4, "a", {"train"}},
        {"/dataset/b.bin", 4, "b", {"train"}},
    };
    assert(client.RegisterManifest(manifest).ok());

    common::RequestContext ctx;
    ctx.tenant_id = "tenant-test";
    ctx.dataset_id = "unit-dataset";

    const std::string path = "/dataset/a.bin";
    const std::string payload = "0123456789abcdef";
    assert(client.WriteFile(path, payload, ctx).ok());

    std::string out;
    assert(client.ReadFile(path, ctx, &out).ok());
    assert(out == payload);
    assert(client.CacheEntries() > 0);

    auto plan = client.PrefetchPlan("unit-dataset", "/dataset/a.bin", 1);
    assert(plan.size() == 1);
    assert(plan[0] == "/dataset/b.bin");

    return 0;
}

