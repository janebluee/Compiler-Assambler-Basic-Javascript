#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace js {

enum class TokenType {
    NUMBER,
    STRING,
    IDENTIFIER,
    KEYWORD,
    OPERATOR,
    LEFT_PAREN,
    RIGHT_PAREN,
    COMMA,
    DOT,
    EOF_TOKEN
};

struct Token {
    TokenType type;
    std::string value;
    Token(TokenType t, std::string v) : type(t), value(v) {}
};

class Lexer {
private:
    std::string input;
    size_t position;
    char current_char;
    std::unordered_map<std::string, TokenType> keywords;

    void advance();
    void skip_whitespace();
    std::string get_number();
    std::string get_identifier();
    std::string get_string();

public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();
};

} 
