#include"../Headers/Parser.hpp"

inline bool isOperand(const Token& token)
{
    return token.type == IDENT 
    || token.type == INT_LITERAL;
}

inline bool isOperator(const Token& token)
{
    return token.type == PLUS 
    || token.type == MINUS 
    || token.type == ASTERIK 
    || token.type == SLASH 
    || token.type == OPEN_PARENTHESIS 
    || token.type == CLOSE_PARENTHESIS 
    || token.type == POW
    || token.type == LOGICAL_AND
    || token.type == LOGICAL_OR
    || token.type == LOGICAL_XOR
    || token.type == LOGICAL_NOT;
}

std::optional<Token> Parser::peek(uint32_t offset) const
{
if (index + offset == tokens.size())
    return {};

return tokens[index + offset];
}

Token Parser::consume()
{
    return tokens[index++];
}

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), index(0)
{

}

std::optional<Node::StmtNode> Parser::ParseStmt()
{
    if (!peek().has_value())
        return {};

    if (peek().value().type == EXIT) {
        auto stmt = ParseExit();
        if (peek().has_value() && peek().value().type == SEMICOLON) {
            consume();
            return std::optional<Node::StmtNode>(stmt);
        }
        Log("Expected `;`");
    }

    if (peek().value().type == LET) {
        auto stmt = ParseLet();
        if (peek().has_value() && peek().value().type == SEMICOLON) {
            consume();
            return std::optional<Node::StmtNode>(stmt);
        }
        Log("Expected `;`");
    }

    if (peek().value().type == PRINT) {
        auto stmt = ParsePrint();
        if (peek().has_value() && peek().value().type == SEMICOLON) {
            consume();
            return std::optional<Node::StmtNode>(stmt);
        }
        Log("Expected `;`");
    }

    if (peek().value().type == READ) {
        auto stmt = ParseRead();
        if (peek().has_value() && peek().value().type == SEMICOLON) {
            consume();
            return std::optional<Node::StmtNode>(stmt);
        }
        Log("Expected `;`");
    }


    if (peek().value().type == LET_STRING) {
        auto stmt = ParseLetString();
        if (peek().has_value() && peek().value().type == SEMICOLON) {
            consume();
            return std::optional<Node::StmtNode>(stmt);
        }
        Log("Expected `;`");
    }
    
    if (peek().value().type == IDENT && type[peek().value().value] == INT && peek(1).has_value() && peek(1).value().type == EQUAL) {
        auto stmt = ParseIntAssignment();
        if (peek().has_value() && peek().value().type == SEMICOLON) {
            consume();
            return std::optional<Node::StmtNode>(stmt);
        }
        Log("Expected `;`");
    }
    
    if (peek().value().type == IDENT && type[peek().value().value] == STRING && peek(1).has_value() && peek(1).value().type == EQUAL) {
        auto stmt = ParseStringAssignment();
        if (peek().has_value() && peek().value().type == SEMICOLON) {
            consume();
            return std::optional<Node::StmtNode>(stmt);
        }
        Log("Expected `;`");
    }

    if (peek().value().type == IF) {
        return std::optional<Node::StmtNode>(ParseIf());
    }
    if (peek().value().type == OPEN_CURLY) {
        return std::optional<Node::StmtNode>(ParseScope());
    }
    if (peek().value().type == FOR) {
        return std::optional<Node::StmtNode>(ParseForLoop());
    }

    // if (peek().value().type == EXIT)
    //     return std::optional<Node::StmtNode>(ParseExit());

    // if (peek().value().type == LET)
    //     return std::optional<Node::StmtNode>(ParseLet());

    // if (peek().value().type == OPEN_CURLY)
    //     return std::optional<Node::StmtNode>(ParseScope());

    // if (peek().value().type == PRINT)
    //     return std::optional<Node::StmtNode>(ParsePrint());

    // if (peek().value().type == LET_STRING)
    //     return std::optional<Node::StmtNode>(ParseLetString());

    // if (peek().value().type == IF)
    //     return std::optional<Node::StmtNode>(ParseIf());

    // if (peek().value().type == FOR)
    //     return std::optional<Node::StmtNode>(ParseForLoop());

    // if (peek().value().type == IDENT && identifiers[peek().value().value] == INT && peek(1).has_value() && peek(1).value().type == EQUAL)
    //     return std::optional<Node::StmtNode>(ParseIntAssignment());

    // if (peek().value().type == IDENT && identifiers[peek().value().value] == STRING && peek(1).has_value() && peek(1).value().type == EQUAL)
    //     return std::optional<Node::StmtNode>(ParseStringAssignment());

    std::cerr << "Unknown keyword " << peek().value().type << " " << Token::EnumToString(peek().value().type) << "\n";
    return {};
}

