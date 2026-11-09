#include <iostream>
#include <utility>

#include "aifs/client/cache_manager.h"

namespace {

bool Check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << message << '\n';
    }
    return condition;
}

}  // namespace

int main() {
    using aifs::client::CacheManager;

    CacheManager source(4);
    source.Put("a", "aa");
    source.Put("b", "bb");
    source.Get("a");
    CacheManager moved(std::move(source));
    if (!Check(source.EntryCount() == 0 && source.UsedBytes() == 0,
               "Moving a cache must leave an empty, reusable source")) {
        return 1;
    }
    source.Put("fresh", "cc");
    if (!Check(source.Get("fresh") == "cc" && source.UsedBytes() == 2,
               "A moved-from cache must accept new entries")) {
        return 1;
    }
    moved.Put("c", "cc");
    if (!Check(!moved.Get("b") && moved.Get("a") == "aa" && moved.Get("c") == "cc",
               "Moving a cache must preserve its LRU order")) {
        return 1;
    }

    CacheManager copied(moved);
    copied.Get("a");
    copied.Put("d", "dd");
    if (!Check(!copied.Get("c") && copied.Get("a") == "aa" && copied.Get("d") == "dd",
               "A copy must manage its own LRU order")) {
        return 1;
    }
    if (!Check(moved.Get("a") == "aa" && moved.Get("c") == "cc" && !moved.Get("d"),
               "Reading and evicting copied entries must not change the original")) {
        return 1;
    }

    CacheManager assigned(1);
    assigned.Put("old", "x");
    {
        CacheManager temporary(4);
        temporary.Put("first", "aa");
        temporary.Put("second", "bb");
        assigned = temporary;
    }
    assigned.Put("third", "cc");
    if (!Check(!assigned.Get("old") && !assigned.Get("first") &&
                   assigned.Get("second") == "bb" && assigned.Get("third") == "cc" &&
                   assigned.UsedBytes() == 4 && assigned.EntryCount() == 2,
               "Copy assignment must preserve capacity and outlive the source")) {
        return 1;
    }
    assigned = assigned;
    if (!Check(assigned.Get("second") == "bb" && assigned.UsedBytes() == 4,
               "Self-copy assignment must preserve cached entries")) {
        return 1;
    }

    CacheManager target(1);
    target.Put("old", "x");
    target = std::move(assigned);
    if (!Check(assigned.EntryCount() == 0 && assigned.UsedBytes() == 0 &&
                   !target.Get("old") && target.Get("second") == "bb",
               "Move assignment must replace old entries and reset the source")) {
        return 1;
    }
    target.Put("fourth", "dd");
    if (!Check(!target.Get("third") && target.Get("second") == "bb" &&
                   target.Get("fourth") == "dd" && target.UsedBytes() == 4,
               "Move assignment must preserve capacity and LRU order")) {
        return 1;
    }
    assigned.Put("fresh", "eeee");
    if (!Check(assigned.Get("fresh") == "eeee", "Move-assigned sources must remain reusable")) {
        return 1;
    }
    auto& same_target = target;
    target = std::move(same_target);
    if (!Check(target.UsedBytes() == 4 && target.Get("second") == "bb",
               "Self-move assignment must preserve cached entries")) {
        return 1;
    }
    return 0;
}
