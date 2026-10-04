#pragma once
// Pure data contract: no Toolbox, LVGL, CAS, allocator or floating point.
#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace numos::units {
enum class UnitId : uint16_t {};
enum class PrefixId : uint8_t {};
struct Atom {
    UnitId unit{}; PrefixId prefix{};
    constexpr bool operator==(Atom other) const { return unit==other.unit && prefix==other.prefix; }
};
// numerator / denominator * 10^decimal * pi^piPower, in coherent SI.
// WHY: extreme prefixes and cubed units must not underflow/overflow a double.
struct Exact { int64_t numerator; uint64_t denominator; int16_t decimal; int8_t piPower; };
enum class Conversion : uint8_t { Multiplicative, Affine, Formula };
enum class Exactness : uint8_t { Exact, Measured, Historical, ConventionalApproximate, Contextual, Derived };
enum class TemperatureRole : uint8_t { None, Absolute, Difference };
struct Prefix { PrefixId id; int8_t exponent; const char *symbol, *en, *es; };
struct Definition {
    UnitId id;
    const char *key, *symbol, *prefixedSymbol, *en, *es, *aliases, *quantity;
    int8_t dimension[7];
    Exact scale, offset;
    uint32_t standardPrefixes, offeredPrefixes;
    Conversion conversion;
    TemperatureRole temperature;
    const char *source, *location, *variant;
    Exactness exactness;
    // WHY: bit/count domains must not silently become scalar SI quantities.
    const char *domain, *uncertainty, *formula;
};
struct Term { Atom atom; int8_t power; };
struct Item {
    uint16_t id, category;
    const char *en, *es, *quantity;
    Term terms[4]; uint8_t termCount;
    PrefixId defaultPrefix;
    bool prefixSelector;
    uint8_t prefixComponent;
    uint32_t coefficientNumerator, coefficientDenominator;
};
struct Category { uint16_t id, parent; const char *en,*es; };
#include "UnitRegistryData.inc"
inline const Definition* definition(UnitId id) {
    size_t lo=0,hi=sizeof(kDefinitions)/sizeof(*kDefinitions);
    while(lo<hi){const size_t mid=(lo+hi)/2;if(uint16_t(kDefinitions[mid].id)<uint16_t(id))lo=mid+1;else hi=mid;}
    return lo<sizeof(kDefinitions)/sizeof(*kDefinitions) && kDefinitions[lo].id==id?&kDefinitions[lo]:nullptr;
}
inline const Prefix* prefix(PrefixId id) {
    const auto n=static_cast<unsigned>(id);
    return n<sizeof(kPrefixes)/sizeof(*kPrefixes)? &kPrefixes[n]:nullptr;
}
inline const Item* item(uint16_t id) {
    size_t lo=0,hi=sizeof(kItems)/sizeof(*kItems);
    while(lo<hi){const size_t mid=(lo+hi)/2;if(kItems[mid].id<id)lo=mid+1;else hi=mid;}
    return lo<sizeof(kItems)/sizeof(*kItems) && kItems[lo].id==id?&kItems[lo]:nullptr;
}
inline const Category* category(uint16_t id) {
    for(const auto& c:kCategories)if(c.id==id)return &c;
    return nullptr;
}
inline bool valid(Atom a) {
    const auto* d=definition(a.unit);const auto p=static_cast<unsigned>(a.prefix);
    return d && p<25 && (d->standardPrefixes & (uint32_t(1)<<p));
}
inline bool offered(Atom a) {
    const auto* d=definition(a.unit);const auto p=static_cast<unsigned>(a.prefix);
    return d && p<25 && (d->offeredPrefixes & (uint32_t(1)<<p));
}
inline bool symbol(Atom a,char* out,size_t capacity) {
    if(!valid(a)||!capacity)return false;
    const auto* d=definition(a.unit);size_t used=0;
    const char* parts[]={prefix(a.prefix)->symbol,uint8_t(a.prefix)?d->prefixedSymbol:d->symbol};
    for(const char* part:parts)for(;*part;++part) {
        if(used+1>=capacity){out[0]=0;return false;}
        out[used++]=*part;
    }
    out[used]=0;return true;
}
inline bool angularTight(Atom a) {
    const auto id=static_cast<uint16_t>(a.unit);return id>=57 && id<=59;
}
// Closed, versioned machine atom. Decoding is transactional and never treats a
// free identifier as a unit. Larger AST containers keep their own framing.
inline bool encode(Atom a,uint8_t (&bytes)[5]) {
    if(!valid(a))return false;
    const auto id=static_cast<uint16_t>(a.unit);
    bytes[0]=0x55;bytes[1]=1;bytes[2]=uint8_t(id);bytes[3]=uint8_t(id>>8);bytes[4]=uint8_t(a.prefix);return true;
}
inline bool decode(const uint8_t* bytes,size_t length,Atom& destination) {
    if(!bytes || length!=5 || bytes[0]!=0x55 || bytes[1]!=1)return false;
    const Atom a{UnitId(uint16_t(bytes[2]) | (uint16_t(bytes[3])<<8)),PrefixId(bytes[4])};
    if(!valid(a))return false;
    destination=a;return true;
}
}
