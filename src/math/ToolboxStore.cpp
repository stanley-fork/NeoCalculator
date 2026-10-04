#include "ToolboxStore.h"
#ifdef ARDUINO
#include <LittleFS.h>
#else
#include "hal/FileSystem.h"
#endif
#include <algorithm>
#include <cstring>
#include <new>

namespace numos::toolbox {
namespace {
constexpr const char* paths[]={"/toolbox-favorites-a.dat","/toolbox-favorites-b.dat"};
uint16_t u16(const uint8_t* p) { return uint16_t(p[0]) | uint16_t(p[1])<<8; }
uint32_t u32(const uint8_t* p) { return uint32_t(u16(p)) | uint32_t(u16(p+2))<<16; }
void put16(uint8_t* p,uint16_t x) {p[0]=x&255;p[1]=x>>8;}
void put32(uint8_t* p,uint32_t x) {put16(p,x&65535);put16(p+2,x>>16);}
uint32_t crc(const uint8_t* p,size_t n) {
    uint32_t value=0xffffffffu;
    for(size_t i=0;i<n;++i) {value^=p[i];for(unsigned b=0;b<8;++b)value=(value>>1)^(0xedb88320u&uint32_t(-int(value&1)));}
    return ~value;
}
StoreStatus read(const char* path,std::array<Identity,Store::kFavorites>& ids,size_t& count,uint32_t& generation) {
    auto file=LittleFS.open(path,"r");
    if(!file) return LittleFS.exists(path) ? StoreStatus::Unavailable : StoreStatus::Missing;
    uint8_t bytes[Store::kRecordBytes]{};
    const size_t size=file.size();
    // Read the header even for a larger, future record; never overwrite it.
    const size_t amount=file.read(bytes,std::min(size,sizeof(bytes)));
    if(amount>=6 && !std::memcmp(bytes,"NTBX",4) && u16(bytes+4)>1) return StoreStatus::Future;
    if(size>sizeof(bytes) || amount!=size) return StoreStatus::Damaged;
    return Store::decode(bytes,size,ids,count,generation);
}
}
Store& Store::instance() { static Store store; return store; }
StoreStatus Store::decode(const uint8_t* p,size_t n,std::array<Identity,kFavorites>& ids,size_t& count,uint32_t& gen) {
    count=0;gen=0;
    if(n<6 || std::memcmp(p,"NTBX",4)) return StoreStatus::Damaged;
    if(u16(p+4)>1) return StoreStatus::Future;
    if(u16(p+4)!=1 || n<16 || u16(p+6)>kFavorites || n!=16+4*size_t(u16(p+6))) return StoreStatus::Damaged;
    if(u32(p+n-4)!=crc(p,n-4)) return StoreStatus::Damaged;
    gen=u32(p+8);
    for(unsigned i=0;i<u16(p+6);++i) {
        Identity id{u16(p+12+4*i),u16(p+14+4*i)};
        if(!find(id) || std::find(ids.begin(),ids.begin()+count,id)!=ids.begin()+count) continue;
        ids[count++]=id;
    }
    return StoreStatus::Ready;
}
void Store::load() noexcept {
#if defined(__cpp_exceptions)
    try {
#endif
    if(_status!=StoreStatus::Unloaded) return;
    _status=StoreStatus::Missing;
    for(int i=0;i<2;++i) {
        std::array<Identity,kFavorites> ids{};size_t count=0;uint32_t gen=0;
        const auto result=read(paths[i],ids,count,gen);
        if(result==StoreStatus::Future) {_status=result;return;}
        if(result==StoreStatus::Ready && (_active<0 || int32_t(gen-_generation)>0)) {
            _favorites=ids;_count=count;_generation=gen;_active=i;_status=StoreStatus::Ready;
        } else if(_active<0 && result!=StoreStatus::Missing) _status=result;
    }
#if defined(__cpp_exceptions)
    } catch(const std::bad_alloc&) {_status=StoreStatus::Unavailable;}
#endif
}
bool Store::contains(Identity id) const { return std::find(_favorites.begin(),_favorites.begin()+_count,id)!=_favorites.begin()+_count; }
FavoriteResult Store::toggle(Identity id) {
    if(!find(id)) return FavoriteResult::Invalid;
    for(size_t i=0;i<_count;++i) if(_favorites[i]==id) {
        for(size_t j=i+1;j<_count;++j) _favorites[j-1]=_favorites[j];
        --_count;_dirty=true;return FavoriteResult::Removed;
    }
    if(_count==kFavorites) return FavoriteResult::Full;
    _favorites[_count++]=id;_dirty=true;return FavoriteResult::Added;
}
bool Store::move(Identity id,int delta) {
    if(delta!=-1 && delta!=1)return false;
    for(size_t i=0;i<_count;++i) if(_favorites[i]==id) {
        const int target=int(i)+delta;
        if(target<0 || size_t(target)>=_count) return false;
        std::swap(_favorites[i],_favorites[target]);_dirty=true;return true;
    }
    return false;
}
void Store::inserted(Identity id) {
    size_t at=0;while(at<_recentCount && _recent[at]!=id) ++at;
    if(at==_recentCount && _recentCount<kRecent) ++_recentCount;
    at=std::min(at,kRecent-1);
    while(at>0) {_recent[at]=_recent[at-1];--at;}
    _recent[0]=id;
}
bool Store::save() noexcept {
#if defined(__cpp_exceptions)
    try {
#endif
    if(!_dirty) return true;
    if(_status==StoreStatus::Future) return false;
    uint8_t data[kRecordBytes]{};
    std::memcpy(data,"NTBX",4);put16(data+4,1);put16(data+6,uint16_t(_count));put32(data+8,_generation+1);
    for(size_t i=0;i<_count;++i){put16(data+12+4*i,_favorites[i].id);put16(data+14+4*i,_favorites[i].variant);}
    const size_t n=16+4*_count;put32(data+n-4,crc(data,n-4));
    const int target=_active==0 ? 1 : 0;
    // WHY: writing only the inactive slot preserves the last good record on
    // interruption. No format, deletion, or platform-dependent replace rename.
    auto file=LittleFS.open(paths[target],"w");
    if(!file || file.write(data,n)!=n) {_status=StoreStatus::Unavailable;return false;}
    file.close();
    std::array<Identity,kFavorites> check{};size_t count=0;uint32_t gen=0;
    if(read(paths[target],check,count,gen)!=StoreStatus::Ready || count!=_count || gen!=_generation+1 ||
        !std::equal(check.begin(),check.begin()+count,_favorites.begin())) {_status=StoreStatus::Unavailable;return false;}
    _active=target;++_generation;_dirty=false;_status=StoreStatus::Ready;return true;
#if defined(__cpp_exceptions)
    } catch(const std::bad_alloc&) {_status=StoreStatus::Unavailable;return false;}
#endif
}
}
