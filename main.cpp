#include<iostream>
#include<fstream>
#include<filesystem>
#include<string>
#include<algorithm>
#include "./Headers/Tokenizer.hpp"
#include "./Headers/Parser.hpp"
#include "./Headers/AsmGenerator.hpp"

std::string readCppFile(const std::filesystem::path& filepath)
{
    std::ifstream inptStream(filepath);
    if (inptStream.is_open() == false)
    {
        std::cerr << "Invalid File Path\n";
        exit(EXIT_FAILURE);
    }

    std::string str;
    std::getline(inptStream, str, '\0');

    return str;
}

int main(int argc, char* argv[])
{
    std::filesystem::path filepath = "example.hy";
    std::string str = readCppFile(filepath);
    
    Tokenizer tokenizer(str);
    std::vector<Token> tokens = tokenizer.Tokenize();
    
    Parser parser(tokens);
    Node::ProgNode program = parser.ParseProgram().value();

    AsmGenerator asmGenerator(program, "C:/Users/rayan.DESKTOP-59MFCDO/Desktop/FP/asm/example.asm");
    asmGenerator.generateProgramAsm();

    AsmGenerator local_asmGenerator(program, "example.asm");
    local_asmGenerator.generateProgramAsm();

    for (Token& token : tokens)
        std::cout << Token::EnumToString(token.type) << ' ' << token.value << '\n';

    std::cout << "done\n";
}