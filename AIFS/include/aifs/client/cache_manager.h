#pragma once

#include <list>
#include <optional>
#include <string>
#include <unordered_map>

namespace aifs::client {

class CacheManager {
public:
    explicit CacheManager(std::size_t capacity_bytes);

    void Put(const std::string& key, std::string value);
    std::optional<std::string> Get(const std::string& key);
    std::size_t UsedBytes() const;
    std::size_t EntryCount() const;

private:
    struct Entry {
        std::string value;
        std::list<std::string>::iterator order_it;
    };

    void EvictIfNeeded();

    std::size_t capacity_bytes_;
    std::size_t used_bytes_ = 0;
    std::list<std::string> lru_;
    std::unordered_map<std::string, Entry> entries_;
};

}  // namespace aifs::client