Node::ForLoopNode* Parser::ParseForLoop()
{
    if (!peek(1).has_value() || peek(1).value().type != OPEN_PARENTHESIS)
        Log("[Parsing For Loop] Expected `(`");

    consume(); // for
    consume(); // (

    std::optional<Node::LetNode> letNode = ParseLet();
    if (!letNode.has_value())
        Log("[Parsing For Loop] Expected a Declaration");

    if (!peek().has_value() || peek().value().type != SEMICOLON)
        Log("[Parsing For Loop] Expected `;`");
    consume(); // ;

    std::optional<Node::IntExprNode> condNode = ParseIntExpr();
    if (!condNode.has_value())
        Log("[Parsing For Loop] Expected a Condition");
    
    if (!peek().has_value() || peek().value().type != SEMICOLON)
        Log("[Parsing For Loop] Expected `;`");
    consume(); // ;

    std::optional<Node::IntAssignmentNode> updateNode = ParseIntAssignment();
    if (!updateNode.has_value())
        Log("[Parsing For Loop] Expected an assignment expression");
    
    if (!peek().has_value() || peek().value().type != CLOSE_PARENTHESIS)
        Log("[Parsing For Loop] Expected `)`");
    consume(); // )

    std::optional<Node::StmtNode> stmtNode = ParseStmt();
    if (!stmtNode.has_value())
        Log("[Parsing For Loop] Expected a loop body");

    return new Node::ForLoopNode(letNode.value(), condNode.value(), updateNode.value(), stmtNode.value());
}

Node::IfNode* Parser::ParseIf()
{
    if (!peek(1).has_value() || peek(1).value().type != OPEN_PARENTHESIS)
        Log("[Parsing If] Expected `)`");
        
    consume(); // if
    consume(); // (

    std::optional<Node::IntExprNode> boolean = ParseIntExpr();
    if (!boolean.has_value())
        Log("[Parsing If] Expected a Convertable to Bool Expression");

    if (!peek().has_value() || peek().value().type != CLOSE_PARENTHESIS)
        Log("[Parsing If] Expected `)`");
    consume(); // )

    std::optional<Node::StmtNode> stmt = ParseStmt();
    
    if (!stmt.has_value())
        Log("[Parsing If] Expected at least 1 statement");

    Node::IfNode* ifnode = new Node::IfNode(boolean.value(), stmt.value());

    while (peek().has_value() && peek().value().type == ELIF)
        ifnode->elifStmts.push_back(ParseElif());

    ifnode->elseStmt = ParseElse();

    return ifnode;
}

Node::ElifNode Parser::ParseElif()
{
    consume(); // elif

    if (!peek().has_value() || peek().value().type != OPEN_PARENTHESIS)
        Log("[Parsing Elif] Expected `(`");
    consume(); // (

    std::optional<Node::IntExprNode> expr = ParseIntExpr();
    
    if (expr.has_value() == false)
        Log("[Parsing Elif] Expected a convertable to boolean Expression");

    if (!peek().has_value() || peek().value().type != CLOSE_PARENTHESIS)
        Log("[Parsing Elif] Expected `)`");
    consume(); // )

    std::optional<Node::StmtNode> stmt = ParseStmt();

    if (stmt.has_value() == false)
        Log("[Parsing Elif] Expected at least 1 statement");

    
    return Node::ElifNode(expr.value(), stmt.value());
}

