#pragma once
#include"Parser.hpp"
#include"ExpressionSolver.hpp"
#include"WinApiGenerator.hpp"
#include"ASMExpressionGenerator.hpp"
#include<unordered_map>

#define commentsOFF 0

struct Variable
{
    size_t stackOffset;
    std::string name;

    Variable(size_t stackOffset, const std::string& name) : 
        stackOffset(stackOffset),
        name(name)
    {

    }
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

    ExpressionSolver solver;
    WinApiGenerator winApi;
    ASMExpressionGenerator* AEG;

private:
    void Comment(const std::string& comment, const std::string& indent)
    {
        if (commentsOFF)
            outStream << "\n";
        else
            outStream << "\n" << indent << ";;" << comment << "\n";
    } 
    void increaseIndent()
    {
        indent += "    ";
    }
    void decreaseIndent()
    {
        indent.pop_back();
        indent.pop_back();
        indent.pop_back();
        indent.pop_back();
    }
    std::string createIf()
    {
        outStream << indent << "    TEST EAX, EAX\n";
        outStream << indent << "    JZ IF_LABEL" << ifCount << "_0\n";

        increaseIndent();
        return "IF_LABEL" + std::to_string(ifCount++);
    }
    void createElif(const std::string& tag, size_t i = 0)
    {
        outStream << indent << "    TEST EAX, EAX\n";
        outStream << indent << "    JZ " << tag << "_" << std::to_string(i) << '\n';
    }
    void endElif(const std::string& tag, size_t i = 0)
    {
        decreaseIndent();
        outStream << indent << "    JMP END_" << tag << "\n";
        outStream << indent << "    " << tag << "_" << std::to_string(i) << ":\n";
        increaseIndent();
    }
    void endif(const std::string& tag, size_t i)
    {
        decreaseIndent();
        outStream << indent << "    END_" << tag << ":\n";
    }
    std::string StrartForLoop()
    {
        outStream << indent << "FOR_LOOP" << loopCount << "_S:\n";
        return "FOR_LOOP" + std::to_string(loopCount++);
    }
    void EndForLoop(const std::string& tag)
    {
        outStream << indent << tag << "_E:\n";
    }
    std::string StartLoop()
    {
        outStream << indent + "    L" << loopCount << ":\n";
        increaseIndent();
        return "L" + std::to_string(loopCount++);
    }
    void EndLoop(const std::string& tag)
    {
        decreaseIndent();
        outStream << indent + "    LOOP " + tag << '\n';
    }
    void push(const std::string& reg)
    {
        outStream << indent + "    PUSH " << reg << '\n';
        stack_pointer += 1;
    }
    void pop(const std::string& reg)
    {
        outStream << indent + "    POP " << reg << '\n';
        stack_pointer -= 1;
    }
public:
    AsmGenerator(const Node::ProgNode& progNode, const std::filesystem::path outputfile) :
        progNode(progNode), 
        outputfile(outputfile), 
        outStream(outputfile),
        winApi(outStream),
        stack_pointer(0),
        loopCount(0),
        ifCount(0),
        logicalTagCount(0),
        AEG(nullptr)
    {
        if (!outStream.is_open())
            exit(EXIT_FAILURE);

        base_pointer.push(0);

        AEG = new ASMExpressionGenerator(outStream,logicalTagCount, 
        [&](const std::string& reg) {this->push(reg);},
        [&](const std::string& reg) {this->pop(reg);},
        [&](const std::string& comment, const std::string& indent) {this->Comment(comment, indent);},
        [&](){return this->StartLoop();},
        [&](const std::string& tag) {this->EndLoop(tag);}
        );
    }

