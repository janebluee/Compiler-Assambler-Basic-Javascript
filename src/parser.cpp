#include "../include/parser.hpp"
#include <stdexcept>

js::Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)), current(0) {}

js::Token js::Parser::peek() {
    if (current >= tokens.size()) {
        return Token(TokenType::EOF_TOKEN, "");
    }
    return tokens[current];
}

js::Token js::Parser::advance() {
    if (current < tokens.size()) {
        return tokens[current++];
    }
    return Token(TokenType::EOF_TOKEN, "");
}

bool js::Parser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

js::NodePtr js::Parser::parse() {
    if (peek().type == TokenType::EOF_TOKEN) {
        return std::make_unique<Expression>(NodeType::PROGRAM);
    }
    return parse_statement();
}

js::NodePtr js::Parser::parse_statement() {
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
            auto ret = std::make_unique<ReturnStatement>();
            ret->argument = std::move(expr);
            return std::move(ret);
        }
    }
    
    auto expr = parse_expression();
    if (peek().type == TokenType::OPERATOR && peek().value == ";") {
        advance();
    }
    return expr;
}

js::NodePtr js::Parser::parse_expression() {
    auto left = parse_primary();
    
    while (peek().type == TokenType::OPERATOR) {
        std::string op = peek().value;
        if (op != "+" && op != "-" && op != "*" && op != "/") {
            break;
        }
        advance();
        
        auto right = parse_primary();
        auto binary = std::make_unique<BinaryExpression>();
        binary->left = std::move(left);
        binary->right = std::move(right);
        binary->op = op;
        left = std::move(binary);
    }
    
    return left;
}

js::NodePtr js::Parser::parse_primary() {
    Token token = peek();
    
    if (token.type == TokenType::NUMBER) {
        advance();
        auto literal = std::make_unique<Literal>();
        literal->value = std::stod(token.value);
        return std::move(literal);
    }
    if (token.type == TokenType::STRING) {
        advance();
        auto literal = std::make_unique<Literal>();
        literal->value = token.value;
        return std::move(literal);
    }
    if (token.type == TokenType::IDENTIFIER) {
        advance();
        auto identifier = std::make_unique<Identifier>();
        identifier->name = token.value;
        
        // Check for member access
        if (match(TokenType::DOT)) {
            return parseMemberExpression(std::move(identifier));
        }
        
        // Check for function call
        if (match(TokenType::LEFT_PAREN)) {
            return parseCallExpression(std::move(identifier));
        }
        
        return std::move(identifier);
    }
    
    throw std::runtime_error("Unexpected token: " + token.value);
}

js::NodePtr js::Parser::parseCallExpression(NodePtr callee) {
    auto call = std::make_unique<CallExpression>();
    call->callee = std::move(callee);
    
    while (!match(TokenType::RIGHT_PAREN)) {
        call->arguments.push_back(parse_expression());
        if (!match(TokenType::COMMA)) {
            if (peek().type != TokenType::RIGHT_PAREN) {
                throw std::runtime_error("Expected ',' or ')' in argument list");
            }
            match(TokenType::RIGHT_PAREN);
            break;
        }
    }
    
    return std::move(call);
}

js::NodePtr js::Parser::parseMemberExpression(NodePtr object) {
    auto member = std::make_unique<MemberExpression>();
    member->object = std::move(object);
    
    // Get property name
    auto token = peek();
    if (token.type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected property name after dot");
    }
    member->property = token.value;
    advance();
    
    return std::move(member);
}

js::NodePtr js::Parser::parse_variable_declaration() {
    Token identifier = peek();
    if (identifier.type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected variable name");
    }
    advance();
    
    auto decl = std::make_unique<VariableDeclaration>();
    decl->name = identifier.value;
    
    if (peek().type == TokenType::OPERATOR && peek().value == "=") {
        advance();
        decl->init = parse_expression();
    }
    
    if (peek().type == TokenType::OPERATOR && peek().value == ";") {
        advance();
    }
    
    return std::move(decl);
}

js::NodePtr js::Parser::parse_function_declaration() {
    Token name = peek();
    if (!match(TokenType::IDENTIFIER)) {
        throw std::runtime_error("Expected function name");
    }
    
    auto decl = std::make_unique<FunctionDeclaration>();
    decl->name = name.value;
    
    if (peek().type != TokenType::OPERATOR || peek().value != "(") {
        throw std::runtime_error("Expected '(' after function name");
    }
    advance();
    
    while (peek().type != TokenType::OPERATOR || peek().value != ")") {
        if (!decl->params.empty()) {
            if (peek().type != TokenType::OPERATOR || peek().value != ",") {
                throw std::runtime_error("Expected ',' between parameters");
            }
            advance();
        }
        
        Token param = peek();
        if (!match(TokenType::IDENTIFIER)) {
            throw std::runtime_error("Expected parameter name");
        }
        decl->params.push_back(param.value);
    }
    advance();
    
    if (peek().type != TokenType::OPERATOR || peek().value != "{") {
        throw std::runtime_error("Expected '{' after function parameters");
    }
    advance();
    
    while (peek().type != TokenType::OPERATOR || peek().value != "}") {
        decl->body.push_back(parse_statement());
    }
    advance();
    
    return std::move(decl);
}
