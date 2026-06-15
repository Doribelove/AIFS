#include <iostream>

#include "aifs/storage/chunk_store.h"

int main() {
    aifs::storage::ChunkStore store("standalone-node");
    std::cout << "AIFS chunk server placeholder\n";
    std::cout << "This teaching build uses an in-memory RocksDBEngine adapter.\n";
    std::cout << "Node id: " << store.node_id() << '\n';
    return 0;
}