    void generateString(const Node::StringLitNode& node)
    {
        const std::string& literal = node.stringLit.value;

        // RAYA N000
        // HA00
        // MEPH ISTO PLI0 
        // MEIN kitt er
        int N = literal.length();
        switch (literal.length()%4)
        {

            case 1:
                outStream << indent + "    MOV AL, 0\n";
                outStream << indent + "    MOV AH, 0\n";
                outStream << indent + "    PUSH AX\n";

                outStream << indent + "    MOV AL, " << int(literal[N - 1]) << "\n";
                outStream << indent + "    MOV AH, 0\n";
                outStream << indent + "    PUSH AX\n";
                break;
            case 2:
                outStream << indent + "    MOV AL, 0\n";
                outStream << indent + "    MOV AH, 0\n";
                outStream << indent + "    PUSH AX\n";

                outStream << indent + "    MOV AL, " << int(literal[N - 2]) << "\n";
                outStream << indent + "    MOV AH, " << int(literal[N - 1]) << "\n";
                outStream << indent + "    PUSH AX\n";
                break;
            case 3:
                outStream << indent + "    MOV AL, " << int(literal[N - 1]) << "\n";
                outStream << indent + "    MOV AH, 0\n";
                outStream << indent + "    PUSH AX\n";
                
                outStream << indent + "    MOV AL, " << int(literal[N - 3]) << "\n";
                outStream << indent + "    MOV AH, " << int(literal[N - 2]) << "\n";
                outStream << indent + "    PUSH AX\n";
                break;
            default: 
                outStream << indent + "    MOV EAX, 0\n";
                outStream << indent + "    PUSH EAX\n";
                break;
        }
        stack_pointer += 1;

        // int N = strRef.length() - (strRef.length() % 4);
        N = N - (N % 4);
        for (int i = N - 1; i >= 0; i -= 4)
        {
            outStream << indent + "    MOV AL, " << int(literal[i - 1]) << "\n";
            outStream << indent + "    MOV AH, " << int(literal[i]) << "\n";
            outStream << indent + "    PUSH AX\n";

            outStream << indent + "    MOV AL, " << int(literal[i - 3]) << "\n";
            outStream << indent + "    MOV AH, " << int(literal[i - 2]) << "\n";
            outStream << indent + "    PUSH AX\n";
            stack_pointer += 1;
        }

    }

    void generatePrint(const Node::PrintNode& node)
    {

        struct Visitor
        {
            AsmGenerator* gen;
            Visitor(AsmGenerator* gen) : gen(gen) {};

            void operator()(const Node::StringLitNode& node) 
            {
                gen->outStream << gen->indent << "    MOV ESI, ESP\n";
                gen->generateString(node.stringLit);
                gen->outStream << gen->indent << "    MOV EDX, ESP\n";
                gen->winApi.CallWriteConsole(gen->indent);
                gen->outStream << gen->indent << "    MOV ESP, ESI\n";
            }
            void operator()(const Node::StringIdentNode& node) 
            {
                if (const Variable* var = gen->find(node.label.value)) 
                {
                    gen->outStream << gen->indent << "    MOV EDX, [EBP - " << 4 * var->stackOffset << "]\n";
                    gen->winApi.CallWriteConsole(gen->indent);
                    return;
                }

                std::cerr << "Undeclared Identifier " << node.label.value << "\n";
                exit(EXIT_FAILURE);
            }
        };

        std::visit(Visitor(this), node.strExpr.expression);
    }

    const Variable* find(const std::string& varName) const
    {
        for (const Variable& var : varMap)
            if (var.name == varName)
                return &var;

        return nullptr;
    }

    void generateExpression(const std::vector<std::variant<Node::OperandNode, Node::OperatorNode>>& expression)
    {
        struct Visitor
        {
            AsmGenerator* gen;
            Visitor(AsmGenerator* gen) : gen(gen)
            {

            }
            void operator()(const Node::OperatorNode& node)
            {
                switch (node.operatorToken.type)
                {
                    case PLUS: gen->AEG->Addition(gen->indent); break;

                    case MINUS: gen->AEG->Subtraction(gen->indent); break;
                    
                    case ASTERIK: gen->AEG->Multiplication(gen->indent); break;
                    
                    case SLASH: gen->AEG->Division(gen->indent); break;
                    
                    case POW: gen->AEG->Power(gen->indent); break;
                    
                    case LOGICAL_AND: gen->AEG->LogicalAnd(gen->indent); break;
                    
                    case LOGICAL_OR: gen->AEG->LogicalOr(gen->indent); break;
                    
                    case LOGICAL_NOT: gen->AEG->LogicalNot(gen->indent); break;

                    case LOGICAL_XOR: gen->AEG->LogicalXor(gen->indent); break;
                }
                gen->push("EAX");
            }
            void operator()(const Node::OperandNode& node) const
            {
                std::visit(*this, node.exprVar);
            }
            void operator()(const Node::IntLitNode& node) const
            {
                gen->outStream << gen->indent + "    MOV EAX, " << node.intLit.value << '\n';
                gen->push("EAX");
            }
            void operator()(const Node::IntIdentNode& node) const
            {
                if (const Variable* var = gen->find(node.label.value))
                {
                    gen->outStream << gen->indent + "    MOV EAX, DWORD PTR [EBP - " << 4 * (var->stackOffset) << "]\n";
                    gen->push("EAX");
                    return;
                }

                std::cerr << "Undeclared Identifier " << node.label.value << "\n";
            }
        };

        Visitor visitor(this);
        for (const std::variant<Node::OperandNode, Node::OperatorNode>& token : expression)
            std::visit(visitor, token);
    }

