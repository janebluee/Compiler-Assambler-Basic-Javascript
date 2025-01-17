#pragma once
#include "ast.hpp"
#include <memory>
#include <unordered_map>

namespace js {

class Optimizer {
private:
    std::unordered_map<std::string, NodePtr> functionMap;

public:
    Optimizer() = default;
    
    NodePtr optimizeExpression(NodePtr node);
    NodePtr optimizeStatement(NodePtr node);
    NodePtr optimizeDeclaration(NodePtr node);
    
    void constantFolding(NodePtr& node);
    void deadCodeElimination(NodePtr& node);
    void inlineSimpleFunctions(NodePtr& node);
    void optimizeUnary(NodePtr& node);
    bool isTruthy(const Literal* lit);
    std::string toString(const Literal* lit);
};

} 
