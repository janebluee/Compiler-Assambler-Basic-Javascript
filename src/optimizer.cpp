#include "../include/optimizer.hpp"
#include <stdexcept>
#include <cmath>
#include <iostream>

namespace js {

NodePtr Optimizer::optimizeExpression(NodePtr node) {
    if (!node) return nullptr;
    
    if (auto* unary = dynamic_cast<UnaryExpression*>(node.get())) {
        unary->argument = optimizeExpression(std::move(unary->argument));
        optimizeUnary(node);
    }
    else if (auto* binary = dynamic_cast<BinaryExpression*>(node.get())) {
        binary->left = optimizeExpression(std::move(binary->left));
        binary->right = optimizeExpression(std::move(binary->right));
        constantFolding(node);
        deadCodeElimination(node);
    }
    else if (auto* call = dynamic_cast<CallExpression*>(node.get())) {
        // Handle console.log
        if (auto* member = dynamic_cast<MemberExpression*>(call->callee.get())) {
            if (auto* obj = dynamic_cast<Identifier*>(member->object.get())) {
                if (obj->name == "console" && member->property == "log") {
                    // Evaluate and print arguments
                    for (size_t i = 0; i < call->arguments.size(); i++) {
                        auto arg = optimizeExpression(std::move(call->arguments[i]));
                        if (auto* lit = dynamic_cast<Literal*>(arg.get())) {
                            if (i > 0) std::cout << " ";
                            if (std::holds_alternative<std::string>(lit->value)) {
                                std::cout << std::get<std::string>(lit->value);
                            } else if (std::holds_alternative<double>(lit->value)) {
                                std::cout << std::get<double>(lit->value);
                            } else if (std::holds_alternative<bool>(lit->value)) {
                                std::cout << (std::get<bool>(lit->value) ? "true" : "false");
                            }
                        }
                    }
                    std::cout << std::endl;
                    return node;
                }
            }
        }
        
        // Handle other function calls
        call->callee = optimizeExpression(std::move(call->callee));
        for (auto& arg : call->arguments) {
            arg = optimizeExpression(std::move(arg));
        }
    }
    
    return std::move(node);
}

NodePtr Optimizer::optimizeStatement(NodePtr node) {
    if (!node) return nullptr;
    
    if (auto* ret = dynamic_cast<ReturnStatement*>(node.get())) {
        ret->argument = optimizeExpression(std::move(ret->argument));
    }
    
    return std::move(node);
}

NodePtr Optimizer::optimizeDeclaration(NodePtr node) {
    if (!node) return nullptr;
    
    if (auto* varDecl = dynamic_cast<VariableDeclaration*>(node.get())) {
        varDecl->init = optimizeExpression(std::move(varDecl->init));
    }
    else if (auto* funcDecl = dynamic_cast<FunctionDeclaration*>(node.get())) {
        auto newFuncDecl = std::make_unique<FunctionDeclaration>();
        newFuncDecl->name = funcDecl->name;
        newFuncDecl->params = funcDecl->params;
        
        for (auto& stmt : funcDecl->body) {
            newFuncDecl->body.push_back(optimizeStatement(std::move(stmt)));
        }
        
        functionMap[funcDecl->name] = std::move(newFuncDecl);
    }
    
    return std::move(node);
}

void Optimizer::optimizeUnary(NodePtr& node) {
    if (auto* unary = dynamic_cast<UnaryExpression*>(node.get())) {
        if (auto* lit = dynamic_cast<Literal*>(unary->argument.get())) {
            try {
                auto result = std::make_unique<Literal>();
                
                if (unary->op == "!") {
                    result->value = !isTruthy(lit);
                }
                else if (unary->op == "-") {
                    if (std::holds_alternative<double>(lit->value)) {
                        result->value = -std::get<double>(lit->value);
                    }
                }
                else if (unary->op == "+") {
                    if (std::holds_alternative<double>(lit->value)) {
                        result->value = std::get<double>(lit->value);
                    }
                    else if (std::holds_alternative<std::string>(lit->value)) {
                        try {
                            result->value = std::stod(std::get<std::string>(lit->value));
                        } catch (...) {
                            result->value = std::numeric_limits<double>::quiet_NaN();
                        }
                    }
                }
                
                node = std::move(result);
            } catch (const std::bad_variant_access&) {}
        }
        else if (auto* nestedUnary = dynamic_cast<UnaryExpression*>(unary->argument.get())) {
            if (unary->op == "!" && nestedUnary->op == "!") {
                node = std::move(nestedUnary->argument);
            }
            else if (unary->op == "-" && nestedUnary->op == "-") {
                node = std::move(nestedUnary->argument);
            }
        }
    }
}

void Optimizer::constantFolding(NodePtr& node) {
    if (auto* binary = dynamic_cast<BinaryExpression*>(node.get())) {
        auto* leftLit = dynamic_cast<Literal*>(binary->left.get());
        auto* rightLit = dynamic_cast<Literal*>(binary->right.get());
        
        if (leftLit && rightLit) {
            try {
                auto result = std::make_unique<Literal>();
                
                if (std::holds_alternative<double>(leftLit->value) && 
                    std::holds_alternative<double>(rightLit->value)) {
                    auto leftNum = std::get<double>(leftLit->value);
                    auto rightNum = std::get<double>(rightLit->value);
                    
                    if (binary->op == "+") {
                        result->value = leftNum + rightNum;
                    }
                    else if (binary->op == "-") {
                        result->value = leftNum - rightNum;
                    }
                    else if (binary->op == "*") {
                        result->value = leftNum * rightNum;
                    }
                    else if (binary->op == "/") {
                        if (rightNum == 0) {
                            throw std::runtime_error("Division by zero");
                        }
                        result->value = leftNum / rightNum;
                    }
                    else if (binary->op == "**") {
                        result->value = std::pow(leftNum, rightNum);
                    }
                    else if (binary->op == "<") {
                        result->value = leftNum < rightNum;
                    }
                    else if (binary->op == ">") {
                        result->value = leftNum > rightNum;
                    }
                    else if (binary->op == "<=") {
                        result->value = leftNum <= rightNum;
                    }
                    else if (binary->op == ">=") {
                        result->value = leftNum >= rightNum;
                    }
                    else if (binary->op == "==") {
                        result->value = leftNum == rightNum;
                    }
                    else if (binary->op == "!=") {
                        result->value = leftNum != rightNum;
                    }
                }
                else if (binary->op == "&&") {
                    result->value = isTruthy(leftLit) && isTruthy(rightLit);
                }
                else if (binary->op == "||") {
                    result->value = isTruthy(leftLit) || isTruthy(rightLit);
                }
                else if (binary->op == "+") {
                    if (std::holds_alternative<std::string>(leftLit->value) || 
                        std::holds_alternative<std::string>(rightLit->value)) {
                        result->value = toString(leftLit) + toString(rightLit);
                    }
                }
                
                node = std::move(result);
            } catch (const std::bad_variant_access&) {}
        }
    }
}

bool Optimizer::isTruthy(const Literal* lit) {
    if (std::holds_alternative<bool>(lit->value)) {
        return std::get<bool>(lit->value);
    }
    else if (std::holds_alternative<double>(lit->value)) {
        return std::get<double>(lit->value) != 0;
    }
    else if (std::holds_alternative<std::string>(lit->value)) {
        return !std::get<std::string>(lit->value).empty();
    }
    return false;
}

std::string Optimizer::toString(const Literal* lit) {
    if (std::holds_alternative<std::string>(lit->value)) {
        return std::get<std::string>(lit->value);
    }
    else if (std::holds_alternative<double>(lit->value)) {
        return std::to_string(std::get<double>(lit->value));
    }
    else if (std::holds_alternative<bool>(lit->value)) {
        return std::get<bool>(lit->value) ? "true" : "false";
    }
    return "undefined";
}

void Optimizer::deadCodeElimination(NodePtr& node) {
    if (auto* binary = dynamic_cast<BinaryExpression*>(node.get())) {
        auto* leftLit = dynamic_cast<Literal*>(binary->left.get());
        auto* rightLit = dynamic_cast<Literal*>(binary->right.get());
        
        if (binary->op == "*") {
            bool isZero = false;
            if (leftLit) {
                try {
                    auto leftNum = std::get<double>(leftLit->value);
                    if (leftNum == 0) isZero = true;
                    else if (leftNum == 1) {
                        node = std::move(binary->right);
                        return;
                    }
                } catch (const std::bad_variant_access&) {}
            }
            if (rightLit) {
                try {
                    auto rightNum = std::get<double>(rightLit->value);
                    if (rightNum == 0) isZero = true;
                    else if (rightNum == 1) {
                        node = std::move(binary->left);
                        return;
                    }
                } catch (const std::bad_variant_access&) {}
            }
            
            if (isZero) {
                auto result = std::make_unique<Literal>();
                result->value = 0.0;
                node = std::move(result);
                return;
            }
        }
        
        if (binary->op == "/") {
            if (rightLit) {
                try {
                    auto rightNum = std::get<double>(rightLit->value);
                    if (rightNum == 1) {
                        node = std::move(binary->left);
                        return;
                    }
                } catch (const std::bad_variant_access&) {}
            }
        }
        
        if (binary->op == "**") {
            if (rightLit) {
                try {
                    auto rightNum = std::get<double>(rightLit->value);
                    if (rightNum == 0) {
                        auto result = std::make_unique<Literal>();
                        result->value = 1.0;
                        node = std::move(result);
                        return;
                    }
                    else if (rightNum == 1) {
                        node = std::move(binary->left);
                        return;
                    }
                } catch (const std::bad_variant_access&) {}
            }
            if (leftLit) {
                try {
                    auto leftNum = std::get<double>(leftLit->value);
                    if (leftNum == 1) {
                        auto result = std::make_unique<Literal>();
                        result->value = 1.0;
                        node = std::move(result);
                        return;
                    }
                } catch (const std::bad_variant_access&) {}
            }
        }
        
        if (binary->op == "+" || binary->op == "-") {
            if (leftLit) {
                try {
                    auto leftNum = std::get<double>(leftLit->value);
                    if (leftNum == 0 && binary->op == "+") {
                        node = std::move(binary->right);
                        return;
                    }
                } catch (const std::bad_variant_access&) {}
            }
            if (rightLit) {
                try {
                    auto rightNum = std::get<double>(rightLit->value);
                    if (rightNum == 0) {
                        node = std::move(binary->left);
                        return;
                    }
                } catch (const std::bad_variant_access&) {}
            }
        }
        
        if (binary->op == "&&") {
            if (leftLit && !isTruthy(leftLit)) {
                auto result = std::make_unique<Literal>();
                result->value = false;
                node = std::move(result);
                return;
            }
            if (rightLit && !isTruthy(rightLit)) {
                auto result = std::make_unique<Literal>();
                result->value = false;
                node = std::move(result);
                return;
            }
        }
        
        if (binary->op == "||") {
            if (leftLit && isTruthy(leftLit)) {
                auto result = std::make_unique<Literal>();
                result->value = true;
                node = std::move(result);
                return;
            }
            if (rightLit && isTruthy(rightLit)) {
                auto result = std::make_unique<Literal>();
                result->value = true;
                node = std::move(result);
                return;
            }
        }
    }
}

void Optimizer::inlineSimpleFunctions(NodePtr& node) {
    if (auto* call = dynamic_cast<CallExpression*>(node.get())) {
        auto* callee = dynamic_cast<Identifier*>(call->callee.get());
        if (!callee) return;
        
        auto it = functionMap.find(callee->name);
        if (it == functionMap.end()) return;
        
        auto* funcDecl = dynamic_cast<FunctionDeclaration*>(it->second.get());
        if (!funcDecl) return;
        
        if (funcDecl->body.size() != 1) return;
        
        auto* ret = dynamic_cast<ReturnStatement*>(funcDecl->body[0].get());
        if (!ret) return;
        
        if (auto* literal = dynamic_cast<Literal*>(ret->argument.get())) {
            auto result = std::make_unique<Literal>();
            result->value = literal->value;
            node = std::move(result);
        }
    }
}

} 