std::optional<Node::StmtNode> Parser::ParseElse()
{
    if (!peek().has_value() || peek().value().type != ELSE)
        return {};

    consume();
    std::optional<Node::StmtNode> stmt = ParseStmt();
    
    if (stmt.has_value() == false)
        Log("Expected at least 1 statement after else keyword");

    return Node::StmtNode(stmt.value());
}

Node::AssignNode Parser::ParseAssignNode()
{
    if (peek().has_value() == false)
        Log("[Parsing Assignmnet] Expected an Assignment Operator Token Variant");

    switch (consume().type)
    {
        case PLUS_EQUALS:
            return Node::PlusEqNode();
        case MINUS_EQUALS:
            return Node::MinusEqNode();
        case MULTIPLY_EQUALS:
            return Node::MulEqNode();
        case DIVIDE_EQUALS:
            return Node::DivEqNode();
        case EQUAL:
            return Node::NormEqNode(); 
        default:
            Log("[Parsing Assignmnet] Expected an Assignment Operator Token Variant");
            return {};
    }
}

Node::IntAssignmentNode Parser::ParseIntAssignment()
{
    Node::IntIdentNode left(consume()); // ident
    Node::AssignNode op = ParseAssignNode();   // =

    std::optional<Node::IntExprNode> parsed = ParseIntExpr();
    if (!parsed.has_value())
        Log("[Parsing Int Assignment] Expected an Expression");

    Node::IntExprNode right(parsed.value());

    return Node::IntAssignmentNode(op, left, right);
}

Node::StringAssignmentNode Parser::ParseStringAssignment()
{
    Node::StringIdentNode left(consume()); // ident
    Node::OperatorNode op(consume()); // =

    std::optional<Node::StringExprNode> parsed = ParseStringExpr();
    if (!parsed.has_value())
        Log("[Parsing String Assignment] Expected an Expression");

    Node::StringExprNode right(parsed.value());

    return Node::StringAssignmentNode(op, left, right);
}

Node::ScopeNode* Parser::ParseScope()
{
    consume(); // {
    Node::ScopeNode* scopeNode = new Node::ScopeNode();
    while (peek().has_value() && peek().value().type != CLOSE_CURLY)
    {
        std::optional<Node::StmtNode> parsedResult = ParseStmt();
        if (!parsedResult.has_value())
            Log("[Parsing Scope] Couldn't Parse Expression");

        scopeNode->statements.push_back(parsedResult.value());
    }

    consume(); // }
    return scopeNode;
}

Node::ProgNode Parser::ParseProgram()
{
    Node::ProgNode progNode;
    while (peek().has_value())
    {
        std::optional<Node::StmtNode> parsedResult = ParseStmt();
        
        if (!parsedResult.has_value())
        {
            std::cerr << "Couldn't Parse Statement " << progNode.statements.size() + 1 << "\n";
            exit(EXIT_FAILURE);
        }

        progNode.statements.push_back(parsedResult.value());
    }

    return progNode;
}

Node::StringExprNode Parser::ParseStringExpr()
{
    if (peek().value().type == STRING_LITERAL)
        return Node::StringExprNode(Node::StringLitNode(consume()));

    if (peek().value().type == IDENT)
        return Node::StringExprNode(Node::StringIdentNode(consume()));

    return Node::StringExprNode(Node::NullNode());
}

std::optional<Node::let_StringNode> Parser::ParseLetString()
{
    consume();
    if (!peek().has_value() || peek().value().type != IDENT)
    {
        std::cerr << "Expected an identifier\n";
        return {};
    }

    type[peek().value().value] = STRING;
    Node::StringIdentNode identNode(consume());
    
    if (!peek().has_value() || peek().value().type != EQUAL)
    {
        std::cerr << "Expected \'=\'";
        return {};
    }

    consume();

    std::optional<Node::StringExprNode> parsedResult = ParseStringExpr();

    Node::StringExprNode exprNode(parsedResult.value());
    return Node::let_StringNode(identNode, exprNode);
}

