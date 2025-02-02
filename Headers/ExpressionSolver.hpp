#pragma once
#include"Nodes.hpp"
#include<stack>
#include<unordered_map>

class ExpressionSolver
{
public:
    ExpressionSolver();
    int getPrecedence(const Token& op) const;
    bool higherPrecedence(const Node::OperatorNode& op1, const Node::OperatorNode& op2) const;
    std::vector<std::variant<Node::OperandNode, Node::OperatorNode>> ConvertToPostFix(const Node::IntExprNode* AEnode);
};