    void generateLetAsm(const Node::LetNode& letNode)
    {
        Comment("INT DECLARATION", indent);
        generateExpression(solver.ConvertToPostFix(&letNode.exprNode));
        if (find(letNode.identNode.label.value) != nullptr)
        {
            std::cerr << "Identifier " << letNode.identNode.label.value << " Redecleared\n";
            return;
        }
        varMap.push_back(Variable(stack_pointer, letNode.identNode.label.value));
    }

    void generateIfAsm(const Node::IfNode* ifnode)
    {
        Comment("IF_SCOPE", indent);
        this->generateExpression(solver.ConvertToPostFix(&ifnode->boolean));
        this->pop("EAX");
        Comment("CONDITION TEST", indent);
        std::string tag = this->createIf();
        this->generateStmt(ifnode->ifStmt);
        this->endElif(tag, 0);


        for (size_t i = 0; i < ifnode->elifStmts.size(); i++)
        {
            decreaseIndent();
            this->generateExpression(solver.ConvertToPostFix(&ifnode->elifStmts[i].boolean));
            this->pop("EAX");
            Comment("CONDITION TEST", indent);
            this->createElif(tag, i + 1);
            increaseIndent();
            this->generateStmt(ifnode->elifStmts[i].stmt);
            this->endElif(tag, i + 1);

        }

        if (ifnode->elseStmt.has_value())
        {
            Comment("ELSE SCOPE", indent);
            this->generateStmt(ifnode->elseStmt.value());
        }
    
        this->endif(tag, ifnode->elifStmts.size());
    }

    void generateExitAsm(const Node::ExitNode& exitNode)
    {
        Comment("EXIT", indent);
        generateExpression(solver.ConvertToPostFix(&exitNode.expr));
        pop("EAX");
        outStream << indent + "    MOV ESP, returnAddress\n";
        outStream << indent + "    RET\n";
    }

    void generateIntAssignment(const Node::IntAssignmentNode& node)
    {
        Comment("INT ASSIGNMENT", indent);
        generateExpression(solver.ConvertToPostFix(&node.right));
        this->pop("EAX");

        if (const Variable* var = this->find(node.left.label.value))
        {
            outStream << indent << "    MOV DWORD PTR [EBP - " << 4 * var->stackOffset << "], EAX\n";
            return;
        }

        std::cerr << "Undeclared Identifier " << node.left.label.value << '\n';
        exit(EXIT_FAILURE);
    }

    void generateStrAssignment(const Node::StringAssignmentNode& node)
    {

        struct Visitor
        {
            AsmGenerator* gen;
            Visitor(AsmGenerator* gen) : gen(gen) {};

            void operator()(const Node::StringIdentNode& strIdent)
            {
                if (const Variable* var = gen->find(strIdent.label.value))
                {
                    gen->outStream << "    MOV EDX, DWORD PTR [EBP - " << 4 * var->stackOffset << "]\n";
                    return;
                }
            }
            void operator()(const Node::StringLitNode& strLit)
            {
                gen->generateString(strLit);
                gen->outStream << "    MOV EDX, ESP\n";
                gen->push("EDX");
            }
        };

        const Variable* var = this->find(node.left.label.value);
        if (var == nullptr)
        {
            std::cerr << "Undeclared Identifier " << node.left.label.value << '\n';
            exit(EXIT_FAILURE);
        }

        Comment("STRING ASSIGNMENT", indent);
        std::visit(Visitor(this), node.right.expression);
        outStream << "    MOV DWORD PTR [EBP - " << 4 * var->stackOffset << "], EDX\n";
    }

