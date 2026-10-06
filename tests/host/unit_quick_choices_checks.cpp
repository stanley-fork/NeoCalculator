#include "ui/UnitQuickChoices.h"
#include <cassert>
#include <cstdio>
using namespace numos;
quantity::Descriptor descriptor(unsigned item,unsigned prefix=0){quantity::Descriptor d;assert(quantity::descriptor(item,prefix,d));return d;}
quantity::Value value(const quantity::Descriptor& d){quantity::Value v;assert(quantity::descriptorDimension(d,v.dimension));v.coefficient="20";return v;}
int main() {
    ui::UnitQuickChoices q;
    auto speed=descriptor(110);auto v=value(speed);q.prepare(v,speed);
    assert(q.count==1 && q.ids[0]==(toolbox::Identity{32879,0}));
    auto permuted=speed;std::swap(permuted.terms[0],permuted.terms[1]);
    assert(ui::UnitQuickChoices::same(speed,permuted));assert(ui::UnitQuickChoices::identity(permuted)==(toolbox::Identity{32878,0}));
    auto& store=toolbox::Store::instance();
    assert(store.toggle({32879,0})==toolbox::FavoriteResult::Added);
    assert(store.toggle({32769,15})==toolbox::FavoriteResult::Added);
    q.prepare(v,speed);assert(q.count==1 && q.favorites==1 && q.ids[0]==(toolbox::Identity{32879,0}));
    // Equivalent destination from another item ID cannot duplicate a favorite.
    unsigned checked=0;
    for(const auto& item:units::kItems) {
        quantity::Descriptor current;if(!quantity::descriptor(item.id,uint8_t(item.defaultPrefix),current))continue;
        auto canonical=value(current);q.prepare(canonical,current);
        for(unsigned i=0;i<q.count;++i) {
            auto d=descriptor(q.ids[i].id&0x3fff,q.ids[i].variant);
            assert(quantity::compatible(canonical,d));assert(!ui::UnitQuickChoices::same(current,d));
            for(unsigned j=0;j<i;++j)assert(!ui::UnitQuickChoices::same(d,descriptor(q.ids[j].id&0x3fff,q.ids[j].variant)));
            ++checked;
        }
    }
    store.toggle({32879,0});store.toggle({32769,15});
    for(unsigned id:{10u,26u,27u,28u,119u}) {
        auto d=descriptor(id);q.prepare(value(d),d);
        for(unsigned i=0;i<q.count;++i)assert((q.ids[i].id&0x3fff)==id);
    }
    auto area=descriptor(101,14),length=descriptor(1,14);
    assert(!ui::UnitQuickChoices::same(area,length));
    auto consumption=descriptor(1298);q.prepare(value(consumption),consumption);
    assert(q.count==0); // No area suggestions from dimension alone.
    std::printf("PASS quick choices: %u compatible suggestions; stable identities, favorites and semantic families\n",checked);
}
