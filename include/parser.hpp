#pragma once
#include "lexer.hpp"
#include "ast.hpp"
#include <memory>
#include <vector>

namespace js {

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;
    
    Token peek();
    Token advance();
    bool match(TokenType type);
    NodePtr parse_statement();
    NodePtr parse_expression();
    NodePtr parse_primary();
    NodePtr parse_variable_declaration();
    NodePtr parse_function_declaration();
    NodePtr parseCallExpression(NodePtr callee);
    NodePtr parseMemberExpression(NodePtr object);
    
public:
    explicit Parser(std::vector<Token> tokens);
    NodePtr parse();
};

} 
