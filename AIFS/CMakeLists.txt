cmake_minimum_required(VERSION 3.16)

project(AIFS
    VERSION 0.1.0
    DESCRIPTION "AI-oriented distributed file system teaching reference"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(AIFS_BUILD_TESTS "Build AIFS tests" ON)

add_library(aifs
    src/common/status.cpp
    src/common/types.cpp
    src/metadata/namespace_manager.cpp
    src/metadata/manifest_manager.cpp
    src/metadata/placement_manager.cpp
    src/metadata/metadata_server.cpp
    src/storage/rocksdb_engine.cpp
    src/storage/chunk_store.cpp
    src/storage/replica_manager.cpp
    src/client/cache_manager.cpp
    src/client/prefetch_manager.cpp
    src/client/io_pipeline.cpp
    src/client/aifs_client.cpp
    src/qos/token_bucket.cpp
    src/qos/checkpoint_scheduler.cpp
    src/tiering/tiering_manager.cpp
    src/tiering/migration_manager.cpp
)

target_include_directories(aifs PUBLIC include)

add_executable(aifs_demo apps/aifs_demo.cpp)
target_link_libraries(aifs_demo PRIVATE aifs)

add_executable(aifs_metadata_server apps/metadata_server_main.cpp)
target_link_libraries(aifs_metadata_server PRIVATE aifs)

add_executable(aifs_chunk_server apps/chunk_server_main.cpp)
target_link_libraries(aifs_chunk_server PRIVATE aifs)

if(AIFS_BUILD_TESTS)
    enable_testing()
    add_executable(aifs_basic_flow_test tests/basic_flow_test.cpp)
    target_link_libraries(aifs_basic_flow_test PRIVATE aifs)
    add_test(NAME aifs_basic_flow_test COMMAND aifs_basic_flow_test)
endif()
