#pragma once
#include "UnitRegistry.h"
#include <array>

namespace numos::units {
// Separate identity space: a physical constant or contextual scale is not a
// unit, an algebraic constant, or a free variable with the same printed symbol.
enum class ReferenceId : uint16_t {};
enum class ReferenceKind : uint8_t { PhysicalConstant, Scale, Contextual };
struct ReferenceAtom {
    ReferenceId reference{}; PrefixId prefix{};
    constexpr bool operator==(ReferenceAtom other) const {
        return reference==other.reference && prefix==other.prefix;
    }
};
struct Reference {
    ReferenceId id; uint16_t category; ReferenceKind kind; Exactness exactness;
    uint32_t prefixes;
    const char *key,*symbol,*en,*es,*aliases,*symbolAliases,*quantity;
    int8_t dimension[7]; bool dimensionKnown;
    const char *nominal,*uncertainty,*formula,*source,*location;
    const char *base,*subscript,*denominatorBase,*denominatorSubscript,*superscript;
    Exact scale; bool hasScale;
};
#include "ReferenceRegistryData.inc"
inline const Reference* reference(ReferenceId id) {
    for(const auto& r:kReferences)if(r.id==id)return &r;
    return nullptr;
}
inline bool valid(ReferenceAtom a) {
    const auto* r=reference(a.reference);const auto p=static_cast<unsigned>(a.prefix);
    return r && p<25 && (r->prefixes & (uint32_t(1)<<p));
}
inline bool symbol(ReferenceAtom a,char* out,size_t capacity) {
    if(!valid(a)||!capacity)return false;
    size_t used=0;
    for(const char* part:{prefix(a.prefix)->symbol,reference(a.reference)->symbol})
        for(;*part;++part) {
            if(used+1>=capacity){out[0]=0;return false;}
            out[used++]=*part;
        }
    out[used]=0;return true;
}
inline bool encode(ReferenceAtom a,uint8_t (&bytes)[5]) {
    if(!valid(a))return false;
    const auto id=static_cast<uint16_t>(a.reference);
    bytes[0]=0x52;bytes[1]=1;bytes[2]=uint8_t(id);bytes[3]=uint8_t(id>>8);bytes[4]=uint8_t(a.prefix);return true;
}
inline bool decodeReference(const uint8_t* bytes,size_t length,ReferenceAtom& destination) {
    if(!bytes || length!=5 || bytes[0]!=0x52 || bytes[1]!=1)return false;
    const ReferenceAtom a{ReferenceId(uint16_t(bytes[2]) | uint16_t(bytes[3])<<8),PrefixId(bytes[4])};
    if(!valid(a))return false;
    destination=a;return true;
}
}
