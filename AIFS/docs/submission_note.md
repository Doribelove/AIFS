# 作业提交说明

本项目是 Word 方案设计对应的参考代码实现，项目名称为 AIFS。

## 与方案对应的核心能力

- 元数据管理：支持文件路径、Manifest、Chunk 布局和租户信息。
- 数据平面：支持文件切分为 Chunk，并写入多个 ChunkStore 副本。
- 客户端加速：支持本地 LRU 缓存、Manifest 感知预取计划和 I/O 管线。
- QoS 治理：支持检查点写入的 TokenBucket 准入控制。
- 冷热分层：根据访问频率输出 HOT/WARM/COLD 迁移建议。
- RPC 边界：通过 `proto/aifs.proto` 定义 gRPC 风格接口。

## 说明

该项目是课程作业参考实现，不是生产级文件系统。为降低运行环境依赖，代码中用内存版 `RocksDBEngine` 模拟 RocksDB，用进程内对象调用模拟 gRPC；正式工程可按 `proto/aifs.proto` 替换为真实 RPC 和持久化组件。

