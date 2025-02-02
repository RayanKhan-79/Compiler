#pragma once
#include<iostream>
#include<vector>
#include<optional>
#include<string>
#include<cstdint>

void Log(const std::string& error);

enum TokenType
{
    EXIT, LET, IF, ELSE, ELIF, FOR, LET_STRING, PRINT, READ, INT_LITERAL, STRING_LITERAL, IDENT, 
    ENDLINE, SEMICOLON, DOUBLE_EQUAL, EQUAL, OPEN_PARENTHESIS, CLOSE_PARENTHESIS, OPEN_CURLY, 
    CLOSE_CURLY, PLUS, MINUS, SLASH, ASTERIK, MOD, PLUS_EQUALS, MINUS_EQUALS, DIVIDE_EQUALS, 
    MULTIPLY_EQUALS, POW, LOGICAL_AND, LOGICAL_OR, LOGICAL_NOT, LOGICAL_XOR, INVALID, Null
};

struct Token
{
    TokenType type;
    std::string value;

    Token(TokenType type = INVALID, const std::string& value = std::string());
    static std::string EnumToString(TokenType token);
};

class Tokenizer
{
private:
    const std::string& sourceCode;
    uint32_t index;

private:
    bool isValidIdentChar(const char& c);
    std::optional<char> peek(uint32_t offset = 0) const;
    char consume();
    void RemoveLineComment();
    void RemoveBlockComment();
public:
    Tokenizer(const std::string& sourceCode);
    std::vector<Token> Tokenize();
    ~Tokenizer() = default;
};