#include"../Headers/ExpressionSolver.hpp"
    
ExpressionSolver::ExpressionSolver() 
{
}
int ExpressionSolver::getPrecedence(const Token& op) const
{
    switch (op.type)
    {
        
        case LOGICAL_OR: 
        case LOGICAL_XOR:
            return 1; 
        
        case LOGICAL_AND: 
            return 2;

        case PLUS:
        case MINUS: 
            return 3;
        
        case ASTERIK:
        case SLASH: 
            return 4;
        
        case POW: 
            return 5;
        
        case LOGICAL_NOT: 
            return 10;
    }
    return -1;
}
bool ExpressionSolver::higherPrecedence(const Node::OperatorNode& op1, const Node::OperatorNode& op2) const
{
    return this->getPrecedence(op1.operatorToken.type) > this->getPrecedence(op2.operatorToken.type);
}

std::vector<std::variant<Node::OperandNode, Node::OperatorNode>> ExpressionSolver::ConvertToPostFix(const Node::IntExprNode* AEnode)
{
    std::vector<std::variant<Node::OperandNode, Node::OperatorNode>> postFixExpr;
    std::stack<Node::OperatorNode> opstk;
    postFixExpr.reserve(AEnode->expression.size());

    struct Visitor 
    {
        std::vector<std::variant<Node::OperandNode, Node::OperatorNode>>& postFixExpr;
        std::stack<Node::OperatorNode>& opstk;
        ExpressionSolver* solver;

        Visitor(std::vector<std::variant<Node::OperandNode, Node::OperatorNode>>& postFixExpr, std::stack<Node::OperatorNode>& opstk, ExpressionSolver* solver) :
            postFixExpr(postFixExpr),
            solver(solver),
            opstk(opstk)
        {
            
        }

        void operator()(const Node::OperatorNode& operatorNode)
        {
            if (opstk.empty() || operatorNode.operatorToken.type == OPEN_PARENTHESIS)
            {
                opstk.push(operatorNode);
                return;
            }

            while (operatorNode.operatorToken.type == CLOSE_PARENTHESIS && opstk.empty() == false)
            {
                if (opstk.top().operatorToken.type == OPEN_PARENTHESIS)
                {
                    opstk.pop();
                    return;
                }

                postFixExpr.push_back(opstk.top());
                opstk.pop();
            }

            if (solver->higherPrecedence(operatorNode, opstk.top()))
            {
                opstk.push(operatorNode);
                return;
            }

            while (!opstk.empty() && !solver->higherPrecedence(operatorNode, opstk.top()))
            {
                postFixExpr.push_back(opstk.top());
                opstk.pop();
            }
            opstk.push(operatorNode);

        }
        
        void operator()(const Node::OperandNode& operandNode)
        {
            postFixExpr.push_back(operandNode);
        }   
    };

    Visitor visitor(postFixExpr, opstk, this);

    for (const std::variant<Node::OperandNode, Node::OperatorNode>& token : AEnode->expression)
    {
        std::visit(visitor, token);
    }

    while (!opstk.empty())
    {
        postFixExpr.push_back(opstk.top());
        opstk.pop();
    }

    return postFixExpr;
}