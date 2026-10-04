// Reduced probe of the actually linked KhiCAS library, not an alternate CAS.
#include "math/giac/GiacEngine.h"
#include "math/AngleModeRuntime.h"
#include <cstdio>
bool setting_complex_enabled = false;
int main() {
    auto& engine=numos::GiacEngine::instance();
    if(!engine.begin())return 1;
    for(const char* text:{"2_m+30_cm","2_cm*3_cm","6_km/300_s","(2_cm)^3",
        "mksa(1_in)","mksa(1_cm)","mksa(1_eV)","12_V/3_A","usimplify(12_V/3_A)",
        "convert(20_(m/s),_km/_h)","ufactor(720000_J,_Wh)","2_m+3_s",
        "0*(2_m+3_s)","sin(90_deg)","sin((pi/2)_rad)","mksa(1_Qm)","mksa(1_mg)"}) {
        auto result=engine.evaluateStructured(text,true);
        std::printf("%s => status=%u exact=%s approx=%s tree=%u diagnostic=%s\n",text,
          unsigned(result.base.status),result.base.exactText.c_str(),result.base.approximateText.c_str(),
          result.hasTree?unsigned(result.tree.kind):255,result.base.diagnostic.c_str());
    }
    numos::setAngleMode(vpam::AngleMode::DEG);
    auto scoped=engine.evaluateStructured("sin(pi/2)",true,numos::GiacEngine::EvaluationAngle::Radians);
    auto restored=engine.evaluateStructured("sin(90)",true);
    std::printf("scoped=%s restored=%s global=%s\n",scoped.base.exactText.c_str(),restored.base.exactText.c_str(),numos::angleModeName());
    return scoped.base.exactText=="1" && restored.base.exactText=="1" && numos::angleModeIsDeg()?0:2;
}
