#pragma once
#include "Nodes.hpp"
#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "ExpressionSolver.hpp"
#include "ExpressionGenerator.hpp"
#include "WinApiGenerator.hpp"
#include "AsmGenerator.hpp"

class Compiler
{
private:
    const std::filesystem::path& sourceFile;
    const std::string sourceCode;
public:
    std::string readFile();
    Compiler(const std::filesystem::path& sourceFile);
    void CompileTo(const std::filesystem::path& dest);
};