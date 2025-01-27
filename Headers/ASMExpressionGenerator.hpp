#include<iostream>
#include<fstream>
#include<string>
#include<functional>

class ASMExpressionGenerator
{
private:
    std::ofstream& outStream;
    int& logicalTagCount;
    std::function<void(const std::string&)> push;
    std::function<void(const std::string&)> pop;
    std::function<void(const std::string&, const std::string&)> Comment;
    std::function<std::string()> StartLoop;
    std::function<void(const std::string&)> EndLoop;
public:
    ASMExpressionGenerator(std::ofstream& outStream, int& logicalTagCount, 
    std::function<void(const std::string&)> push, 
    std::function<void(const std::string&)> pop,
    std::function<void(const std::string&, const std::string&)> Comment,
    std::function<std::string()> StartLoop,
    std::function<void(const std::string&)> EndLoop
    ) :
        outStream(outStream),
        logicalTagCount(logicalTagCount),
        push(push),
        pop(pop),
        Comment(Comment),
        StartLoop(StartLoop),
        EndLoop(EndLoop)
    {

    }
    void Addition(const std::string& indent)
    {
        this->pop("EBX");
        this->pop("EAX");
        outStream << indent << "    ADD EAX, EBX\n";
    }
    void Subtraction(const std::string& indent)
    {
        this->pop("EBX");
        this->pop("EAX");
        outStream << indent << "    SUB EAX, EBX\n"; 
    }
    void Multiplication(const std::string& indent)
    {
        this->pop("EBX");
        this->pop("EAX");
        outStream << indent + "    MUL EBX\n";
    }
    void Division(const std::string& indent)
    {
        this->pop("EBX");
        this->pop("EAX");
        outStream << indent + "    MOV EDX, 0\n";
        outStream << indent + "    DIV EBX\n";
    }
    void Power(const std::string& indent)
    {
        this->Comment("POWER", indent);
        this->pop("EBX");
        this->pop("EAX");
        outStream << indent << "    MOV ECX, EBX\n";
        outStream << indent << "    MOV EBX, EAX\n";
        outStream << indent << "    MOV EAX, 1\n";
        std::string tag = this->StartLoop();
        outStream << indent << "    MUL EBX\n";
        this->EndLoop(tag);
    }
    void LogicalAnd(const std::string& indent)
    {
        this->Comment("AND", indent);
        this->pop("EBX");
        this->pop("EAX");
        outStream << indent << "    TEST EAX, EAX\n";
        outStream << indent << "    JZ isFalse" << logicalTagCount << "\n";
        outStream << indent << "    TEST EBX, EBX\n";
        outStream << indent << "    JZ isFalse" << logicalTagCount << "\n";
        outStream << indent << "    MOV EAX, 1\n";
        outStream << indent << "    JMP EndAnd" << logicalTagCount << "\n";
        outStream << indent << "    isFalse" << logicalTagCount << ":\n";
        outStream << indent << "    MOV EAX, 0\n";
        outStream << indent << "    EndAnd" << logicalTagCount << ":\n";
        
        logicalTagCount += 1;
    }
    void LogicalOr(const std::string& indent)
    {
        this->Comment("OR", indent);
        this->pop("EBX");
        this->pop("EAX");
        outStream << indent << "    TEST EAX, EAX\n";
        outStream << indent << "    JNZ isTrue" << logicalTagCount << "\n";
        outStream << indent << "    TEST EBX, EBX\n";
        outStream << indent << "    JNZ isTrue" << logicalTagCount << "\n";
        outStream << indent << "    MOV EAX, 0\n";
        outStream << indent << "    JMP EndOr" << logicalTagCount << "\n";
        outStream << indent << "    isTrue" << logicalTagCount << ":\n";
        outStream << indent << "    MOV EAX, 1\n";
        outStream << indent << "    EndOr" << logicalTagCount << ":\n";
        
        logicalTagCount += 1;
    }
    void LogicalXor(const std::string& indent)
    {
        this->Comment("XOR", indent);
        this->pop("EBX");
        this->pop("EAX");
        /*
            MOV ECX, 0
            TEST EAX, EAX
            JZ l1
                MOV ECX, 1
            l1:
                MOV ECX, 0
            TEST EBX, EBX
            JZ l2
                TEST ECX, ECX
                JZ isTrue
                JMP isFalse     
            l2:
                TEST ECX, ECX
                JZ isFalse
                JMP isTrue
            isTrue:
                MOV EAX, 1
                JMP EndXor
            isFalse:
                MOV EAX, 0
                JMP EndXor
            EndXor:

        */
        outStream << indent << "    MOV ECX, 0\n";
        outStream << indent << "    TEST EAX, EAX\n";
        outStream << indent << "    JZ XOR_CHECK_F" << logicalTagCount << "\n";
        outStream << indent << "        MOV ECX, 1\n";
        outStream << indent << "        JMP XOR_TEST_S" << logicalTagCount << "\n";
        outStream << indent << "    XOR_CHECK_F" << logicalTagCount << ":\n";
        outStream << indent << "        MOV ECX, 0\n";
        outStream << indent << "        JMP XOR_TEST_S" << logicalTagCount << "\n";
        outStream << indent << "    XOR_TEST_S" << logicalTagCount << ":\n";
        outStream << indent << "    TEST EBX, EBX\n";
        outStream << indent << "    JZ XOR_CHECK_S" << logicalTagCount <<"\n";
        outStream << indent << "        TEST ECX, ECX\n";
        outStream << indent << "        JZ XORisTrue" << logicalTagCount << "\n";
        outStream << indent << "        JMP XORisFalse" << logicalTagCount << "\n";
        outStream << indent << "    XOR_CHECK_S" << logicalTagCount << ":\n";
        outStream << indent << "        TEST ECX, ECX\n";
        outStream << indent << "        JZ XORisFalse" << logicalTagCount << "\n";
        outStream << indent << "        JMP XORisTrue" << logicalTagCount << "\n";
        outStream << indent << "    XORisTrue" << logicalTagCount << ":\n";
        outStream << indent << "        MOV EAX, 1\n";
        outStream << indent << "        JMP EndXor" << logicalTagCount << "\n";
        outStream << indent << "    XORisFalse" << logicalTagCount << ":\n";
        outStream << indent << "        MOV EAX, 0\n";
        outStream << indent << "        JMP EndXor" << logicalTagCount << "\n";
        outStream << indent << "    EndXor" << logicalTagCount << ":\n";
        
        logicalTagCount += 1;
    }
    void LogicalNot(const std::string& indent)
    {
        this->Comment("NOT", indent);
        this->pop("EAX");
        outStream << indent << "    CMP EAX, 0\n";
        outStream << indent << "    JE SetOne" << logicalTagCount << "\n";
        outStream << indent << "    MOV EAX, 0\n";
        outStream << indent << "    JMP EndNot" << logicalTagCount << "\n";
        outStream << indent << "    SetOne" << logicalTagCount << ":\n";
        outStream << indent << "    MOV EAX, 1\n";
        outStream << indent << "    EndNot" << logicalTagCount << ":\n";

        logicalTagCount += 1;
    }

};
