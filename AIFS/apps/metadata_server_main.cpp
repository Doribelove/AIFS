#include <iostream>

#include "aifs/metadata/metadata_server.h"

int main() {
    aifs::metadata::MetadataServer server;
    std::cout << "AIFS metadata server placeholder\n";
    std::cout << "This teaching build uses in-process calls; see proto/aifs.proto for gRPC APIs.\n";
    std::cout << "Current file count: " << server.FileCount() << '\n';
    return 0;
}

