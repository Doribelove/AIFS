#include "aifs/client/cache_manager.h"

namespace aifs::client {

CacheManager::CacheManager(std::size_t capacity_bytes) : capacity_bytes_(capacity_bytes) {}

void CacheManager::Put(const std::string& key, std::string value) {
    auto existing = entries_.find(key);
    if (existing != entries_.end()) {
        used_bytes_ -= existing->second.value.size();
        lru_.erase(existing->second.order_it);
        entries_.erase(existing);
    }

    used_bytes_ += value.size();
    lru_.push_front(key);
    entries_[key] = Entry{std::move(value), lru_.begin()};
    EvictIfNeeded();
}

std::optional<std::string> CacheManager::Get(const std::string& key) {
    auto it = entries_.find(key);
    if (it == entries_.end()) {
        return std::nullopt;
    }
    lru_.erase(it->second.order_it);
    lru_.push_front(key);
    it->second.order_it = lru_.begin();
    return it->second.value;
}

std::size_t CacheManager::UsedBytes() const {
    return used_bytes_;
}

std::size_t CacheManager::EntryCount() const {
    return entries_.size();
}

void CacheManager::EvictIfNeeded() {
    while (used_bytes_ > capacity_bytes_ && !lru_.empty()) {
        const auto key = lru_.back();
        lru_.pop_back();
        auto it = entries_.find(key);
        if (it != entries_.end()) {
            used_bytes_ -= it->second.value.size();
            entries_.erase(it);
        }
    }
}

}  // namespace aifs::client

