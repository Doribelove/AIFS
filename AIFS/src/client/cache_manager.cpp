#include "aifs/client/cache_manager.h"

#include <utility>

namespace aifs::client {

CacheManager::CacheManager(std::size_t capacity_bytes) : capacity_bytes_(capacity_bytes) {}

CacheManager::CacheManager(const CacheManager& other)
    : capacity_bytes_(other.capacity_bytes_),
      used_bytes_(other.used_bytes_),
      lru_(other.lru_),
      entries_(other.entries_) {
    // Copied entries must refer to this cache's LRU nodes.
    for (auto it = lru_.begin(); it != lru_.end(); ++it) {
        entries_.at(*it).order_it = it;
    }
}

CacheManager& CacheManager::operator=(const CacheManager& other) {
    if (this != &other) {
        CacheManager copy(other);
        Swap(copy);
    }
    return *this;
}

CacheManager::CacheManager(CacheManager&& other) noexcept
    : CacheManager(other.capacity_bytes_) {
    Swap(other);
}

CacheManager& CacheManager::operator=(CacheManager&& other) noexcept {
    if (this != &other) {
        CacheManager moved(std::move(other));
        Swap(moved);
    }
    return *this;
}

void CacheManager::Swap(CacheManager& other) noexcept {
    std::swap(capacity_bytes_, other.capacity_bytes_);
    std::swap(used_bytes_, other.used_bytes_);
    lru_.swap(other.lru_);
    entries_.swap(other.entries_);
}

void CacheManager::Put(const std::string& key, std::string value) {
    auto existing = entries_.find(key);
    if (existing != entries_.end()) {
        used_bytes_ -= existing->second.value.size();
        lru_.erase(existing->second.order_it);
        entries_.erase(existing);
    }

    if (value.size() > capacity_bytes_) {
        return;
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

