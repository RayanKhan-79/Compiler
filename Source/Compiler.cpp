#include"../Headers/Compiler.hpp"

std::string Compiler::readFile()
{
    std::ifstream inptStream(sourceFile);
    if (inptStream.is_open() == false)
        Log("Invalid File Path");

    std::string str;
    std::getline(inptStream, str, '\0');
    return str;
}
Compiler::Compiler(const std::filesystem::path& sourceFile) :
    sourceFile(sourceFile),
    sourceCode(readFile())
{
    std::cout  << '\n' << sourceCode << '\n';
}
void Compiler::CompileTo(const std::filesystem::path& dest)
{
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.Tokenize();

    std::cout << "!\n";

    for (Token& token : tokens)
        std::cout << Token::EnumToString(token.type) << ' ' << token.value << '\n';

    Parser parser(tokens);
    Node::ProgNode programNode = parser.ParseProgram();
    
    AsmGenerator generator(programNode, dest);
    generator.generateProgramAsm();
}