#include "aifs/client/io_pipeline.h"

namespace aifs::client {

IOPipeline::IOPipeline(std::size_t max_inflight) : max_inflight_(max_inflight) {}

std::vector<std::string> IOPipeline::Split(const std::string& data, std::size_t chunk_size) const {
    std::vector<std::string> chunks;
    if (chunk_size == 0) {
        return chunks;
    }
    for (std::size_t offset = 0; offset < data.size(); offset += chunk_size) {
        chunks.push_back(data.substr(offset, chunk_size));
    }
    if (chunks.empty()) {
        chunks.emplace_back();
    }
    return chunks;
}

std::string IOPipeline::Join(const std::vector<std::string>& chunks) const {
    std::string result;
    for (const auto& chunk : chunks) {
        result += chunk;
    }
    return result;
}

std::size_t IOPipeline::max_inflight() const {
    return max_inflight_;
}

}  // namespace aifs::client

