#pragma once
#include "lexer.hpp"
#include <memory>
#include <vector>

enum class NodeType {
    PROGRAM,
    VARIABLE_DECLARATION,
    FUNCTION_DECLARATION,
    IDENTIFIER,
    LITERAL,
    BINARY_EXPRESSION,
    CALL_EXPRESSION,
    BLOCK_STATEMENT,
    RETURN_STATEMENT
};

class ASTNode {
public:
    NodeType type;
    virtual ~ASTNode() = default;
    explicit ASTNode(NodeType t) : type(t) {}
};

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;
    
    Token peek();
    Token advance();
    bool match(TokenType type);
    std::unique_ptr<ASTNode> parse_statement();
    std::unique_ptr<ASTNode> parse_expression();
    std::unique_ptr<ASTNode> parse_primary();
    std::unique_ptr<ASTNode> parse_variable_declaration();
    std::unique_ptr<ASTNode> parse_function_declaration();
    
public:
    explicit Parser(std::vector<Token> tokens);
    std::unique_ptr<ASTNode> parse();
};
