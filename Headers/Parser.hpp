#pragma once
#include "Tokenizer.hpp"
#include "Nodes.hpp"
#include <unordered_map>

inline bool isOperand(const Token& token);
inline bool isOperator(const Token& token);

enum DataType
{
    INT,
    STRING,
};

class Parser
{
private:
    const std::vector<Token>& tokens;
    uint32_t index;
    std::unordered_map<std::string,DataType> type; 
public:
    Parser(const std::vector<Token>& tokens);
    std::optional<Node::StmtNode> ParseStmt();
    std::optional<Node::StmtNode> ParseElse();
    std::optional<Node::let_StringNode> ParseLetString();
    std::optional<Node::LetNode> ParseLet();
    Node::StringExprNode ParseStringExpr();
    Node::IntExprNode ParseIntExpr();
    Node::ForLoopNode* ParseForLoop();
    Node::IfNode* ParseIf();
    Node::ElifNode ParseElif();
    Node::IntAssignmentNode ParseIntAssignment();
    Node::StringAssignmentNode ParseStringAssignment();
    Node::ScopeNode* ParseScope();
    Node::ProgNode ParseProgram();
    Node::PrintNode ParsePrint();
    Node::ReadNode ParseRead();
    Node::ExitNode ParseExit();
    Node::AssignNode ParseAssignNode();

private:
    std::optional<Token> peek(uint32_t offset = 0) const;
    Token consume();
};