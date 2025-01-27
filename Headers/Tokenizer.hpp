#pragma once
#include<iostream>
#include<vector>
#include<optional>
#include<string>

inline void Log(const std::string& error)
{
    std::cerr << error << std::endl;
    exit(EXIT_FAILURE);
}

enum TokenType
{
    EXIT,
    LET,
    IF,
    ELSE,
    ELIF,
    FOR,
    LET_STRING,
    PRINT,
    INT_LITERAL,
    STRING_LITERAL,
    IDENT,
    ENDLINE,
    SEMICOLON,
    DOUBLE_EQUAL,
    EQUAL,
    OPEN_PARENTHESIS,
    CLOSE_PARENTHESIS,
    OPEN_CURLY,
    CLOSE_CURLY,
    PLUS,
    MINUS,
    SLASH,
    ASTERIK,
    POW,
    LOGICAL_AND,
    LOGICAL_OR,
    LOGICAL_NOT,
    LOGICAL_XOR,
    INVALID,
};

struct Token
{
    TokenType type;
    std::string value;

    Token(TokenType type = INVALID, const std::string& value = std::string()) : type(type), value(value)
    {

    }
    static std::string EnumToString(TokenType token)
    {
        switch (token)
        {
            case EXIT: return "EXIT";
            case LET: return "LET";
            case IF: return "IF";
            case ELSE: return "ELSE";
            case ELIF: return "ELIF";
            case FOR: return "FOR";
            case LET_STRING: return "LET_STRING";
            case PRINT: return "PRINT";
            case INT_LITERAL: return "INT_LITERAL";
            case STRING_LITERAL: return "STRING_LITERAL";
            case IDENT: return "IDENT";
            case SEMICOLON: return ";";
            case DOUBLE_EQUAL: return "==";
            case EQUAL: return "=";
            case OPEN_PARENTHESIS: return "(";
            case CLOSE_PARENTHESIS: return ")";
            case OPEN_CURLY: return "{";
            case CLOSE_CURLY: return "}";
            case PLUS: return "+";
            case MINUS: return "-";
            case SLASH: return "/";
            case ASTERIK: return "*";
            case POW: return "**";
            case LOGICAL_AND: return "and";
            case LOGICAL_OR:  return "or";
            case LOGICAL_NOT: return "not";
            case LOGICAL_XOR: return "xor";
            case INVALID: return "INVALID";
            default: return "NEW";
        }
    }

};

