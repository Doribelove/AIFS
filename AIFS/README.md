# AIFS: AI-Oriented Distributed File System

AIFS 是一个与论文《面向 AI 场景的分布式文件系统架构调研与设计》对应的教学版参考代码项目。项目按 Word 方案中的六层架构组织：Metadata Layer、Storage Layer、Client Layer、QoS Layer、Tiering Layer 和 RPC Layer。

本仓库用于课程作业展示，目标是证明方案可以落到工程结构和核心代码。默认实现使用 C++17 标准库模拟 RocksDB 和 gRPC 的边界，便于在没有外部依赖的环境中阅读、构建和演示；`proto/aifs.proto` 保留正式服务接口，后续可替换为真实 gRPC/RocksDB 实现。

## 架构对应关系

| 论文设计模块 | 工程模块 | 关键类 |
| --- | --- | --- |
| 元数据平面 | Metadata Layer | `MetadataServer`, `NamespaceManager`, `ManifestManager`, `PlacementManager` |
| 数据平面 | Storage Layer | `ChunkStore`, `RocksDBEngine`, `ReplicaManager` |
| 客户端加速层 | Client Layer | `AIFSClient`, `CacheManager`, `PrefetchManager`, `IOPipeline` |
| 检查点与多租户治理 | QoS Layer | `CheckpointScheduler`, `TokenBucket` |
| 冷热分层与生命周期 | Tiering Layer | `TieringManager`, `MigrationManager` |
| 模块通信接口 | RPC Layer | `proto/aifs.proto` |

## 目录结构

```text
AIFS/
├── apps/                 # 演示入口程序
├── docs/                 # 架构说明与模块映射
├── include/aifs/         # C++ 头文件
├── proto/                # Protobuf 接口定义
├── scripts/              # 构建和运行脚本
├── src/                  # C++ 实现
├── tests/                # 基础流程测试
├── CMakeLists.txt
├── Dockerfile
└── docker-compose.yml
```

## 快速运行

需要 CMake 3.16+ 和支持 C++17 的编译器。

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/aifs_demo
```

Windows PowerShell 可运行：

```powershell
.\scripts\run_demo.ps1
```

Linux/macOS 可运行：

```bash
./scripts/run_demo.sh
```

## 演示内容

`aifs_demo` 会模拟以下流程：

1. 创建 MetadataServer 和 3 个 ChunkStore 数据节点。
2. 注册训练数据集 Manifest。
3. 通过 AIFSClient 写入样本文件，文件被切分为 Chunk 并写入副本。
4. 读取样本文件，触发本地缓存和预取路径。
5. 写入训练检查点，通过 QoS 模块进行准入控制。
6. 根据访问热度生成冷热分层迁移建议。

## 后续扩展方向

- 将 `RocksDBEngine` 替换为真实 RocksDB。
- 根据 `proto/aifs.proto` 生成 gRPC Stub，替换当前进程内调用。
- 增加 Raft 日志复制、目录分片和故障注入测试。
- 增加 Prometheus 指标、Web 控制台和多租户审计日志。

