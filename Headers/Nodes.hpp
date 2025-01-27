#pragma once
#include<vector>
#include<variant>
#include"Tokenizer.hpp"

namespace Node
{
    struct IntIdentNode
    {
        Token label;

        // Parameterized constructor with default argument
        IntIdentNode(const Token& label = INVALID) : label(label) {}
    };

    struct IntLitNode
    {
        Token intLit;

        // Parameterized constructor with default argument
        IntLitNode(const Token& intLit = INVALID) : intLit(intLit) {}
    };

    struct StringIdentNode
    {
        Token label;
        StringIdentNode(const Token& label = INVALID) : label(label) {}
    };

    struct StringLitNode
    {
        Token stringLit;
        StringLitNode(const Token& stringLit = INVALID) : stringLit(stringLit) {}
    };

    struct OperandNode
    {
        std::variant<IntIdentNode, IntLitNode> exprVar;

        // Parameterized constructor with default argument
        OperandNode(const std::variant<IntIdentNode, IntLitNode>& exprVar = {}) : exprVar(exprVar) {}
    };

    struct OperatorNode
    {
        Token operatorToken;

        OperatorNode(const Token& operatorToken = Token()) : operatorToken(operatorToken) {}
    };

    struct IntExprNode
    {
        std::vector<std::variant<OperandNode, OperatorNode>> expression;  
    };

    struct StringExprNode
    {
        std::variant<StringLitNode, StringIdentNode> expression;

        StringExprNode(const std::variant<StringLitNode, StringIdentNode>& expression = std::variant<StringLitNode, StringIdentNode>()) : expression(expression) {}
    };

    struct ExitNode
    {
        IntExprNode expr;

        // Parameterized constructor with default argument
        ExitNode(const IntExprNode& expr = IntExprNode()) : expr(expr) {}
    };

    struct let_StringNode
    {
        StringIdentNode identNode;
        StringExprNode strExpr;
        let_StringNode(StringIdentNode identNode, StringExprNode strExpr) : identNode(identNode), strExpr(strExpr) {}
    };

    struct PrintNode
    {
        StringExprNode strExpr;

        PrintNode(StringExprNode strExpr = StringExprNode()) : strExpr(strExpr) {};
    };

    struct LetNode
    {
        IntIdentNode identNode;
        IntExprNode exprNode;

        // Parameterized constructor with default arguments
        LetNode(const IntIdentNode& identNode = IntIdentNode(), const IntExprNode& exprNode = IntExprNode()) : identNode(identNode), exprNode(exprNode) {}
    };

    struct IntAssignmentNode
    {
        OperatorNode operatorNode;
        IntIdentNode left;
        IntExprNode right;

        IntAssignmentNode(
        const OperatorNode& operatorNode = OperatorNode(),
        const IntIdentNode& left = IntIdentNode(),
        const IntExprNode& right = IntExprNode()
        ) : operatorNode(operatorNode), left(left), right(right)
        {
            
        }
    };

    struct StringAssignmentNode
    {
        OperatorNode operatorNode;
        StringIdentNode left;
        StringExprNode right;

        StringAssignmentNode(
        const OperatorNode& operatorNode = OperatorNode(),
        const StringIdentNode& left = StringIdentNode(),
        const StringExprNode& right = StringExprNode()
        ) : operatorNode(operatorNode), left(left), right(right)
        {

        }

    };

    // struct AssignmentNode
    // {
    //     std::variant<StringAssignmentNode, IntAssignmentNode> var;
    //     AssignmentNode(const std::variant<StringAssignmentNode, IntAssignmentNode>& var = std::variant<StringAssignmentNode, IntAssignmentNode>()) : var(var) {};
    // };

    struct ScopeNode;
    struct IfNode;
    struct ForLoopNode;

    struct StmtNode
    {
        std::variant<ExitNode, LetNode, PrintNode, let_StringNode, IntAssignmentNode, StringAssignmentNode, ScopeNode*, IfNode*, ForLoopNode*> stmtVar;

        // Parameterized constructor with default argument
        StmtNode(const std::variant<ExitNode, LetNode, PrintNode, let_StringNode, IntAssignmentNode, StringAssignmentNode, ScopeNode*, IfNode*, ForLoopNode*>& stmtVar = {}) : stmtVar(stmtVar) {}
    };

    struct ScopeNode
    {
        std::vector<StmtNode> statements;
    };

    struct ElifNode
    {
        IntExprNode boolean;
        StmtNode stmt;
        ElifNode(const IntExprNode& boolean = IntExprNode(), const StmtNode& stmt = StmtNode()) 
            : boolean(boolean),
            stmt(stmt)
        {

        }
    };

    struct IfNode
    {
        IntExprNode boolean;
        StmtNode ifStmt;
        std::vector<ElifNode> elifStmts;
        std::optional<StmtNode> elseStmt;

        IfNode(const IntExprNode& boolean = IntExprNode(), const StmtNode& ifStmt = StmtNode(), const std::optional<StmtNode>& elseStmt = std::optional<StmtNode>()) 
            : boolean(boolean), 
            ifStmt(ifStmt),
            elseStmt(elseStmt) 
        {

        } 
    };

    struct ForLoopNode
    {
        LetNode initialization;
        IntExprNode condition;
        IntAssignmentNode updation;
        StmtNode loopBody;

        ForLoopNode(const LetNode& initialization,
        const IntExprNode& condition,
        const IntAssignmentNode& updation,
        const StmtNode& loopBody) :
        
        initialization(initialization),
        condition(condition),
        updation(updation),
        loopBody(loopBody)
        {

        }
    };

    struct ProgNode
    {
        std::vector<StmtNode> statements;

        // Parameterized constructor with default argument
        ProgNode(const std::vector<StmtNode>& statements = {}) 
            : statements(statements) 
        {

        }
    };
};