// SPDX-License-Identifier: GPL-3.0-or-later
#include "math/ToolboxCatalog.h"
#include "math/InputSymbols.h"
#include "math/ToolboxStore.h"
#include "math/CursorController.h"
#include "math/CalculationEngine.h"
#include "ui/MathTypography.h"
#include "hal/FileSystem.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <new>
using namespace numos::toolbox;
using namespace vpam;
namespace {size_t attempts=0,failAt=SIZE_MAX,failed=0,live=0;bool armed=false;unsigned checks=0;}
void* operator new(size_t n) {
    if(armed && ++attempts>=failAt){++failed;throw std::bad_alloc();}
    if(void* p=std::malloc(n?n:1)){++live;return p;}throw std::bad_alloc();
}
void* operator new[](size_t n){return ::operator new(n);}
void operator delete(void* p) noexcept {if(p){--live;std::free(p);}}
void operator delete[](void* p) noexcept {::operator delete(p);}
void operator delete(void* p,size_t) noexcept {::operator delete(p);}
void operator delete[](void* p,size_t) noexcept {::operator delete(p);}
namespace vpam {void* testNodeAllocate(size_t n){return ::operator new(n);}void testNodeRelease(void* p){::operator delete(p);}}
void check(bool ok,const char* message){++checks;if(!ok){std::printf("FAIL %s\n",message);std::exit(1);}}
bool parents(const MathNode* n,const MathNode* parent=nullptr){if(!n||n->parent()!=parent)return false;for(int i=0;i<n->childCount();++i)if(!parents(n->child(i),n))return false;return true;}
bool completePreview(const MathNode* n){if(!n||n->type()==NodeType::Empty)return false;for(int i=0;i<n->childCount();++i)if(!completePreview(n->child(i)))return false;return true;}
std::string serial(const MathNode* n){std::string text,error;return numos::CalculationEngine::serializeForGiac(n,text,error)?text:"<incomplete>";}
int main(){
    unsigned visible=0;
    for(size_t i=0;i<entryCount();++i)visible+=entryAt(i)->recipe!=Recipe::Unit && entryAt(i)->recipe!=Recipe::QuantityReference && discoverable(*entryAt(i));
    check(visible==140,"140 discoverable identities; keyboard duplicates retained by ID");
    const auto& rootProvider=catalogProvider();
    check(rootProvider.count(rootProvider.context,0)==10,"three featured functions, six categories and unit provider");
    for(unsigned i=0;i<3;++i)check(rootProvider.at(rootProvider.context,0,i).entry->identity==Identity{uint16_t(i==0?103:i==1?111:120),0},"featured root order");
    check(rootProvider.count(nullptr,18)==3,"three alphabet submenus");
    check(find({410,0})->options==101 && rootProvider.count(nullptr,101)==3 && rootProvider.initial(nullptr,101)==0,"infinity opens three explicit signs");
    const char* infinityLabels[]={u8"∞",u8"+∞",u8"-∞",u8"±∞"};
    const char* infinitySerials[]={"infinity","infinity","(-infinity)","[-infinity,infinity]"};
    for(uint8_t variant=0;variant<4;++variant) {
        const auto prepared=prepare(*find({410,variant}));
        check(std::strcmp(static_cast<NodeSpecialValue*>(prepared.node.get())->label(),infinityLabels[variant])==0,"infinity sign is visible exactly as selected");
        check(serial(prepared.node.get())==infinitySerials[variant],"infinity signs have explicit CAS semantics");
        const auto cloned=cloneNode(prepared.node.get());
        check(serial(cloned.get())==infinitySerials[variant],"recall preserves infinity kind");
        if(variant) {
            const auto option=rootProvider.at(nullptr,101,variant-1);
            check(option.entry->identity==Identity{410,variant} && !option.children,"sign submenu has terminal variants");
        }
    }
    check(serial(makeSpecialValue(SpecialValueKind::UnsignedInfinity).get())=="<incomplete>","bare positive infinity does not admit unsigned complex infinity");
    for(size_t point=1;point<=14;++point) {
        const auto before=live;
        {
            auto row=makeRow();CursorController cursor;cursor.init(static_cast<NodeRow*>(row.get()));
            auto infinity=prepare(*find({410,2}));check(cursor.insertPrepared(std::move(infinity.node),nullptr),"insert negative infinity before power");
            const auto previous=cursor.cursor();attempts=failed=0;failAt=point;armed=true;
            cursor.insertPower();armed=false;
            if(failed)check(serial(row.get())=="(-infinity)" && cursor.cursor().row==previous.row && cursor.cursor().index==previous.index,"failed signed power leaves original infinity and cursor");
            else {
                auto* power=static_cast<NodePower*>(row->child(0));
                check(power->type()==NodeType::Power && power->base()->child(0)->type()==NodeType::Paren,"signed power has real grouping delimiters");
                cursor.insertDigit('2');check(serial(row.get())=="((((-infinity)))^(2))","signed power serializes the visible grouping");
            }
            check(parents(row.get()),"signed power parent links");
        }
        check(live==before,"signed power failure releases all temporaries");
    }
    for(auto group:{19,20,21})check(rootProvider.count(nullptr,group)==size_t(group==19?26:group==20?24:9),"complete alphabet groups");
    for(auto group:{19,20})for(size_t i=0;i<rootProvider.count(nullptr,group);++i) {
        const auto row=rootProvider.at(nullptr,group,i);
        check(row.entry && row.entry->identity.variant==0 && row.children,"lowercase is the primary action");
        check(rootProvider.count(nullptr,row.children)==2 && rootProvider.initial(nullptr,row.children)==1,"right opens uppercase selection");
        check(rootProvider.at(nullptr,row.children,1).entry->identity==Identity{row.entry->identity.id,1},"uppercase keeps letter identity");
    }
    for(const auto& spelling:numos::inputsymbol::kReserved) {
        const auto node=spelling.display.size()==1?makeVariable(spelling.display.front()):makeSymbol(std::string(spelling.display));
        check(serial(node.get())==spelling.cas,"free letters remain distinct from Giac constants");
        check(numos::inputsymbol::fromCas(spelling.cas)==spelling.display,"reserved spelling round trip");
    }
    for(const char* invalid:{"read(1)","alpha",u8"α+β",u8"☃","\xCE","\xCE\xFF"})
        check(serial(makeSymbol(invalid).get())=="<incomplete>","authored symbols use a strict UTF-8 Greek allowlist");
    {
        auto row=makeRow();CursorController cursor;cursor.init(static_cast<NodeRow*>(row.get()));
        auto greek=prepare(*find({300,0}));check(cursor.insertPrepared(std::move(greek.node),greek.slot),"Greek insertion");
        cursor.insertPower();cursor.insertDigit('2');check(serial(row.get())==u8"((α)^(2))","power captures a Greek atom");
    }
    unsigned injectionPoints=0;
    for(size_t i=0;i<entryCount();++i){
        const auto& e=*entryAt(i);check(find(e.identity)==&e,"stable identity");
        check(e.en[0]&&e.es[0]&&e.helpEn[0]&&e.helpEs[0],"bilingual completeness");
        check(searchRank(e,e.en)==0&&searchRank(e,e.es)==0,"exact bilingual search");
        for(size_t j=i+1;j<entryCount();++j)check(e.identity!=entryAt(j)->identity,"unique identity");
        auto p=prepare(e);check(p.node!=nullptr&&parents(p.node.get()),"constructor parents");
        auto illustration=preview(e);
        check(illustration && parents(illustration.get()),"preview has independent valid parent links");
        check(completePreview(illustration.get()),"preview has named arguments, no empty slots");
        FontMetrics fm{9,14,4,14,4,12};
        fm.emSize=ui::mathPrimaryFont()->line_height; // Synthetic checks bind to a real font; visual checks use MathCanvas metrics.
        armed=true;attempts=failed=0;failAt=1;
        p.node->calculateLayout(fm);
        armed=false;check(failed==0,"zero heap preview layout");
        check(p.node->layout().height()>0&&p.node->layout().width>0,"visible placeholder preview");
        auto root=makeRow();CursorController cc;cc.init(static_cast<NodeRow*>(root.get()));cc.insertDigit('7');
        check(cc.insertPrepared(std::move(p.node),p.slot,p.index),"publish prepared template");check(parents(root.get()),"published parents");
        check(root->child(0)->type()==NodeType::Number,"preceding operand retained");
        if(e.recipe!=Recipe::Constant&&e.recipe!=Recipe::Variable&&e.recipe!=Recipe::Symbol&&e.recipe!=Recipe::Infinity)check(serial(root.get())=="<incomplete>","pending slots reject evaluation");
        for(size_t point=1;point<=24;++point){
            const size_t before=live;
            {
                auto row=makeRow();CursorController c;c.init(static_cast<NodeRow*>(row.get()));c.insertDigit('7');
                const auto cursor=c.cursor();const auto epoch=c.epoch();
                attempts=failed=0;failAt=point;armed=true;bool committed=false;
                try{auto prepared=prepare(e);committed=c.insertPrepared(std::move(prepared.node),prepared.slot,prepared.index);}catch(const std::bad_alloc&){}
                armed=false;
                if(failed){++injectionPoints;check(!committed&&serial(row.get())=="7"&&c.cursor().row==cursor.row&&c.cursor().index==cursor.index&&c.epoch()==epoch,"persistent failure rollback");}
                check(parents(row.get()),"fault parents");
            }
            check(live==before,"failed construction releases all temporary nodes");
        }
    }
    check(!find({101,9})&&!find({65535,0})&&!find({0xc001,0}),"unknown variant and conflicting namespaces rejected");
    check(searchRank(*find({111,0}),"raiz")<4&&searchRank(*find({111,0}),"raíz")<4,"accent search");
    check(!available(*find({150,0}),Grapher),"capability rejection");
    check(searchRank(*find({140,0}),"hiperbolica")<4,"alias search");
    auto p=prepare(*find({163,0}));auto r=makeRow();CursorController c;c.init(static_cast<NodeRow*>(r.get()));
    c.insertPrepared(std::move(p.node),p.slot);c.insertDigit('1');c.insertDigit('2');c.moveRight();c.insertDigit('8');
    check(serial(r.get())=="gcd((12),(8))","argument order");c.moveRight();c.moveLeft();check(c.cursor().row->parent()->type()==NodeType::Call,"reenter call argument");
    auto unsafe=makeCall("read");static_cast<NodeCall*>(unsafe.get())->appendArgument(makeNumber("1"));check(serial(unsafe.get())=="<incomplete>","arbitrary calls rejected");
    const auto& provider=catalogProvider();check(provider.initial(provider.context,100)==1,"provider initial selection");
    check(provider.at(provider.context,11,3).entry!=nullptr,"lazy provider");
    const auto* log=find({120,0});check(log&&log->options==100,"primary action and children");
    const auto base=std::filesystem::path("out/toolbox-01/host-store");std::filesystem::create_directories(base);
    // Only named test records inside the test root are removed, never real data.
    std::filesystem::remove(base/"toolbox-favorites-a.dat");std::filesystem::remove(base/"toolbox-favorites-b.dat");
    LittleFS.setRoot(base.generic_string().c_str());LittleFS.begin(false);
    Store store;store.load();check(store.status()==StoreStatus::Missing,"missing record");
    for(size_t i=0;i<24;++i)check(store.toggle(entries()[i].identity)==FavoriteResult::Added,"add favorite");
    check(store.toggle(entries()[24].identity)==FavoriteResult::Full&&store.favoriteCount()==24,"no silent eviction");
    auto id=store.favorite(1);check(store.move(id,-1)&&store.favorite(0)==id,"explicit ordering");
    check(!store.move(id,INT32_MAX)&&!store.move(id,0),"only one-position favorite moves are accepted");
    check(store.save()&&!store.dirty(),"verified write");Store restored;restored.load();check(restored.favorite(0)==id&&restored.favoriteCount()==24,"firmware codec/native reload");
    check(restored.toggle(id)==FavoriteResult::Removed&&!restored.contains(id),"remove favorite");check(restored.save(),"second journal slot");
    for(size_t i=0;i<24;++i)store.inserted(entries()[i].identity);
    store.inserted(entries()[22].identity);check(store.recentCount()==12&&store.recent(0)==entries()[22].identity,"bounded distinct MRU");
    auto file=LittleFS.open("/toolbox-favorites-b.dat","r");uint8_t bytes[112]{};size_t n=file.read(bytes,sizeof(bytes));file.close();
    std::array<Identity,24> ids{};size_t count=0;uint32_t generation=0;
    check(Store::decode(bytes,n-1,ids,count,generation)==StoreStatus::Damaged,"truncated record");bytes[n-1]^=1;
    check(Store::decode(bytes,n,ids,count,generation)==StoreStatus::Damaged,"CRC damaged");bytes[4]=2;
    check(Store::decode(bytes,n,ids,count,generation)==StoreStatus::Future,"future schema protected");
    {
        // Deliberately valid integrity over unknown and duplicate identities.
        // A loader must validate semantics as well as bytes, never use indices.
        uint8_t record[28]={'N','T','B','X',1,0,3,0,1,0,0,0,101,0,0,0,101,0,0,0,255,255,0,0};
        uint32_t crc=0xffffffffu;
        for(unsigned i=0;i<24;++i){crc^=record[i];for(unsigned b=0;b<8;++b)crc=(crc>>1)^(0xedb88320u&uint32_t(-int(crc&1)));}
        crc=~crc;for(unsigned i=0;i<4;++i)record[24+i]=uint8_t(crc>>(8*i));
        check(Store::decode(record,sizeof(record),ids,count,generation)==StoreStatus::Ready&&count==1&&ids[0]==Identity{101,0},"unknown and duplicate identities filtered, not remapped");
        record[4]=0;check(Store::decode(record,sizeof(record),ids,count,generation)==StoreStatus::Damaged,"obsolete schema rejected");
    }
    check(searchRank(*find({120,0}),"log")==0&&searchRank(*find({120,1}),"log")==0&&find({120,0})<find({120,1}),"ambiguous aliases retain deterministic catalogue order");
    check(searchRank(*find({120,1}),"log10")==0&&searchRank(*find({120,0}),"log10")>0,"specific alias selects its variant");
    auto future=LittleFS.open("/toolbox-favorites-b.dat","w");future.write(bytes,n);future.close();Store protectedStore;protectedStore.load();protectedStore.toggle({101,0});check(!protectedStore.save(),"future record never overwritten");
    LittleFS.setRoot("out/toolbox-01/no-such-parent/storage");store.toggle(entries()[0].identity);check(!store.save()&&store.dirty(),"unavailable filesystem retains session");
    // Private mathematical provider, deliberately absent from product roots.
    // More rows than the viewport prove lazy access and an initial middle row.
    static Entry variants[9];
    for(unsigned i=0;i<9;++i){variants[i]=*find({102,0});variants[i].identity={900,uint16_t(i)};variants[i].category=99;}
    variants[0].aliases="m";variants[1].aliases="M";variants[0].caseSensitiveAliases=variants[1].caseSensitiveAliases=true;
    check(searchRank(variants[0],"m")==0&&searchRank(variants[1],"m")>0&&searchRank(variants[1],"M")==0,"provider symbols preserve case without changing identity");
    static const Provider fixture{
        nullptr,
        [](const void*,uint16_t group)->size_t{return !group?1:group==99?9:0;},
        [](const void*,uint16_t group,size_t i)->Row{if(!group&&i==0)return {nullptr,99,"Test variants","Variantes de prueba"};if(group==99&&i<9)return {&variants[i],0,variants[i].en,variants[i].es};return {};},
        [](const void*,uint16_t)->size_t{return 4;},
        [](const void*)->size_t{return 9;},
        [](const void*,size_t i)->const Entry*{return i<9?&variants[i]:nullptr;},
        [](const Entry&)->Prepared{auto node=makePower();auto* slot=static_cast<NodeRow*>(node->child(0));return {std::move(node),slot,0};}
    };
    check(registerProvider(fixture),"register typed lazy provider");
    check(!registerProvider(fixture),"provider duplicate identities rejected");
    check(catalogProvider().initial(nullptr,99)==4&&catalogProvider().count(nullptr,99)==9,"provider initial middle selection");
    check(find({900,8})==&variants[8]&&entryAt(entryCount()-1)==&variants[8],"variant is searchable without materializing tree");
    auto extension=prepare(*find({900,8}));check(extension.slot&&extension.node->type()==NodeType::Power,"provider owns typed construction");
    Store ordered;ordered.toggle({900,8});std::swap(variants[0],variants[8]);
    check(ordered.favorite(0)==Identity{900,8}&&find(ordered.favorite(0))==&variants[0],"catalogue reorder cannot retarget a favorite");
    std::printf("PASS toolbox: %u checks, %u persistent allocation faults, %zu entries. Store=%zu Identity=%zu\n",checks,injectionPoints,entryCount(),sizeof(Store),sizeof(Identity));
}
