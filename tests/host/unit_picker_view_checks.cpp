#include "math/ToolboxView.h"
#include "math/ToolboxStore.h"
#include "math/Quantity.h"
#include "math/units/ReferenceRegistry.h"
#include "i18n/Locale.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace numos;
unsigned assertions=0,contexts=0,variants=0,queries=0;
void checked(bool value,unsigned line){++assertions;if(!value){std::fprintf(stderr,"FAIL line %u assertion %u context %u\n",line,assertions,contexts);std::abort();}}
#define check(value) checked((value),__LINE__)
struct Context {quantity::Value value;quantity::Descriptor output;int component=-1;};
// Reference CALC-UNITS-01 predicate, deliberately not the new view/index.
bool referenceFilter(void* owner,const toolbox::Entry& e) {
    const auto& c=*static_cast<Context*>(owner);
    if(e.recipe!=toolbox::Recipe::Unit)return false;
    quantity::Descriptor candidate;
    if(!quantity::descriptor(uint16_t(e.argument),uint8_t(e.identity.variant),candidate))return false;
    if(c.component<0)return quantity::compatible(c.value,candidate);
    if(unsigned(c.component)>=c.output.count || candidate.count!=1 || candidate.terms[0].power!=1 ||
       candidate.numerator!=1 || candidate.denominator!=1)return false;
    return quantity::dimension(candidate.terms[0].atom)==quantity::dimension(c.output.terms[c.component].atom);
}
// Previous provider enumeration, independent of generated adjacency.
std::vector<toolbox::Row> referenceRows(uint16_t group) {
    std::vector<toolbox::Row> rows;
    if(group&0xc000) {
        for(auto p:units::kPrefixOrder)if(auto* e=toolbox::find({group,uint16_t(p)}))
            rows.push_back({e,0,e->en,e->es,toolbox::Calculation});
    } else {
        for(const auto& c:units::kCategories)if(c.parent==group)rows.push_back({nullptr,c.id,c.en,c.es,toolbox::Calculation});
        for(const auto& i:units::kItems)if(i.category==group) {
            auto* e=toolbox::find({uint16_t(0x8000|i.id),uint16_t(i.defaultPrefix)});
            if(e)rows.push_back({e,e->options,e->en,e->es,toolbox::Calculation});
        }
        for(const auto& r:units::kReferences)if(r.category==group) {
            auto* e=toolbox::find({uint16_t(0x4000|uint16_t(r.id)),0});
            if(e)rows.push_back({e,e->options,e->en,e->es,toolbox::Calculation});
        }
    }return rows;
}
bool referenceVisible(Context& c,const toolbox::Row& row,unsigned depth=0) {
    if(!(row.capabilities&toolbox::Calculation))return false;
    if(row.entry && referenceFilter(&c,*row.entry))return true;
    if(!row.children || depth>=6)return false;
    for(auto child:referenceRows(row.children))if(referenceVisible(c,child,depth+1))return true;
    return false;
}
void compareGroup(toolbox::View& view,Context& c,uint16_t group) {
    std::vector<toolbox::Row> expected;
    for(auto row:referenceRows(group))if(referenceVisible(c,row))expected.push_back(row);
    check(view.count(group)==expected.size());
    for(size_t i=0;i<expected.size();++i){const auto got=view.at(group,i);check(got.entry==expected[i].entry && got.children==expected[i].children);}
    const auto last=view.at(group,expected.size());check(!last.entry && !last.children);
}
void compare(Context& c,bool search) {
    ++contexts;toolbox::View view;view.prepare(toolbox::Calculation,&c,referenceFilter,true);
    for(size_t i=0;i<toolbox::entryCount();++i){const auto& e=*toolbox::entryAt(i);check(toolbox::entryIndex(e)==i);check(view.visible(e)==referenceFilter(&c,e));++variants;}
    for(const auto& cat:units::kCategories)compareGroup(view,c,cat.id);
    for(const auto& item:units::kItems)if(item.prefixSelector)compareGroup(view,c,uint16_t(0x8000|item.id));
    // Favorites keep exact identity; filtering cannot replace a prefixed item
    // by its family/default. Same view is valid after favorite changes.
    auto& store=toolbox::Store::instance();
    check(store.favoriteCount()==0);
    const toolbox::Identity favorites[]={{32769,15},{32786,10},{32787,16},{32879,0}};
    for(auto id:favorites)store.toggle(id);
    for(size_t i=0;i<store.favoriteCount();++i){const auto* e=toolbox::find(store.favorite(i));check(e && view.visible(*e)==referenceFilter(&c,*e));}
    for(auto id:favorites)store.toggle(id);
    if(!search)return;
    for(const char* q:{"","m","mA","MA","MHz","mHz","Pa","pA","pH","µs","μs","metro","metre","meter","milímetro","ohmio","Ω","km/h","joule","zzzzz"}) {
        ++queries;std::vector<std::pair<unsigned,const toolbox::Entry*>> expected;
        for(size_t i=0;i<toolbox::entryCount();++i){auto* e=toolbox::entryAt(i);if(referenceFilter(&c,*e) && toolbox::discoverable(*e)){auto rank=toolbox::searchRank(*e,q);if(rank<4)expected.push_back({rank,e});}}
        std::stable_sort(expected.begin(),expected.end(),[](auto a,auto b){return a.first<b.first;});
        view.search(q);check(view.matches()==expected.size());
        for(size_t i=0;i<expected.size();++i)check(view.searchAt(i).entry==expected[i].second);
        check(!view.searchAt(expected.size()).entry);
    }
}
int main() {
    for(auto locale:{i18n::Locale::English,i18n::Locale::EnglishUK,i18n::Locale::Spanish,i18n::Locale::SpanishLatinAmerica}) {
        i18n::productLocale=locale;
        // Every distinct offered item creates a whole-unit context, followed
        // by every component with its real exponent (including denominators).
        for(const auto& item:units::kItems) {
            Context c;if(!quantity::descriptor(item.id,uint8_t(item.defaultPrefix),c.output))continue;
            check(quantity::descriptorDimension(c.output,c.value.dimension));
            c.value.coefficient="1";c.value.meaning=item.terms[0].atom.unit;
            const bool representative=item.id<30 || item.id==101 || item.id==102 || item.id==111 || item.id==114 || item.id>1300;
            compare(c,representative);
            for(unsigned component=0;component<c.output.count;++component){c.component=int(component);compare(c,false);}
        }
        // Unknown domain signatures and an invalid component have no rows,
        // rather than accidentally accepting scalar or seven-dimension peers.
        Context c;c.value.dimension.powers[16]=3;compare(c,true);c.component=17;compare(c,true);
    }
    std::printf("PASS contexts=%u variantComparisons=%u queries=%u assertions=%u\n",contexts,variants,queries,assertions);
}
