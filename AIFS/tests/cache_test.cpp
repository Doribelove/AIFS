#include <iostream>

#include "aifs/client/cache_manager.h"

int main() {
    aifs::client::CacheManager cache(4);
    cache.Put("first", "aa");
    cache.Put("second", "bb");
    cache.Get("first");  // Make "second" the least recently used entry.

    cache.Put("oversized", "12345");
    if (cache.UsedBytes() != 4 || cache.EntryCount() != 2 || cache.Get("oversized")) {
        std::cerr << "An oversized chunk displaced cached data\n";
        return 1;
    }

    cache.Put("third", "cc");
    if (cache.Get("second") || cache.Get("first") != "aa" || cache.Get("third") != "cc") {
        std::cerr << "Skipping an oversized chunk changed LRU eviction order\n";
        return 1;
    }

    cache.Put("first", "12345");
    if (cache.Get("first") || cache.Get("third") != "cc" || cache.UsedBytes() != 2) {
        std::cerr << "An oversized replacement must invalidate only its own key\n";
        return 1;
    }
    return 0;
}
