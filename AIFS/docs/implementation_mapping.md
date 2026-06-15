# 论文方案与代码映射

| Word 章节 | 设计点 | 代码位置 |
| --- | --- | --- |
| 4.4.1 Metadata Layer | 命名空间、Manifest、布局管理 | `include/aifs/metadata`, `src/metadata` |
| 4.4.2 Storage Layer | Chunk、RocksDBEngine、副本 | `include/aifs/storage`, `src/storage` |
| 4.4.3 Client Layer | 缓存、预取、I/O 管线 | `include/aifs/client`, `src/client` |
| 4.4.4 QoS Layer | 检查点队列、TokenBucket | `include/aifs/qos`, `src/qos` |
| 4.4.5 Tiering Layer | HOT/WARM/COLD、迁移建议 | `include/aifs/tiering`, `src/tiering` |
| 4.4.6 RPC Layer | gRPC/Protobuf 服务边界 | `proto/aifs.proto` |

教学版实现优先保证代码结构清晰和流程可演示。真实生产系统还需要补充分布式共识、认证授权、故障注入、异步 I/O 和可观测性。