std::optional<Node::LetNode> Parser::ParseLet()
{
    if (!peek().has_value() || peek().value().type != LET)
        return {};

    consume();

    if (!peek().has_value() || peek().value().type != IDENT)
    {
        std::cerr << "Expected an identifier\n";
        return {};
    }

    type[peek().value().value] = INT;
    Node::IntIdentNode identNode(consume());
    
    if (!peek().has_value() || peek().value().type != EQUAL)
    {
        std::cerr << "Expected \'=\'";
        return {};
    }

    consume();

    std::optional<Node::IntExprNode> parsedResult = ParseIntExpr();

    if (!parsedResult.has_value())
    {
        std::cerr << "Couldn't Parse Expression\n";
        return {};
    }

    Node::IntExprNode exprNode(parsedResult.value());
    return Node::LetNode(identNode, exprNode);
}

Node::PrintNode Parser::ParsePrint()
{

    if (!peek(1).has_value() || peek(1).value().type != OPEN_PARENTHESIS)
        Log("[Parsing Print] Expected `(`");

    consume(); // print
    consume(); // (

    
    std::optional<std::variant<Node::IntExprNode, Node::StringExprNode>> result;
    if (peek().value().type == INT_LITERAL)
        result = ParseIntExpr();
    else
        result = ParseStringExpr();

    if (!result.has_value())
        Log("[Parsing Print] Expected an expression");
    
    if (!peek().has_value() || peek().value().type != CLOSE_PARENTHESIS)
        Log("[Parsing Print] Expected `)`");

    consume(); // )

    return Node::PrintNode(result.value());
}

Node::IntExprNode Parser::ParseIntExpr()
{
    size_t openCount = 0;
    Node::IntExprNode exprNode;
    while (peek().has_value() && (isOperand(peek().value()) || isOperator(peek().value())))
    {
        if (peek().value().type == CLOSE_PARENTHESIS && openCount == 0)
        {
            break;
        }
        else if (peek().value().type == CLOSE_PARENTHESIS)
        {
            openCount -= 1;
            exprNode.expression.push_back(Node::OperatorNode(consume()));
        }
        else if (peek().value().type == OPEN_PARENTHESIS)
        {
            openCount += 1;
            exprNode.expression.push_back(Node::OperatorNode(consume()));
        }
        else if (isOperator(peek().value()))
            exprNode.expression.push_back(Node::OperatorNode(consume()));
        else if (peek().value().type == IDENT)
            exprNode.expression.push_back(Node::OperandNode(Node::IntIdentNode(consume())));
        else if (peek().value().type == INT_LITERAL)
            exprNode.expression.push_back(Node::OperandNode(Node::IntLitNode(consume())));
    }

    if (openCount) {
        Log("Imbalanced Expression");
    }

    return exprNode;
}

Node::ReadNode Parser::ParseRead()
{
    if (!peek(1).has_value() || peek(1).value().type != OPEN_PARENTHESIS)
        Log("[Parsing Read] Expected `(`");

    consume();
    consume();

    if (!peek().has_value() || peek().value().type != IDENT)
        Log("[Parsing Read] Expected an ident");

    if (!peek(1).has_value() || peek(1).value().type != CLOSE_PARENTHESIS)
        Log("[Parsing Read] Expected an `)`");

    Node::ReadNode readNode;
    if (type[peek().value().value] == INT)
        readNode.ident = Node::IntIdentNode(consume());
    else if (type[peek().value().value] == STRING)
        readNode.ident = Node::StringIdentNode(consume());

    consume();
    return readNode;
}

Node::ExitNode Parser::ParseExit()
{
    if (!peek(1).has_value() || peek(1).value().type != OPEN_PARENTHESIS)
        Log("[Parsing Exit] Expected `(`");

    consume(); // exit
    consume(); // (

    std::optional<Node::IntExprNode> parsedResult = ParseIntExpr();
    if (!parsedResult.has_value())
        Log("[Parsing Exit] Expected an Expression");

    if (!peek().has_value() || peek().value().type != CLOSE_PARENTHESIS)
        Log("[Parsing Exit] Expected `)`");

    consume(); // )

    Node::IntExprNode exprNode(parsedResult.value());
    return Node::ExitNode(exprNode);
}