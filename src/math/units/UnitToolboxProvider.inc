// Included by ToolboxCatalog.cpp inside its private namespace.
// The immutable registry itself has no dependency on this adapter.
constexpr Entry unitEntries[] = {
#include "UnitToolboxEntries.inc"
};
const Entry* unitEntry(uint16_t item,uint16_t prefix) {
    size_t lo=0,hi=std::size(unitEntries);
    const uint32_t key=(uint32_t(item)<<16)|prefix;
    while(lo<hi) {
        const size_t mid=lo+(hi-lo)/2;const auto& e=unitEntries[mid];
        const uint32_t candidate=(uint32_t(e.argument)<<16)|e.identity.variant;
        if(candidate<key)lo=mid+1;else hi=mid;
    }
    return lo<std::size(unitEntries) && unitEntries[lo].argument==item && unitEntries[lo].identity.variant==prefix ? &unitEntries[lo]:nullptr;
}
constexpr Entry referenceEntries[] = {
#include "ReferenceToolboxEntries.inc"
};
#include "UnitVariantMetadata.inc"
const UnitVariantMetadata* variantMetadata(Identity id) {
    size_t lo=0,hi=std::size(unitVariantMetadata);const uint32_t key=uint32_t(id.id)<<16|id.variant;
    while(lo<hi){size_t mid=(lo+hi)/2;const auto v=unitVariantMetadata[mid].identity;if((uint32_t(v.id)<<16|v.variant)<key)lo=mid+1;else hi=mid;}
    return lo<std::size(unitVariantMetadata) && unitVariantMetadata[lo].identity==id?&unitVariantMetadata[lo]:nullptr;
}
bool exactSymbolAlias(const Entry& e,const char* query) {
    const auto* meta=variantMetadata(e.identity);if(!meta)return false;
    const size_t n=std::strlen(query);
    for(const char* start=meta->symbols,*p=start;;++p)if(!*p || *p=='|') {
        if(size_t(p-start)==n && !std::strncmp(start,query,n))return true;
        if(!*p)break;start=p+1;
    }
    return false;
}
const Entry* referenceEntry(uint16_t id,uint16_t prefix) {
    size_t lo=0,hi=std::size(referenceEntries);const uint32_t key=uint32_t(id)<<16|prefix;
    while(lo<hi){const size_t mid=(lo+hi)/2;const auto& e=referenceEntries[mid];if((uint32_t(e.argument)<<16|e.identity.variant)<key)lo=mid+1;else hi=mid;}
    return lo<std::size(referenceEntries) && referenceEntries[lo].argument==id && referenceEntries[lo].identity.variant==prefix?&referenceEntries[lo]:nullptr;
}
size_t unitEntryCount(const void*) {return std::size(unitEntries)+std::size(referenceEntries);}
const Entry* unitEntryAt(const void*,size_t index) {
    return index<std::size(unitEntries)?&unitEntries[index]:index<std::size(unitEntries)+std::size(referenceEntries)?&referenceEntries[index-std::size(unitEntries)]:nullptr;
}
bool unitSymbol(const Entry& entry,char* out,size_t capacity) {
    if(entry.recipe==Recipe::QuantityReference)return units::symbol({units::ReferenceId(entry.argument),units::PrefixId(entry.identity.variant)},out,capacity);
    const auto* item=units::item(uint16_t(entry.argument));if(!item || !capacity)return false;
    size_t offset=0;out[0]=0;
    auto append=[&](const char* text){while(*text){if(offset+1>=capacity)return false;out[offset++]=*text++;}out[offset]=0;return true;};
    unsigned positive=0,negative=0;for(unsigned j=0;j<item->termCount;++j)(item->terms[j].power>0?positive:negative)++;
    const unsigned counts[]={positive+(item->coefficientNumerator!=1),negative+(item->coefficientDenominator!=1)};
    if(!positive && item->coefficientNumerator==1 && !append("1"))return false;
    for(unsigned side=0;side<2;++side) {
        if(side && !counts[1])break;
        if(side && (!append("/") || (counts[1]>1 && !append("("))))return false;
        unsigned seen=0;
        const uint32_t coefficient=side?item->coefficientDenominator:item->coefficientNumerator;
        if(coefficient!=1){char text[16]{};std::snprintf(text,sizeof(text),"%lu",static_cast<unsigned long>(coefficient));if(!append(text))return false;++seen;}
        for(unsigned j=0;j<item->termCount;++j) {
            auto term=item->terms[j];if((term.power<0)!=bool(side))continue;
            if(item->prefixSelector && j==item->prefixComponent)term.atom.prefix=units::PrefixId(entry.identity.variant);
            char text[32]{};if(!units::symbol(term.atom,text,sizeof(text)))return false;
            const int power=term.power<0?-term.power:term.power;
            if((seen++ && !append("·")) || !append(text) || !append(power==2?"²":power==3?"³":power==4?"⁴":""))return false;
        }
        if(side && counts[1]>1 && !append(")"))return false;
    }
    return true;
}
size_t unitCount(const void*,uint16_t group) {
    size_t n=0;
    if(group&0x8000) {
        const auto* item=units::item(group&0x3fff);if(!item || !item->prefixSelector)return 0;
        for(const auto p:units::kPrefixOrder)if(unitEntry(item->id,uint8_t(p)))++n;
    } else if(group&0x4000) {
        for(const auto p:units::kPrefixOrder)if(referenceEntry(group&0x3fff,uint8_t(p)))++n;
    } else {
        for(const auto& c:units::kCategories)if(c.parent==group)++n;
        for(const auto& i:units::kItems)if(i.category==group)++n;
        for(const auto& r:units::kReferences)if(r.category==group)++n;
    }
    return n;
}
Row unitAt(const void*,uint16_t group,size_t index) {
    if(group&0xc000) {
        for(const auto p:units::kPrefixOrder) {
            const auto* e=group&0x8000?unitEntry(group&0x3fff,uint8_t(p)):referenceEntry(group&0x3fff,uint8_t(p));
            if(e && index--==0)return {e,0,e->en,e->es,Calculation};
        }
    } else {
        for(const auto& c:units::kCategories)if(c.parent==group && index--==0)return {nullptr,c.id,c.en,c.es,Calculation};
        for(const auto& i:units::kItems)if(i.category==group && index--==0) {
            const auto* e=unitEntry(i.id,uint8_t(i.defaultPrefix));return e?Row{e,e->options,e->en,e->es,Calculation}:Row{};
        }
        for(const auto& r:units::kReferences)if(r.category==group && index--==0) {
            const auto* e=referenceEntry(uint16_t(r.id),0);return e?Row{e,e->options,e->en,e->es,Calculation}:Row{};
        }
    }
    return {};
}
size_t unitInitialFrom(const void*,uint16_t group,Identity origin) {
    if(!(group&0xc000))return 0;
    const auto* item=group&0x8000?units::item(group&0x3fff):nullptr;
    const auto wanted=origin.id==group?origin.variant:item?uint8_t(item->defaultPrefix):0;
    for(size_t i=0;i<unitCount(nullptr,group);++i)if(unitAt(nullptr,group,i).entry->identity.variant==wanted)return i;
    return 0;
}
size_t unitInitial(const void*,uint16_t group) {return unitInitialFrom(nullptr,group,{});}
vpam::NodePtr unitTerm(units::Term term) {
    auto unit=vpam::makeUnit(term.atom);if(!unit)return {};
    const int power=term.power<0?-term.power:term.power;if(power==1)return unit;
    auto base=vpam::makeRow(),exponent=vpam::makeRow();
    static_cast<vpam::NodeRow*>(base.get())->appendChild(std::move(unit));
    static_cast<vpam::NodeRow*>(exponent.get())->appendChild(vpam::makeNumber(power==2?"2":power==3?"3":"4"));
    return vpam::makePower(std::move(base),std::move(exponent));
}
Prepared buildUnit(const Entry& entry) {
    if(entry.recipe==Recipe::QuantityReference) {
        if(referenceEntry(uint16_t(entry.argument),entry.identity.variant)!=&entry)return {};
        return {vpam::makeQuantityReference({units::ReferenceId(entry.argument),units::PrefixId(entry.identity.variant)})};
    }
    const auto* item=units::item(uint16_t(entry.argument));
    if(!item || unitEntry(item->id,entry.identity.variant)!=&entry)return {};
    if(item->termCount==1 && item->terms[0].power>0 && item->coefficientNumerator==1 && item->coefficientDenominator==1) {
        auto term=item->terms[0];if(item->prefixSelector)term.atom.prefix=units::PrefixId(entry.identity.variant);
        return {unitTerm(term)};
    }
    auto numerator=vpam::makeRow(),denominator=vpam::makeRow();
    auto* num=static_cast<vpam::NodeRow*>(numerator.get());auto* den=static_cast<vpam::NodeRow*>(denominator.get());
    if(item->coefficientNumerator!=1){char text[16]{};std::snprintf(text,sizeof(text),"%lu",static_cast<unsigned long>(item->coefficientNumerator));num->appendChild(vpam::makeNumber(text));}
    if(item->coefficientDenominator!=1){char text[16]{};std::snprintf(text,sizeof(text),"%lu",static_cast<unsigned long>(item->coefficientDenominator));den->appendChild(vpam::makeNumber(text));}
    // WHY: all components are built privately; cancellation or failure cannot
    // publish half of a compound unit into the editor.
    for(unsigned j=0;j<item->termCount;++j) {
        auto term=item->terms[j];if(item->prefixSelector && j==item->prefixComponent)term.atom.prefix=units::PrefixId(entry.identity.variant);
        auto node=unitTerm(term);if(!node)return {};
        auto* row=term.power<0?den:num;
        if(row->childCount())row->appendChild(vpam::makeOperator(vpam::OpKind::UnitProduct));
        row->appendChild(std::move(node));
    }
    if(!den->childCount())return {std::move(numerator)};
    if(!num->childCount())num->appendChild(vpam::makeNumber("1"));
    return {vpam::makeFraction(std::move(numerator),std::move(denominator))};
}
void normalizeUnitSymbolQuery(const char* query,char (&normalized)[65]) {
    size_t n=0;for(size_t i=0;query[i] && n+4<sizeof(normalized);) {
        if(uint8_t(query[i])==0xce && uint8_t(query[i+1])==0xbc){normalized[n++]=char(0xc2);normalized[n++]=char(0xb5);i+=2;}
        else if(uint8_t(query[i])==0xe2 && uint8_t(query[i+1])==0x84 && uint8_t(query[i+2])==0xa6){normalized[n++]=char(0xce);normalized[n++]=char(0xa9);i+=3;}
        else normalized[n++]=query[i++];
    }
    normalized[n]=0;
}
bool unitSymbolQuery(const char* query) {
    char normalized[65]{};normalizeUnitSymbolQuery(query,normalized);
    // Single UI-thread cache; 66 bytes, recomputed only when the query changes.
    // No AST, widget, Giac call or allocation for symbol classification.
    static char previous[65]{};static bool isSymbol=false;
    if(std::strcmp(previous,normalized)) {
        std::memcpy(previous,normalized,sizeof(previous));isSymbol=false;
        for(size_t i=0;i<unitEntryCount(nullptr);++i) {
            const auto& other=*unitEntryAt(nullptr,i);
            char candidate[128]{};unitSymbol(other,candidate,sizeof(candidate));
            if(!std::strcmp(candidate,normalized) || exactSymbolAlias(other,normalized)){isSymbol=true;break;}
        }
    }
    return isSymbol;
}
unsigned unitSearchRank(const Entry& e,const char* query) {
    char symbol[128]{},normalized[65]{};if(!unitSymbol(e,symbol,sizeof(symbol)))return 99;
    // Equivalent Unicode spellings are accepted only at the unit boundary.
    normalizeUnitSymbolQuery(query,normalized);
    if(!std::strcmp(symbol,normalized) || exactSymbolAlias(e,normalized))return 0;
    if(unitSymbolQuery(query))return 99;
    char q[65];fold(query,q,sizeof(q));if(!q[0])return 3;
    unsigned rank=std::min(rankText(e.en,q),rankText(e.es,q));
    char aliases[192];fold(e.aliases,aliases,sizeof(aliases));
    for(char* start=aliases,*p=aliases;;++p)if(*p=='|' || !*p) {
        const bool end=!*p;*p=0;if(*start)rank=std::min(rank,rankText(start,q));if(end)break;start=p+1;
    }
    const auto* c=units::category(e.category);
    if(c)rank=std::min(rank,std::min(rankText(c->en,q),rankText(c->es,q)));
    return rank;
}
const Provider unitProvider{nullptr,unitCount,unitAt,unitInitial,unitEntryCount,unitEntryAt,buildUnit,unitInitialFrom};
