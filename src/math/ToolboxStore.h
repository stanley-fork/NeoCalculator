#pragma once
#include "ToolboxCatalog.h"
#include <array>
namespace numos::toolbox {
enum class StoreStatus : uint8_t { Unloaded, Ready, Missing, Damaged, Future, Unavailable };
enum class FavoriteResult : uint8_t { Added, Removed, Full, Invalid };
class Store {
public:
    static constexpr size_t kFavorites=24, kRecent=12, kRecordBytes=112;
    static Store& instance();
    void load() noexcept;
    bool save() noexcept;
    FavoriteResult toggle(Identity);
    bool move(Identity, int delta);
    bool contains(Identity) const;
    void inserted(Identity);
    size_t favoriteCount() const { return _count; }
    size_t recentCount() const { return _recentCount; }
    Identity favorite(size_t i) const { return i<_count ? _favorites[i] : Identity{}; }
    Identity recent(size_t i) const { return i<_recentCount ? _recent[i] : Identity{}; }
    StoreStatus status() const { return _status; }
    bool dirty() const { return _dirty; }
    // Bounded codec is testable independently of flash/IDBFS.
    static StoreStatus decode(const uint8_t*,size_t,std::array<Identity,kFavorites>&,size_t&,uint32_t&);
private:
    std::array<Identity,kFavorites> _favorites{};
    std::array<Identity,kRecent> _recent{};
    size_t _count=0, _recentCount=0;
    uint32_t _generation=0;
    int _active=-1;
    bool _dirty=false;
    StoreStatus _status=StoreStatus::Unloaded;
};
}
