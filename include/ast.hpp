#pragma once
#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <iostream>

namespace js {

enum class NodeType {
    PROGRAM,
    VARIABLE_DECLARATION,
    FUNCTION_DECLARATION,
    RETURN_STATEMENT,
    BINARY_EXPRESSION,
    CALL_EXPRESSION,
    IDENTIFIER,
    LITERAL,
    UNARY_EXPRESSION,
    MEMBER_EXPRESSION
};

class ASTNode {
public:
    NodeType type;
    explicit ASTNode(NodeType t) : type(t) {}
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const {
        std::string indentation(indent * 2, ' ');
        std::cout << indentation << "ASTNode" << std::endl;
    }
    
    void* operator new(size_t size);
    void operator delete(void* ptr) noexcept;
};

using NodePtr = std::unique_ptr<ASTNode>;

class Expression : public ASTNode {
public:
    explicit Expression(NodeType t) : ASTNode(t) {}
};

class Literal : public Expression {
public:
    std::variant<double, std::string, bool> value;
    Literal() : Expression(NodeType::LITERAL) {}
    void print(int indent = 0) const override {
        std::string indentation(indent * 2, ' ');
        std::cout << indentation << "Literal: ";
        if (std::holds_alternative<double>(value)) {
            std::cout << std::get<double>(value);
        } else if (std::holds_alternative<std::string>(value)) {
            std::cout << std::get<std::string>(value);
        } else if (std::holds_alternative<bool>(value)) {
            std::cout << std::get<bool>(value);
        }
        std::cout << std::endl;
    }
};

class Identifier : public Expression {
public:
    std::string name;
    Identifier() : Expression(NodeType::IDENTIFIER) {}
    void print(int indent = 0) const override {
        std::string indentation(indent * 2, ' ');
        std::cout << indentation << "Identifier: " << name << std::endl;
    }
};

class UnaryExpression : public Expression {
public:
    std::string op;
    NodePtr argument;
    UnaryExpression() : Expression(NodeType::UNARY_EXPRESSION) {}
    void print(int indent = 0) const override {
        std::string indentation(indent * 2, ' ');
        std::cout << indentation << "UnaryExpression: " << op << std::endl;
        if (argument) argument->print(indent + 1);
    }
};

class BinaryExpression : public Expression {
public:
    NodePtr left;
    NodePtr right;
    std::string op;
    BinaryExpression() : Expression(NodeType::BINARY_EXPRESSION) {}
    void print(int indent = 0) const override {
        std::string indentation(indent * 2, ' ');
        std::cout << indentation << "BinaryExpression: " << op << std::endl;
        if (left) left->print(indent + 1);
        if (right) right->print(indent + 1);
    }
};

class CallExpression : public Expression {
public:
    NodePtr callee;
    std::vector<NodePtr> arguments;
    CallExpression() : Expression(NodeType::CALL_EXPRESSION) {}
    void print(int indent = 0) const override {
        std::string indentation(indent * 2, ' ');
        std::cout << indentation << "CallExpression" << std::endl;
        if (callee) callee->print(indent + 1);
        for (const auto& arg : arguments) {
            arg->print(indent + 1);
        }
    }
};

class MemberExpression : public Expression {
public:
    NodePtr object;
    std::string property;
    MemberExpression() : Expression(NodeType::MEMBER_EXPRESSION) {}
    void print(int indent = 0) const override {
        std::string indentation(indent * 2, ' ');
        std::cout << indentation << "MemberExpression: " << property << std::endl;
        if (object) object->print(indent + 1);
    }
};

class Statement : public ASTNode {
public:
    explicit Statement(NodeType t) : ASTNode(t) {}
};

class ReturnStatement : public Statement {
public:
    NodePtr argument;
    ReturnStatement() : Statement(NodeType::RETURN_STATEMENT) {}
    void print(int indent = 0) const override {
        std::string indentation(indent * 2, ' ');
        std::cout << indentation << "ReturnStatement" << std::endl;
        if (argument) argument->print(indent + 1);
    }
};

class Declaration : public Statement {
public:
    explicit Declaration(NodeType t) : Statement(t) {}
};

class VariableDeclaration : public Declaration {
public:
    std::string name;
    NodePtr init;
    VariableDeclaration() : Declaration(NodeType::VARIABLE_DECLARATION) {}
    void print(int indent = 0) const override {
        std::string indentation(indent * 2, ' ');
        std::cout << indentation << "VariableDeclaration: " << name << std::endl;
        if (init) init->print(indent + 1);
    }
};

class FunctionDeclaration : public Declaration {
public:
    std::string name;
    std::vector<std::string> params;
    std::vector<NodePtr> body;
    FunctionDeclaration() : Declaration(NodeType::FUNCTION_DECLARATION) {}
    void print(int indent = 0) const override {
        std::string indentation(indent * 2, ' ');
        std::cout << indentation << "FunctionDeclaration: " << name << std::endl;
        for (const auto& param : params) {
            std::cout << indentation << "  " << param << std::endl;
        }
        for (const auto& stmt : body) {
            stmt->print(indent + 1);
        }
    }
};

} // namespace js
