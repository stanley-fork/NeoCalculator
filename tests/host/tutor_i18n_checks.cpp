// SPDX-License-Identifier: GPL-3.0-or-later
#include "math/tutor/Derivation.h"
#include "ui/TutorFonts.h"
#include "demo/DemoSettingsRecord.h"
#include <cstring>
#include <iostream>
bool setting_complex_enabled=false;
int main() {
    using namespace numos::tutor;unsigned checks=0,failed=0;
    auto check=[&](bool value,const char* name){++checks;if(!value){++failed;std::cerr<<name<<'\n';}};
    check(validateCatalogs(),"catalog coverage/schema");
    for(unsigned k=0;k<unsigned(Message::Count);++k){
        const auto key=Message(k);check(hasTranslation(key,Locale::English),"missing EN");check(hasTranslation(key,Locale::Spanish),"missing ES");
        Vector<Parameter> parameters;
        for(const char* p=messageSchema(key);*p;++p){
            const auto kind=*p=='e'?ParameterKind::Expression:*p=='v'?ParameterKind::Variable:*p=='r'?ParameterKind::Row:ParameterKind::Integer;
            parameters.push_back({kind,*p=='e'?"3/2":*p=='v'?"x":"2"});
        }
        check(validateMessage(key,parameters),"typed parameters");
        const auto en=explain(key,parameters,Locale::English),es=explain(key,parameters,Locale::Spanish);
        check(!en.empty()&&!es.empty()&&es.find('{')==std::string::npos,"resolved text");
        if(!parameters.empty()){
            auto wrong=parameters;wrong[0].kind=parameters[0].kind==ParameterKind::Integer?ParameterKind::Variable:ParameterKind::Integer;
            check(!validateMessage(key,wrong),"wrong typed parameter accepted");
        } else check(es==messageFallback(key,Locale::Spanish),"OOM text silently used English");
    }
    for(const auto n:{0,1,2}) {
        auto s=n==1?explain(Message::ViewCountRealOne,{},Locale::Spanish):explain(Message::ViewCountRealMany,{{ParameterKind::Integer,std::to_string(n)}},Locale::Spanish);
        check(s.find(n==1?"1 solución":"soluciones")!=std::string::npos,"plural");
    }
    check(explain(Message::ViewResult,{},Locale::French)==explain(Message::ViewResult,{},Locale::English),"French safety fallback");
    check(storedLocale(255)==Locale::English,"unknown preference default");
    for(uint8_t version:{uint8_t(2),uint8_t(3)}) {
        auto old=numos::demo::encodeSettingsRecord(true,false,true,8,128);
        old[4]=version;
        auto checksum=numos::demo::settingsRecordChecksum(old.data(),12);
        std::memcpy(old.data()+12,&checksum,4);
        numos::demo::DecodedSettings decoded;
        check(numos::demo::decodeSettingsRecord(old.data(),old.size(),decoded),"legacy record rejected");
        check(decoded.tutorLanguage==0&&decoded.angleDeg&&!decoded.complexEnabled&&decoded.educationEnabled&&decoded.precision==8,"legacy values changed");
    }
    for(uint8_t language:{uint8_t(0),uint8_t(1),uint8_t(255)}) {
        auto bytes=numos::demo::encodeSettingsRecord(false,true,true,10,128,language);
        numos::demo::DecodedSettings decoded;
        check(numos::demo::decodeSettingsRecord(bytes.data(),bytes.size(),decoded),"settings roundtrip");
        check(decoded.tutorLanguage==(language==1?1:0),"language roundtrip");
        bytes[10]=253;auto checksum=numos::demo::settingsRecordChecksum(bytes.data(),12);std::memcpy(bytes.data()+12,&checksum,4);
        check(numos::demo::decodeSettingsRecord(bytes.data(),bytes.size(),decoded)&&decoded.tutorLanguage==0,"unknown saved language");
        bytes[0]^=1;check(!numos::demo::decodeSettingsRecord(bytes.data(),bytes.size(),decoded),"corrupt record");
    }
    lv_init();
    const uint32_t glyphs[]={0xe1,0xe9,0xed,0xf3,0xfa,0xfc,0xf1,0xc1,0xc9,0xcd,0xd3,0xda,0xdc,0xd1,0xbf,0xa1};
    const lv_font_t* fonts[]={ui::tutorFont10(),ui::tutorFont12(),ui::tutorFont14()};
    const lv_font_t* originals[]={&lv_font_montserrat_10,&lv_font_montserrat_12,&lv_font_montserrat_14};
    for(unsigned i=0;i<3;++i){
        check(fonts[i]->line_height==originals[i]->line_height&&fonts[i]->base_line==originals[i]->base_line,"changed ASCII metrics");
        for(auto cp:glyphs){lv_font_glyph_dsc_t d{};check(lv_font_get_glyph_dsc(fonts[i],&d,cp,0)&&d.box_w&&d.box_h&&!d.is_placeholder,"missing Spanish glyph");}
        for(uint32_t cp=32;cp<127;++cp){lv_font_glyph_dsc_t a{},b{};lv_font_get_glyph_dsc(fonts[i],&a,cp,'a');lv_font_get_glyph_dsc(originals[i],&b,cp,'a');check(a.adv_w==b.adv_w&&a.box_w==b.box_w&&a.box_h==b.box_h&&a.ofs_x==b.ofs_x&&a.ofs_y==b.ofs_y,"ASCII geometry changed");}
    }
    std::cout<<"i18n checks="<<checks<<" failed="<<failed<<'\n';return failed?1:0;
}