    void generateLetStringAsm(const Node::let_StringNode& node)
    {
        struct Visitor
        {
            AsmGenerator* gen;
            Visitor(AsmGenerator* gen) : gen(gen) {};

            void operator()(const Node::StringLitNode& node)
            {
                gen->generateString(node);
                gen->outStream << "    MOV EDX, ESP\n";
                gen->push("EDX");
                gen->outStream << '\n';
            }
            void operator()(const Node::StringIdentNode& node)
            {
                if (const Variable* var = gen->find(node.label.value))
                {
                    gen->outStream << "    MOV EDX, DWORD PTR [EBP - " << 4 * var->stackOffset << "]\n";
                    gen->push("EDX");
                    gen->outStream << '\n';                
                    return;
                }

                std::cerr << "Undeclared Identifier " << node.label.value << '\n';
                exit(EXIT_FAILURE);
            }
        };
        
        Comment("STRING DECLARATION", indent);
        std::visit(Visitor(this), node.strExpr.expression);
        varMap.push_back(Variable(stack_pointer, node.identNode.label.value));
    }

    void generateForLoopASM(const Node::ForLoopNode* forloop)
    {
        Comment("FOR LOOP", indent);
        this->generateLetAsm(forloop->initialization);
        std::string tag = this->StrartForLoop();
        this->generateExpression(solver.ConvertToPostFix(&forloop->condition));
        outStream << indent << "    TEST EAX, EAX\n";
        outStream << indent << "    JZ " << tag << "_E\n";
        this->generateStmt(forloop->loopBody);
        this->generateIntAssignment(forloop->updation);
        outStream << indent << "    JMP " << tag << "_S\n";
        outStream << indent << "    " << tag << "_E:\n";
    }

    void generateProgramAsm()
    {
        outStream << ".386\n";
        outStream << ".model flat,stdcall\n";
        outStream << ".data\n";
        winApi.Prototypes();
        outStream << "    returnAddress DWORD 0\n";
        outStream << "    bytesWritten DWORD 0\n";
        outStream << ".code\n";
        outStream << "main PROC\n";
        outStream << "    MOV returnAddress, ESP\n\n";
        outStream << "    PUSH EBP\n";
        outStream << "    MOV EBP, ESP\n\n";
        
        for (const Node::StmtNode& stmt : progNode.statements)
            this->generateStmt(stmt);

        outStream << "\n";
        outStream << "    MOV EAX, 0\n";
        outStream << "    MOV ESP, returnAddress\n";
        outStream << "    RET\n";
        outStream << "main ENDP\n";
        winApi.StringLengthPROC();
        outStream << "end main";
    }

    void generateStmt(const Node::StmtNode& stmtNode)
    {
        struct Visitor
        {
            AsmGenerator* gen;
            Visitor(AsmGenerator* gen) : gen(gen)
            {

            }
            void operator()(const Node::ExitNode& node)
            {
                gen->generateExitAsm(node);
                gen->outStream << '\n';
            }
            void operator()(const Node::LetNode& node)
            {
                gen->generateLetAsm(node);
                gen->outStream << '\n';
            }
            void operator()(const Node::PrintNode& node)
            {
                gen->generatePrint(node);
                gen->outStream << '\n';
            }
            void operator()(const Node::let_StringNode& node)
            {
                gen->generateLetStringAsm(node);
                gen->outStream << '\n';
            }
            void operator()(const Node::IntAssignmentNode& node)
            {
                gen->generateIntAssignment(node);
                gen->outStream << '\n';
            }
            void operator()(const Node::StringAssignmentNode& node)
            {
                gen->generateStrAssignment(node);
                gen->outStream << '\n';
            }
            void operator()(const Node::IfNode* node)
            {
                gen->generateIfAsm(node);
                gen->outStream << '\n';
            }
            void operator()(const Node::ScopeNode* node)
            {
                gen->generateScope(node);
                gen->outStream << '\n';
            }
            void operator()(const Node::ForLoopNode* node)
            {
                gen->generateForLoopASM(node);
                gen->outStream << '\n';
            }
        };

        std::visit(Visitor(this), stmtNode.stmtVar);
    }

    void generateScope(const Node::ScopeNode* scopeNode)
    {
        base_pointer.push(stack_pointer);
        Comment("BEGIN_SCOPE", indent);

        this->increaseIndent();
    
        for (const Node::StmtNode& stmt : scopeNode->statements)
            this->generateStmt(stmt);

        this->decreaseIndent();

        outStream << "    ADD ESP, " << 4 * (stack_pointer - base_pointer.top()) << "\n";
        Comment("END_SCOPE", indent);

        while (stack_pointer != base_pointer.top())
        {
            varMap.pop_back();
            stack_pointer -= 1;
        }

        // stack_pointer = base_pointer.top();
        base_pointer.pop();
    }

    ~AsmGenerator()
    {
        outStream.close();
        delete AEG;
        AEG = nullptr;
    }
};