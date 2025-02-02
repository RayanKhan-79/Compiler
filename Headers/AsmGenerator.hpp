#pragma once
#include"Parser.hpp"
#include"ExpressionSolver.hpp"
#include"WinApiGenerator.hpp"
#include"ExpressionGenerator.hpp"
#include<unordered_map>
#include<filesystem>

#define commentsOFF 0

struct Variable
{
    size_t stackOffset;
    std::string name;
    bool isNull;

    Variable(size_t stackOffset, const std::string& name, bool isNull);
};

class AsmGenerator
{
private:
    const Node::ProgNode& progNode;
    const std::filesystem::path outputfile;
    std::ofstream outStream;
    
    std::vector<Variable> varMap;
    size_t stack_pointer;
    std::stack<size_t> base_pointer;
    
    int loopCount;
    int ifCount;
    int logicalTagCount;

    std::string indent;

    WinApiGenerator winApi;
    ExpressionSolver solver;
    ExpressionGenerator* AEG;
public:
    AsmGenerator(const Node::ProgNode& progNode, const std::filesystem::path outputfile);
    void generateString(const Node::StringLitNode& node);
    void generatePrint(const Node::PrintNode& node);
    void generateRead(const Node::ReadNode& node);
    const Variable* find(const std::string& varName) const;
    Variable* find(const std::string& varName);
    void generateExpression(const std::vector<std::variant<Node::OperandNode, Node::OperatorNode>>& expression);
    void generateLetAsm(const Node::LetNode& letNode);
    void generateIfAsm(const Node::IfNode* ifnode);
    void generateExitAsm(const Node::ExitNode& exitNode);
    void generateIntAssignment(const Node::IntAssignmentNode& node);
    void generateStrAssignment(const Node::StringAssignmentNode& node);
    void generateLetStringAsm(const Node::let_StringNode& node);
    void generateForLoopASM(const Node::ForLoopNode* forloop);
    void generateProgramAsm();
    void generateStmt(const Node::StmtNode& stmtNode);
    void generateScope(const Node::ScopeNode* scopeNode);
    ~AsmGenerator();
private:
    void Comment(const std::string& comment, const std::string& indent);
    void increaseIndent();
    void decreaseIndent();
    std::string createIf();
    void createElif(const std::string& tag, size_t i = 0);
    void endElif(const std::string& tag, size_t i = 0);
    void endif(const std::string& tag, size_t i);
    std::string StrartForLoop();
    void EndForLoop(const std::string& tag);
    std::string StartLoop();
    void EndLoop(const std::string& tag);
    void push(const std::string& reg);
    void pop(const std::string& reg);
};