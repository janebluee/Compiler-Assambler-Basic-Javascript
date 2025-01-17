#include "../include/parser.hpp"
#include <stdexcept>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)), current(0) {}

Token Parser::peek() {
    if (current >= tokens.size()) {
        return Token(TokenType::EOF_TOKEN, "");
    }
    return tokens[current];
}

Token Parser::advance() {
    if (current < tokens.size()) {
        return tokens[current++];
    }
    return Token(TokenType::EOF_TOKEN, "");
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

std::unique_ptr<ASTNode> Parser::parse() {
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    while (peek().type != TokenType::EOF_TOKEN) {
        statements.push_back(parse_statement());
    }
    
    return std::make_unique<ASTNode>(NodeType::PROGRAM);
}

std::unique_ptr<ASTNode> Parser::parse_statement() {
    Token token = peek();
    
    if (token.type == TokenType::KEYWORD) {
        if (token.value == "let" || token.value == "const" || token.value == "var") {
            advance();
            return parse_variable_declaration();
        }
        if (token.value == "function") {
            advance();
            return parse_function_declaration();
        }
        if (token.value == "return") {
            advance();
            auto expr = parse_expression();
            
            if (peek().type == TokenType::OPERATOR && peek().value == ";") {
                advance();
            }
            
            return std::make_unique<ASTNode>(NodeType::RETURN_STATEMENT);
        }
    }
    
    auto expr = parse_expression();
    
    if (peek().type == TokenType::OPERATOR && peek().value == ";") {
        advance();
    }
    
    return expr;
}

std::unique_ptr<ASTNode> Parser::parse_expression() {
    auto left = parse_primary();
    
    while (peek().type == TokenType::OPERATOR) {
        std::string op = peek().value;
        if (op != "+" && op != "-" && op != "*" && op != "/") {
            break;
        }
        advance();
        
        auto right = parse_primary();
        auto binary = std::make_unique<ASTNode>(NodeType::BINARY_EXPRESSION);
        left = std::move(binary);
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parse_primary() {
    Token token = peek();
    advance();
    
    switch (token.type) {
        case TokenType::NUMBER:
        case TokenType::STRING:
            return std::make_unique<ASTNode>(NodeType::LITERAL);
        case TokenType::IDENTIFIER:
            if (peek().type == TokenType::OPERATOR && peek().value == "(") {
                advance();
                std::vector<std::unique_ptr<ASTNode>> args;
                
                while (peek().type != TokenType::OPERATOR || peek().value != ")") {
                    if (!args.empty()) {
                        if (peek().type != TokenType::OPERATOR || peek().value != ",") {
                            throw std::runtime_error("Expected ',' between function arguments");
                        }
                        advance();
                    }
                    args.push_back(parse_expression());
                }
                advance();
                
                return std::make_unique<ASTNode>(NodeType::CALL_EXPRESSION);
            }
            return std::make_unique<ASTNode>(NodeType::IDENTIFIER);
        default:
            throw std::runtime_error("Unexpected token: " + token.value);
    }
}

std::unique_ptr<ASTNode> Parser::parse_variable_declaration() {
    Token identifier = peek();
    if (identifier.type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected variable name");
    }
    advance();
    
    Token equals = peek();
    if (equals.type == TokenType::OPERATOR && equals.value == "=") {
        advance();
        auto initializer = parse_expression();
    }
    
    if (peek().type == TokenType::OPERATOR && peek().value == ";") {
        advance();
    }
    
    return std::make_unique<ASTNode>(NodeType::VARIABLE_DECLARATION);
}

std::unique_ptr<ASTNode> Parser::parse_function_declaration() {
    if (!match(TokenType::IDENTIFIER)) {
        throw std::runtime_error("Expected function name");
    }
    
    Token next = peek();
    if (next.type != TokenType::OPERATOR || next.value != "(") {
        throw std::runtime_error("Expected '(' after function name");
    }
    advance();
    
    std::vector<std::string> parameters;
    while (peek().type != TokenType::OPERATOR || peek().value != ")") {
        if (!parameters.empty()) {
            Token comma = peek();
            if (comma.type != TokenType::OPERATOR || comma.value != ",") {
                throw std::runtime_error("Expected ',' between parameters");
            }
            advance();
        }
        
        if (!match(TokenType::IDENTIFIER)) {
            throw std::runtime_error("Expected parameter name");
        }
        parameters.push_back(peek().value);
    }
    advance();
    
    Token openBrace = peek();
    if (openBrace.type != TokenType::OPERATOR || openBrace.value != "{") {
        throw std::runtime_error("Expected '{' after function parameters");
    }
    advance();
    
    std::vector<std::unique_ptr<ASTNode>> body;
    while (peek().type != TokenType::OPERATOR || peek().value != "}") {
        body.push_back(parse_statement());
    }
    advance();
    
    return std::make_unique<ASTNode>(NodeType::FUNCTION_DECLARATION);
}
