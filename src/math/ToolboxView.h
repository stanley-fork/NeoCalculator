#pragma once
#include "ToolboxCatalog.h"
#include <array>
#include <vector>

namespace numos::toolbox {
// One session, one immutable admission context. No AST/history ownership, CAS,
// widgets, translated strings or persisted indexes. Release on modal close.
class View {
public:
    using Filter=bool(*)(void*,const Entry&);
    void prepare(uint8_t capabilities,void* owner,Filter filter,bool unitFamilies) {
        caps_=capabilities;filtered_=filter!=nullptr;families_=unitFamilies;
        flags_.assign(entryCount(),0);groupsUsed_=0;matches_=0;
        const Entry* previous=nullptr;bool allowed=false;
        for(size_t i=0;i<flags_.size();++i) {
            const auto& e=*entryAt(i);
            // WHY: the opt-in filter depends on the entire item's dimensions,
            // powers, domain and admission, all invariant across its validated
            // offered prefixes. Confirmation still performs exact validation.
            if(!families_ || !previous || e.recipe!=Recipe::Unit ||
               previous->recipe!=Recipe::Unit || previous->argument!=e.argument)
                allowed=!filter || filter(owner,e);
            const bool capable=(e.recipe!=Recipe::Unit && e.recipe!=Recipe::QuantityReference) || available(e,caps_);
            flags_[i]=(allowed && capable?Visible:0)|NoMatch;previous=&e;
        }
    }
    bool visible(const Entry& e)const {
        const auto i=entryIndex(e);return i<flags_.size() && (flags_[i]&Visible);
    }
    bool visible(const Row& row,unsigned depth=0)const {
        if(!(row.capabilities&caps_))return false;
        if(!filtered_)return true;
        if(row.entry && visible(*row.entry))return true;
        // All children of a built-in unit entry are its offered prefixes.
        if(families_ && row.entry && row.entry->recipe==Recipe::Unit)return false;
        return row.children && depth<6 && count(row.children,depth+1)!=0;
    }
    size_t count(uint16_t group,unsigned depth=0)const {
        for(unsigned i=0;i<groupsUsed_;++i)if(groups_[i].id==group && groups_[i].depth==depth)return groups_[i].count;
        const auto& p=catalogProvider();size_t total=0;
        const auto n=p.count(p.context,group);
        for(size_t i=0;i<n;++i)if(visible(p.at(p.context,group,i),depth))++total;
        // Bounded memoization, not a catalogue limit: on saturation compute the
        // complete answer. Counts depend only on this session's admission.
        if(groupsUsed_<groups_.size())groups_[groupsUsed_++]={group,uint8_t(depth),total};
        return total;
    }
    Row at(uint16_t group,size_t index)const {
        const auto& p=catalogProvider();const auto n=p.count(p.context,group);
        for(size_t i=0;i<n;++i){auto row=p.at(p.context,group,i);if(visible(row) && index--==0)return row;}
        return {};
    }
    void search(const char* query) {
        matches_=0;
        // One rank per admitted identity and query, not per displayed row.
        // Reuses the allocation prepared at open; includes every rank/result.
        for(size_t i=0;i<flags_.size();++i) {
            auto& flags=flags_[i];flags=(flags&Visible)|NoMatch;
            if(!(flags&Visible))continue;
            const auto& e=*entryAt(i);if(!discoverable(e))continue;
            const auto rank=searchRank(e,query);
            if(rank<4){flags=Visible|uint8_t(rank);++matches_;}
        }
    }
    size_t matches()const{return matches_;}
    Row searchAt(size_t index)const {
        for(unsigned rank=0;rank<4;++rank)for(size_t i=0;i<flags_.size();++i)
            if(flags_[i]==(Visible|rank) && index--==0){const auto* e=entryAt(i);return {e,e->options,e->en,e->es};}
        return {};
    }
private:
    static constexpr uint8_t Visible=0x80,NoMatch=4;
    struct Group {uint16_t id;uint8_t depth;size_t count;};
    std::vector<uint8_t> flags_;
    mutable std::array<Group,64> groups_{};
    mutable unsigned groupsUsed_=0;
    size_t matches_=0;
    uint8_t caps_=0;
    bool filtered_=false,families_=false;
};
}