class Tokenizer
{
private:
    const std::string& sourceCode;
    uint32_t index;

private:
    bool isValidIdentChar(const char& c)
    {
        return isalnum(c) || c == '_';
    }
    std::optional<char> previous() const
    {
        if (index == 0)
            return {};

        return sourceCode[index - 1];
    }
    std::optional<char> peek(uint32_t offset = 0) const
    {
        if (index + offset == sourceCode.length())
            return {};

        return sourceCode[index + offset];
    }
    char consume()
    {
        return sourceCode[index++];
    }
public:
    Tokenizer(const std::string& sourceCode) : sourceCode(sourceCode), index(0)
    {

    }
    std::vector<Token> Tokenize()
    {
        std::vector<Token> tokens;
        std::string buffer;
        while (peek().has_value())
        {
            if (peek().value() == ';')
            {
                tokens.push_back(Token(SEMICOLON));
                consume();
            }
            else if (peek().value() == '+')
            {
                tokens.push_back(Token(PLUS));
                consume();
            }
            else if (peek().value() == '-')
            {
                tokens.push_back(Token(MINUS));
                consume();
            }
            else if (peek().value() == '*')
            {
                if (peek(1).has_value() && peek(1).value() == '*') 
                {
                    tokens.push_back(Token(POW));
                    consume();
                    consume();
                }
                else if (peek(1).has_value() && peek(1).value() == '/')
                {
                    std::cerr << "Missing /* token\n";
                    exit(EXIT_FAILURE);
                }
                else
                {
                    tokens.push_back(Token(ASTERIK));
                    consume();
                }
            }
            else if (peek().value() == '/')
            {
                if (peek(1).has_value() && peek(1).value() == '/')
                {
                    while (peek().has_value() && peek().value() != '\n')
                        consume();
                }
                else if (peek(1).has_value() && peek(1).value() == '*')
                {
                    consume();
                    consume();

                    bool tokenFound = false;
                    while (peek().has_value() && peek(1).has_value())
                    {
                        if (peek().value() == '*' && peek(1).value() == '/')
                        {
                            tokenFound = true;
                            consume();
                            consume();
                            break;
                        }
                        
                        consume();
                    }

                    if (!tokenFound)
                    {
                        std::cerr << "Missing */ token\n";
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    tokens.push_back(Token(SLASH));
                    consume();
                }
            }
            // else if (peek().value() == '\\')
            // {
            //     if (peek(1).has_value() && peek(1).value() == 'n')
            //     {
            //         tokens.push_back(Token(ENDLINE));
            //         consume();
            //         consume();
            //     }
            //     else
            //         Log("Unknown Escape Sequence");
                
            // }
            else if (peek().value() == '(')
            {
                tokens.push_back(Token(OPEN_PARENTHESIS));
                consume();
            }
            else if (peek().value() == ')')
            {
                tokens.push_back(Token(CLOSE_PARENTHESIS));
                consume();
            }
            else if (peek().value() == '{')
            {
                tokens.push_back(Token(OPEN_CURLY));
                consume();
            }
            else if (peek().value() == '}')
            {
                tokens.push_back(Token(CLOSE_CURLY));
                consume();
            }
            else if (peek().value() == '=')
            {
                if (peek(1).has_value() && peek(1).value() == '=') 
                { 
                    tokens.push_back(Token(DOUBLE_EQUAL));
                    consume();
                    consume();
                } 
                else 
                {
                    tokens.push_back(Token(EQUAL));
                    consume();
                }
            }
            else if (peek().value() == '\"')
            {
                consume();
                while (peek().has_value() && peek().value() != '\"')
                {
                    if (peek().value() == '\\')
                    {
                        if (peek(1).has_value() && peek(1).value() == 'n')
                            buffer += '\n';
                        else
                            Log("Unknown Escape Sequence");

                        consume();
                        consume();
                        continue;
                    }
    
                    buffer += consume();
                }
                consume();
                tokens.push_back(Token(STRING_LITERAL, buffer));
                buffer.erase(buffer.begin(), buffer.end());
            }
            else if (isdigit(peek().value()))
            {
                while (peek().has_value() && peek().value() != ' ' && peek().value() != ';' && peek().value() != ')' && peek().value() != '\n')
                {
                    if (!isdigit(peek().value()))
                    {
                        std::cerr << "Invalid number\n";
                        exit(EXIT_FAILURE);
                    }

                    buffer += consume();
                }

                tokens.push_back(Token(INT_LITERAL, buffer));
                buffer.erase(buffer.begin(), buffer.end());
            }
            else if (isalnum(peek().value()))
            {
                while (peek().has_value() && isValidIdentChar(peek().value()))
                {
                    buffer += consume();
                }

                if (buffer == "exit")
                    tokens.push_back(Token(EXIT,buffer));
                else if (buffer == "let")
                    tokens.push_back(Token(LET));
                else if (buffer == "let_String")
                    tokens.push_back(Token(LET_STRING));
                else if (buffer == "if")
                    tokens.push_back(Token(IF));
                else if (buffer == "elif")
                    tokens.push_back(Token(ELIF));
                else if (buffer == "else")
                    tokens.push_back(Token(ELSE));
                else if (buffer == "print")
                    tokens.push_back(Token(PRINT));
                else if (buffer == "and")
                    tokens.push_back(Token(LOGICAL_AND));
                else if (buffer == "or")
                    tokens.push_back(Token(LOGICAL_OR));
                else if (buffer == "not")
                    tokens.push_back(Token(LOGICAL_NOT));
                else if (buffer == "xor")
                    tokens.push_back(Token(LOGICAL_XOR));
                else if (buffer == "for")
                    tokens.push_back(Token(FOR));
                else
                    tokens.push_back(Token(IDENT, buffer));
    
                buffer.erase(buffer.begin(), buffer.end());
            }
            else
            {
                consume();
            }
        }

        return tokens;
    }
    ~Tokenizer() = default;
};