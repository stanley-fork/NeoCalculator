#pragma once
#include "MathAST.h"

namespace numos::inputrow {
// WHY: one lexical/precedence contract for authored rows. Both the scalar
// serializer and quantity evaluator consume these events; neither parses text.
inline vpam::OpKind arithmetic(vpam::OpKind op) {
    using O=vpam::OpKind;
    return op==O::UnitProduct || op==O::UnitAttach || op==O::UnitAttachTight ? O::Mul : op;
}
inline unsigned precedence(vpam::OpKind op) {
    return op==vpam::OpKind::Add || op==vpam::OpKind::Sub ? 1u:2u;
}
template<class Sink> bool visit(const vpam::NodeRow& row,Sink& sink) {
    using namespace vpam;
    bool previous=false,any=false;
    for(const auto& child:row.children()) {
        const auto* node=child.get();if(!node)continue;
        if(node->type()==NodeType::Empty)return sink.fail("incomplete expression");
        if(node->type()==NodeType::Operator) {
            const auto op=arithmetic(static_cast<const NodeOperator*>(node)->op());
            if(op!=OpKind::Add && op!=OpKind::Sub && op!=OpKind::Mul && op!=OpKind::Div)
                return sink.fail("unsupported operator");
            if(!previous) {
                if(op!=OpKind::Add && op!=OpKind::Sub)return sink.fail("misplaced operator");
                if(!sink.unary(op))return false;
            } else {if(!sink.binary(op))return false;previous=false;}
        } else {
            if(previous && !sink.binary(OpKind::Mul))return false;
            if(!sink.operand(node))return false;
            previous=any=true;
        }
    }
    return any && previous ? true:sink.fail("incomplete expression");
}
}
