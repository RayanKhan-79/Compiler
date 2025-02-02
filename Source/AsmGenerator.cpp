#include"../Headers/AsmGenerator.hpp"

    Variable::Variable(size_t stackOffset, const std::string& name, bool isNull) : 
        stackOffset(stackOffset),
        name(name),
        isNull(isNull)
    {

    }

    void AsmGenerator::Comment(const std::string& comment, const std::string& indent)
    {
        if (commentsOFF)
            outStream << "\n";
        else
            outStream << "\n" << indent << ";;" << comment << "\n";
    } 
    void AsmGenerator::increaseIndent()
    {
        indent += "    ";
    }
    void AsmGenerator::decreaseIndent()
    {
        indent.pop_back();
        indent.pop_back();
        indent.pop_back();
        indent.pop_back();
    }
    std::string AsmGenerator::createIf()
    {
        outStream << indent << "    TEST EAX, EAX\n";
        outStream << indent << "    JZ IF_LABEL" << ifCount << "_0\n";

        increaseIndent();
        return "IF_LABEL" + std::to_string(ifCount++);
    }
    void AsmGenerator::createElif(const std::string& tag, size_t i)
    {
        outStream << indent << "    TEST EAX, EAX\n";
        outStream << indent << "    JZ " << tag << "_" << std::to_string(i) << '\n';
    }
    void AsmGenerator::endElif(const std::string& tag, size_t i)
    {
        decreaseIndent();
        outStream << indent << "    JMP END_" << tag << "\n";
        outStream << indent << "    " << tag << "_" << std::to_string(i) << ":\n";
        increaseIndent();
    }
    void AsmGenerator::endif(const std::string& tag, size_t i)
    {
        decreaseIndent();
        outStream << indent << "    END_" << tag << ":\n";
    }
    std::string AsmGenerator::StrartForLoop()
    {
        outStream << indent << "FOR_LOOP" << loopCount << "_S:\n";
        return "FOR_LOOP" + std::to_string(loopCount++);
    }
    void AsmGenerator::EndForLoop(const std::string& tag)
    {
        outStream << indent << tag << "_E:\n";
    }
    std::string AsmGenerator::StartLoop()
    {
        outStream << indent + "    L" << loopCount << ":\n";
        increaseIndent();
        return "L" + std::to_string(loopCount++);
    }
    void AsmGenerator::EndLoop(const std::string& tag)
    {
        decreaseIndent();
        outStream << indent + "    LOOP " + tag << '\n';
    }
    void AsmGenerator::push(const std::string& reg)
    {
        outStream << indent + "    PUSH " << reg << '\n';
        stack_pointer += 1;
    }
    void AsmGenerator::pop(const std::string& reg)
    {
        outStream << indent + "    POP " << reg << '\n';
        stack_pointer -= 1;
    }

    AsmGenerator::AsmGenerator(const Node::ProgNode& progNode, const std::filesystem::path outputfile) :
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

        AEG = new ExpressionGenerator(outStream,logicalTagCount, 
        [&](const std::string& reg) {this->push(reg);},
        [&](const std::string& reg) {this->pop(reg);},
        [&](const std::string& comment, const std::string& indent) {this->Comment(comment, indent);},
        [&](){return this->StartLoop();},
        [&](const std::string& tag) {this->EndLoop(tag);}
        );
    }

    void AsmGenerator::generateString(const Node::StringLitNode& node)
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

    void AsmGenerator::generatePrint(const Node::PrintNode& node)
    {

        struct Visitor
        {
            AsmGenerator* Gen;
            Visitor(AsmGenerator* Gen) : Gen(Gen) {};

            void operator()(const Node::StringLitNode& node) 
            {
                Gen->outStream << Gen->indent << "    MOV ESI, ESP\n";
                Gen->generateString(node.stringLit);
                Gen->outStream << Gen->indent << "    MOV EDX, ESP\n";
                Gen->winApi.CallWriteConsole(Gen->indent);
                Gen->outStream << Gen->indent << "    MOV ESP, ESI\n";
            }
            void operator()(const Node::StringIdentNode& node) 
            {
                const Variable* var = Gen->find(node.label.value);
                if (var == nullptr) 
                    Log("Undeclared Identifier " + node.label.value);

                if (var->isNull)
                    Log("Null Pointer Exception");

                Gen->outStream << Gen->indent << "    MOV EDX, [EBP - " << 4 * var->stackOffset << "]\n";
                Gen->winApi.CallWriteConsole(Gen->indent);
            }
            void operator()(const Node::StringExprNode& node)
            {
                std::visit(Visitor(Gen), node.expression);
            }
            void operator()(const Node::NullNode& node)
            {
                Log("Null Pointer Exception");
            }
            void operator()(const Node::IntExprNode& node)
            {
                Log("Not Implemented");
            }
        };

        std::visit(Visitor(this), node.output);
    }

    const Variable* AsmGenerator::find(const std::string& varName) const
    {
        for (const Variable& var : varMap)
            if (var.name == varName)
                return &var;

        return nullptr;
    }

    Variable* AsmGenerator::find(const std::string& varName)
    {
        for (Variable& var : varMap)
            if (var.name == varName)
                return &var;

        return nullptr;
    }

    void AsmGenerator::generateExpression(const std::vector<std::variant<Node::OperandNode, Node::OperatorNode>>& expression)
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

    void AsmGenerator::generateLetAsm(const Node::LetNode& letNode)
    {
        Comment("INT DECLARATION", indent);
        generateExpression(solver.ConvertToPostFix(&letNode.exprNode));
        if (find(letNode.identNode.label.value) != nullptr)
        {
            std::cerr << "Identifier " << letNode.identNode.label.value << " Redecleared\n";
            return;
        }
        varMap.push_back(Variable(stack_pointer, letNode.identNode.label.value, false));
    }

    void AsmGenerator::generateIfAsm(const Node::IfNode* ifnode)
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

    void AsmGenerator::generateExitAsm(const Node::ExitNode& exitNode)
    {
        Comment("EXIT", indent);
        generateExpression(solver.ConvertToPostFix(&exitNode.expr));
        pop("EAX");
        outStream << indent + "    MOV ESP, returnAddress\n";
        outStream << indent + "    RET\n";
    }

    void AsmGenerator::generateIntAssignment(const Node::IntAssignmentNode& node)
    {
        Comment("INT ASSIGNMENT", indent);
        generateExpression(solver.ConvertToPostFix(&node.right));
        this->pop("EAX");


        struct Visitor
        {
            AsmGenerator* Gen;
            const Variable* var;

            Visitor(AsmGenerator* Gen, const Variable* var) : Gen(Gen), var(var)
            {

            }
            void operator()(const Node::NormEqNode& normNode)
            {
                Gen->outStream << Gen->indent << "    MOV DWORD PTR [EBP - " << 4 * var->stackOffset << "], EAX\n";
            }
            void operator()(const Node::PlusEqNode& plusNode)
            {
                Gen->outStream << Gen->indent << "    ADD DWORD PTR [EBP - " << 4 * var->stackOffset << "], EAX\n";
            }
            void operator()(const Node::MinusEqNode& minusNode)
            {
                Gen->outStream << Gen->indent << "    SUB DWORD PTR [EBP - " << 4 * var->stackOffset << "], EAX\n";
            }
            void operator()(const Node::MulEqNode& mulNode)
            {
                Gen->outStream << Gen->indent << "    MOV EBX, DWORD PTR [EBP - " << 4 * var->stackOffset << "]\n";
                Gen->outStream << Gen->indent << "    XCHG EAX, EBX\n";
                Gen->outStream << Gen->indent << "    MUL EBX\n";
                Gen->outStream << Gen->indent << "    MOV DWORD PTR [EBP - " << 4 * var->stackOffset << "], EAX\n";
            }
            void operator()(const Node::DivEqNode& divNode)
            {
                Gen->outStream << Gen->indent << "    MOV EBX, DWORD PTR [EBP - " << 4 * var->stackOffset << "]\n";
                Gen->outStream << Gen->indent << "    XCHG EAX, EBX\n";
                Gen->outStream << Gen->indent << "    MOV EDX, 0\n";
                Gen->outStream << Gen->indent << "    DIV EBX\n";
                Gen->outStream << Gen->indent << "    MOV DWORD PTR [EBP - " << 4 * var->stackOffset << "], EAX\n";
            }
        };

        if (const Variable* var = this->find(node.left.label.value)) {
            std::visit(Visitor(this,var), node.assignVar);
            return;
        }

        Log("Undeclared Identifier " + node.left.label.value);
    }

    void AsmGenerator::generateStrAssignment(const Node::StringAssignmentNode& node)
    {

        struct Visitor
        {
            AsmGenerator* Gen;
            Variable* leftVar;
            Visitor(AsmGenerator* Gen, Variable* leftVar) : Gen(Gen), leftVar(leftVar) {};

            void operator()(const Node::StringIdentNode& strIdent)
            {
                if (const Variable* var = Gen->find(strIdent.label.value))
                {
                    if (leftVar->isNull == false)
                        Log("Reassignment Of A Non Null Pointer Would Cause A Memory Leak");
                    
                    leftVar->isNull = false;
                    Gen->outStream << "    MOV EDX, DWORD PTR [EBP - " << 4 * var->stackOffset << "]\n";
                }
            }
            void operator()(const Node::StringLitNode& strLit)
            {
                if (leftVar->isNull == false)
                    Log("Reassignment Of A Non Null Pointer Would Cause A Memory Leak");

                leftVar->isNull = false;
                Gen->generateString(strLit);
                Gen->outStream << "    MOV EDX, ESP\n";
                Gen->push("EDX");
            }
            void operator()(const Node::NullNode& node)
            {
                leftVar->isNull = true;
                Gen->outStream << "    MOV EDX, 0\n";
            }
        };

        Variable* var = this->find(node.left.label.value);
        if (var == nullptr)
            Log("Undeclared Identifier " + node.left.label.value);

        Comment("STRING ASSIGNMENT", indent);
        std::visit(Visitor(this, var), node.right.expression);
        outStream << "    MOV DWORD PTR [EBP - " << 4 * var->stackOffset << "], EDX\n";
    }

    void AsmGenerator::generateLetStringAsm(const Node::let_StringNode& node)
    {
        struct Visitor
        {
            AsmGenerator* gen;
            const std::string& identName;
            Visitor(AsmGenerator* gen, const std::string& identName) : gen(gen), identName(identName) {};

            void operator()(const Node::NullNode& node)
            {
                gen->outStream << "    MOV EDX, 0\n";
                gen->push("EDX");
                gen->outStream << "\n";
                Variable declaration(gen->stack_pointer, identName, true);
                gen->varMap.push_back(declaration);

            }
            void operator()(const Node::StringLitNode& node)
            {
                gen->generateString(node);
                gen->outStream << "    MOV EDX, ESP\n";
                gen->push("EDX");
                gen->outStream << '\n';                    
                Variable declaration(gen->stack_pointer, identName, false);
                gen->varMap.push_back(declaration);

            }
            void operator()(const Node::StringIdentNode& node)
            {
                if (const Variable* var = gen->find(node.label.value))
                {
                    gen->outStream << "    MOV EDX, DWORD PTR [EBP - " << 4 * var->stackOffset << "]\n";
                    gen->push("EDX");
                    gen->outStream << '\n';
                    Variable declaration(gen->stack_pointer, identName, false);
                    gen->varMap.push_back(declaration);
                    return;
                }
                Log("Undeclared Identifier");
            }
        };

        Comment("STRING DECLARATION", indent);
        std::visit(Visitor(this, node.identNode.label.value), node.strExpr.expression);
    }

    void AsmGenerator::generateForLoopASM(const Node::ForLoopNode* forloop)
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

    void AsmGenerator::generateRead(const Node::ReadNode& node)
    {

        Log("Reading Not Implemented");

        outStream << "    MOV EAX, console_pointer\n";
        outStream << "    CMP EAX, 0\n";
        outStream << "    JNE DONT_Call_function\n";
        outStream << "        MOV ECX, LENGTHOF ConsoleInput\n";
        outStream << "        MOV EDX, OFFSET ConsoleInput\n";
        outStream << "        INVOKE GetStdHandle, -10\n";
        outStream << "        INVOKE ReadConsoleA, Eax, Edx, Ecx, OFFSET charsRead, 0\n";
        outStream << "    DONT_CALL_function:\n";

        struct Visitor
        {
            AsmGenerator* Gen;
            Visitor(AsmGenerator* Gen) : Gen(Gen) {}

            void operator()(const Node::StringIdentNode& node)
            {
                if (const Variable* var = Gen->find(node.label.value)) {

                    if (var->isNull == false)
                        Log("Writing To A Non-Null Pointer Will Cause A Memory Leak");

                    Gen->outStream << "    LEA EDX, ConsoleInput\n";
                    Gen->outStream << "    ADD EDX, console_pointer\n";
                    Gen->outStream << "    MOV EBX, ' '\n";
                    Gen->outStream << "    CALL StringLength\n";
                    Gen->outStream << "    SUB ESP, EAX + 1\n";
                }
            }
            void operator()(const Node::IntIdentNode& node)
            {
                Log("Not Implemented");
            }
        };



        std::visit(Visitor(this),node.ident);

        outStream << "    MOV EAX, console_pointer\n";
        outStream << "    CMP EAX, charsRead\n";
        outStream << "    JL DONT_RESET\n";
        outStream << "        MOV console_pointer, 0\n";
        outStream << "    DONT_RESET:\n";
    }


    void AsmGenerator::generateProgramAsm()
    {
        outStream << ".386\n";
        outStream << ".model flat,stdcall\n";
        outStream << ".data\n";
        winApi.Prototypes();
        outStream << "    ConsoleInput DB 1000 DUP(0)\n";
        outStream << "    console_pointer DWORD 0\n";
        outStream << "    charsRead DWORD 0\n";
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
        winApi.StrCpyPROC();
        outStream << "end main";
    }

    void AsmGenerator::generateStmt(const Node::StmtNode& stmtNode)
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
            void operator()(const Node::ReadNode& node)
            {
                gen->generateRead(node);
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

    void AsmGenerator::generateScope(const Node::ScopeNode* scopeNode)
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

    AsmGenerator::~AsmGenerator()
    {
        outStream.close();
        delete AEG;
        AEG = nullptr;
    }