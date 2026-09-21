// SPDX-License-Identifier: GPL-3.0-or-later
#include "math/giac/GiacEngine.h"
#include "math/MathEvaluator.h"
#include <iostream>
bool setting_complex_enabled=false;
int main(int argc,char** argv) {
    using namespace numos;using namespace numos::tutor;
    auto& engine=GiacEngine::instance();if(!engine.begin())return 2;
    Snapshot snapshot;snapshot.inputEpoch=1;snapshot.engineGeneration=engine.generation();
    std::vector<SolveEquation> equations;
    for(int i=1;i<argc;++i){std::string arg=argv[i];
        if(arg=="--degrees"){snapshot.degrees=true;vpam::g_angleMode=vpam::AngleMode::DEG;continue;}
        if(arg=="--complex"){snapshot.complex=true;continue;}
        if(arg=="--set"&&i+1<argc){engine.evaluate(argv[++i]);continue;}
        const auto eq=arg.find('=');if(eq==std::string::npos)return 2;
        equations.push_back({arg.substr(0,eq),arg.substr(eq+1)});snapshot.authored.push_back({arg.substr(0,eq),arg.substr(eq+1)});
    }
    for(size_t i=0;i<equations.size();++i)snapshot.variables.push_back(std::string(1,"xyz"[i]));
    if(equations.empty())return 2;
    const auto policy=snapshot.complex?SolveDomainPolicy::RealAndComplex:SolveDomainPolicy::RealOnly;
    const auto answer=equations.size()==1?engine.solveStructured(equations[0],"x",policy):engine.solveSystemStructured(equations,std::vector<std::string>(snapshot.variables.begin(),snapshot.variables.end()),policy);
    const auto trace=engine.explainEquations(snapshot,answer);
    if(trace.status==Status::Complete&&engine.verifyDerivation(trace,trace.input)!=Verdict::Verified)return 3;
    // One checked graph, three presentations. No solving or planning between locales.
    std::cout<<"{\"en\":"<<replayJson(trace,Locale::English)<<",\"es\":"<<replayJson(trace,Locale::Spanish)<<",\"enAgain\":"<<replayJson(trace,Locale::English)<<"}\n";
    return 0;
}
