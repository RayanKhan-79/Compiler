#include "../Headers/Compiler.hpp"

int main(int argc, char* argv[])
{
    // if (argc == 1)
    //     Log("Missing Argument");

    std::filesystem::path filepath("../example.hy");
    Compiler compiler(filepath);
    compiler.CompileTo("C:/Users/rayan.DESKTOP-59MFCDO/Desktop/FP/asm/example.asm");
    compiler.CompileTo("../example.asm");
    std::cout << "done\n";
}