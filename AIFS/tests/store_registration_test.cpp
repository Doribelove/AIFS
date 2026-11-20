#include <iostream>
#include <memory>
#include <string>

#include "aifs/client/aifs_client.h"

int main() {
    using namespace aifs;

    metadata::MetadataServer metadata(metadata::PlacementManager(4, 1));
    storage::ReplicaManager replicas;
    {
        auto store = std::make_shared<storage::ChunkStore>("node-a");
        if (!replicas.AddStore(store).ok() || !replicas.AddStore(store).ok()) {
            std::cerr << "Re-registering the same store should succeed\n";
            return 1;
        }
    }
    qos::CheckpointScheduler scheduler(qos::TokenBucket(1024, 1024));
    const std::string path = "/sample.bin";
    const std::string payload = "sample data";
    {
        client::AIFSClient writer(metadata, replicas, scheduler);
        const auto write = writer.WriteFile(path, payload, {});
        if (!write.ok()) {
            std::cerr << write.ToString() << '\n';
            return 1;
        }
    }

    const auto duplicate = replicas.AddStore(std::make_shared<storage::ChunkStore>("node-a"));
    if (duplicate.code() != common::Status::Code::kAlreadyExists) {
        std::cerr << "Expected a node ID conflict, got " << duplicate.ToString() << '\n';
        return 1;
    }
    client::AIFSClient reader(metadata, replicas, scheduler);
    std::string data;
    const auto read = reader.ReadFile(path, {}, &data);
    if (!read.ok() || data != payload) {
        std::cerr << "Lost acknowledged file after duplicate registration: "
                  << read.ToString() << '\n';
        return 1;
    }
    return 0;
}
