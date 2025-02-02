#include"../Headers/Tokenizer.hpp"

void Log(const std::string& error)
{
    std::cerr << error << std::endl;
    exit(EXIT_FAILURE);
}

Token::Token(TokenType type, const std::string& value) : type(type), value(value)
{

}
std::string Token::EnumToString(TokenType token)
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
        case PLUS_EQUALS: return "+=";
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

bool Tokenizer::isValidIdentChar(const char& c)
{
    return isalnum(c) || c == '_';
}
std::optional<char> Tokenizer::peek(uint32_t offset) const
{
    if (index + offset == sourceCode.length())
        return {};

    return sourceCode[index + offset];
}
char Tokenizer::consume()
{
    return sourceCode[index++];
}
Tokenizer::Tokenizer(const std::string& sourceCode) : sourceCode(sourceCode), index(0)
{

}
void Tokenizer::RemoveLineComment()
{
    while (peek().has_value() && peek().value() != '\n')
        consume();
}

void Tokenizer::RemoveBlockComment()
{
    consume();
    consume();

    while (peek().has_value()) 
    {
        if (peek().value() == '*' && peek(1).has_value() && peek(1).value() == '/') 
        {
            consume();
            consume();
            return;
        }
        consume();
    }

    Log("[Tokenizer] Missing `*/` token");
}

std::vector<Token> Tokenizer::Tokenize()
{
    std::vector<Token> tokens;
    std::string buffer;

    std::cout << sourceCode << '\n';
    while (peek().has_value())
    {
        // std::cout << i++ << '\n';
        if (peek().value() == ';')
        {
            tokens.push_back(Token(SEMICOLON));
            consume();
        }
        else if (peek().value() == '+')
        {
            if (peek(1).has_value() && peek(1).value() == '=')
            {
                tokens.push_back(Token(PLUS_EQUALS));
                consume();
                consume();
                continue;
            }

            tokens.push_back(Token(PLUS));
            consume();
        }
        else if (peek().value() == '-')
        {
            if (peek(1).has_value() && peek(1).value() == '=')
            {
                tokens.push_back(Token(MINUS_EQUALS));
                consume();
                consume();
                continue;
            }

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
                continue;
            }

            if (peek(1).has_value() && peek(1).value() == '=')
            {
                tokens.push_back(Token(MULTIPLY_EQUALS));
                consume();
                consume();
                continue;
            }
            
            if (peek(1).has_value() && peek(1).value() == '/')
                Log("Missing `/*` token");
            

            tokens.push_back(Token(ASTERIK));
            consume();
            
        }
        else if (peek().value() == '/')
        {
            if (peek(1).has_value() && peek(1).value() == '=')
            {
                tokens.push_back(Token(DIVIDE_EQUALS));
                consume();
                consume();
            }            
            else if (peek(1).has_value() && peek(1).value() == '/')
            {
                this->RemoveLineComment();
            }
            else if (peek(1).has_value() && peek(1).value() == '*')
            {
                this->RemoveBlockComment();
            }
            else
            {
                tokens.push_back(Token(SLASH));
                consume();
            }
        }
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
            else if (buffer == "read")
                tokens.push_back(Token(READ));
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