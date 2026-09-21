// SPDX-License-Identifier: GPL-3.0-or-later
#include "math/giac/GiacEngine.h"
#include "math/MathEvaluator.h"
#include <iostream>
#include <chrono>
bool setting_complex_enabled=false;
std::string quote(const std::string& s){std::string r="\"";for(char c:s){if(c=='\n')r+="\\n";else if(c=='\r')r+="\\r";else{if(c=='\"'||c=='\\')r+='\\';r+=c;}}return r+'\"';}
void tree(const numos::EngineResultNode& n){std::cout<<"{\"kind\":"<<unsigned(n.kind)<<",\"text\":"<<quote(n.text)<<",\"children\":[";for(size_t i=0;i<n.children.size();++i){if(i)std::cout<<',';tree(n.children[i]);}std::cout<<"]}";}
int main(int argc,char** argv){using namespace numos;
    auto& e=GiacEngine::instance();if(!e.begin()||argc<2)return 2;
    std::string input=argv[1];auto at=input.find('=');if(at==std::string::npos)return 2;
    bool tutor=true,complex=false,benchmark=false;
    for(int i=2;i<argc;++i){std::string arg=argv[i];if(arg=="--degrees")vpam::g_angleMode=vpam::AngleMode::DEG;else if(arg=="--no-tutor")tutor=false;else if(arg=="--complex")complex=true;else if(arg=="--benchmark")benchmark=true;else e.evaluate(arg.c_str());}
    SolveEquation eq{input.substr(0,at),input.substr(at+1)};
    auto start=std::chrono::steady_clock::now();const auto answer=e.solveStructured(eq,"x",complex?SolveDomainPolicy::RealAndComplex:SolveDomainPolicy::RealOnly);
    auto micros=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-start).count();
    std::cout<<"{\"status\":"<<unsigned(answer.status)<<",\"kind\":"<<unsigned(answer.setKind)<<",\"scope\":"<<unsigned(answer.coverage)<<",\"origin\":"<<unsigned(answer.origin)
        <<",\"degrees\":"<<answer.degrees<<",\"binderScope\":"<<answer.binderScope<<",\"raw\":"<<quote(answer.rawExactText)
        <<",\"calls\":"<<answer.periodicCalls<<",\"payload\":"<<answer.periodicPayload<<",\"micros\":"<<micros<<",\"families\":[";
    for(size_t i=0;i<answer.families.size();++i){if(i)std::cout<<',';const auto& f=answer.families[i];
        std::cout<<"{\"variable\":"<<quote(f.variable)<<",\"offset\":"<<quote(f.offset)<<",\"period\":"<<quote(f.period)<<",\"binderScope\":"<<f.binderScope<<",\"binderId\":"<<f.binderId<<",\"domain\":"<<unsigned(f.domain)<<",\"offsetTree\":";
        tree(f.offsetValue);std::cout<<",\"periodTree\":";tree(f.periodValue);std::cout<<'}';}
    std::cout<<"],\"restrictions\":[";for(size_t i=0;i<answer.restrictions.size();++i){if(i)std::cout<<',';const auto& c=answer.restrictions[i];std::cout<<"{\"nonzero\":"<<quote(c.exactText)<<",\"side\":"<<unsigned(c.side)<<",\"tree\":";tree(c.expression);std::cout<<'}';}
    std::cout<<"],\"tutor\":";
    if(tutor){numos::tutor::Snapshot s;s.authored={{eq.lhs,eq.rhs}};s.variables={"x"};s.inputEpoch=1;s.degrees=vpam::g_angleMode==vpam::AngleMode::DEG;s.complex=complex;
        auto d=e.explainEquations(s,answer);if(d.status==numos::tutor::Status::Complete&&e.verifyDerivation(d,d.input)!=numos::tutor::Verdict::Verified)return 3;
        std::cout<<numos::tutor::replayJson(d);
    }else std::cout<<"null";
    std::cout<<",\"warmSolveMicros\":[";
    if(benchmark){
        // One cold solve above, five warm-ups, then thirty retained samples.
        // No Giac reset or logging inside the measured solve/publication scope.
        std::vector<long long> samples;samples.reserve(30);
        for(int i=0;i<35;++i){
            const auto begin=std::chrono::steady_clock::now();
            auto repeated=e.solveStructured(eq,"x",complex?SolveDomainPolicy::RealAndComplex:SolveDomainPolicy::RealOnly);
            const auto elapsed=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-begin).count();
            if(repeated.coverage!=answer.coverage||repeated.rawExactText!=answer.rawExactText)return 4;
            if(i>=5)samples.push_back(elapsed);
        }
        for(size_t i=0;i<samples.size();++i){if(i)std::cout<<',';std::cout<<samples[i];}
    }
    std::cout<<"]}\n";return 0;
}
