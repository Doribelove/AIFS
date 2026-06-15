#pragma once

#include <string>
#include <vector>

namespace aifs::client {

class IOPipeline {
public:
    explicit IOPipeline(std::size_t max_inflight = 16);

    std::vector<std::string> Split(const std::string& data, std::size_t chunk_size) const;
    std::string Join(const std::vector<std::string>& chunks) const;
    std::size_t max_inflight() const;

private:
    std::size_t max_inflight_;
};

}  // namespace aifs::client

