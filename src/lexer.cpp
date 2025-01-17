#include "../include/lexer.hpp"
#include <cctype>
#include <stdexcept>

js::Lexer::Lexer(const std::string& source) : input(source), position(0) {
    keywords = {
        {"let", TokenType::KEYWORD},
        {"const", TokenType::KEYWORD},
        {"var", TokenType::KEYWORD},
        {"function", TokenType::KEYWORD},
        {"return", TokenType::KEYWORD},
        {"if", TokenType::KEYWORD},
        {"else", TokenType::KEYWORD},
        {"while", TokenType::KEYWORD},
        {"for", TokenType::KEYWORD}
    };
    current_char = input.empty() ? '\0' : input[0];
}

void js::Lexer::advance() {
    position++;
    current_char = position < input.length() ? input[position] : '\0';
}

void js::Lexer::skip_whitespace() {
    while (current_char && std::isspace(current_char)) {
        advance();
    }
}

std::string js::Lexer::get_number() {
    std::string result;
    bool hasDecimal = false;
    
    while (current_char && (std::isdigit(current_char) || current_char == '.')) {
        if (current_char == '.') {
            if (hasDecimal) break;
            hasDecimal = true;
        }
        result += current_char;
        advance();
    }
    
    return result;
}

std::string js::Lexer::get_identifier() {
    std::string result;
    
    while (current_char && (std::isalnum(current_char) || current_char == '_')) {
        result += current_char;
        advance();
    }
    
    return result;
}

std::string js::Lexer::get_string() {
    char quote = current_char;
    advance();
    std::string result;
    
    while (current_char && current_char != quote) {
        if (current_char == '\\') {
            advance();
            switch (current_char) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                default: result += current_char;
            }
        } else {
            result += current_char;
        }
        advance();
    }
    
    if (current_char == quote) {
        advance();
    }
    
    return result;
}

std::vector<js::Token> js::Lexer::tokenize() {
    std::vector<js::Token> tokens;
    
    while (current_char) {
        if (std::isspace(current_char)) {
            skip_whitespace();
            continue;
        }
        
        if (std::isdigit(current_char)) {
            tokens.emplace_back(TokenType::NUMBER, get_number());
            continue;
        }
        
        if (std::isalpha(current_char) || current_char == '_') {
            std::string identifier = get_identifier();
            auto it = keywords.find(identifier);
            if (it != keywords.end()) {
                tokens.emplace_back(it->second, identifier);
            } else {
                tokens.emplace_back(TokenType::IDENTIFIER, identifier);
            }
            continue;
        }
        
        if (current_char == '"' || current_char == '\'') {
            tokens.emplace_back(TokenType::STRING, get_string());
            continue;
        }
        
        if (current_char == '.') {
            advance();
            tokens.emplace_back(TokenType::DOT, ".");
            continue;
        }
        
        if (std::string("+-*/()=;{}[],<>!&|").find(current_char) != std::string::npos) {
            std::string op(1, current_char);
            advance();
            
            if (current_char) {
                if ((op == "=" && current_char == '=') ||
                    (op == "!" && current_char == '=') ||
                    (op == "<" && current_char == '=') ||
                    (op == ">" && current_char == '=') ||
                    (op == "&" && current_char == '&') ||
                    (op == "|" && current_char == '|')) {
                    op += current_char;
                    advance();
                }
            }
            
            tokens.emplace_back(TokenType::OPERATOR, op);
            continue;
        }
        
        throw std::runtime_error("Invalid character encountered: " + std::string(1, current_char));
    }
    
    tokens.emplace_back(TokenType::EOF_TOKEN, "");
    return tokens;
}
