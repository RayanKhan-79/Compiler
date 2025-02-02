g++ -c Source\AsmGenerator.cpp -o Build\AsmGenerator.o
g++ -c Source\ExpressionGenerator.cpp -o Build\ExpressionGenerator.o
g++ -c Source\ExpressionSolver.cpp -o Build\ExpressionSolver.o            
g++ -c Source\WinApiGenerator.cpp -o Build\WinApiGenerator.o 
g++ -c Source\Parser.cpp -o Build\Parser.o
g++ -c Source\Tokenizer.cpp -o Build\Tokenizer.o
g++ -c Source\Compiler.cpp -o Build\Compiler.o
g++ -c main.cpp -o Build\main.o
g++ Build\main.o Build\Tokenizer.o Build\Parser.o Build\WinApiGenerator.o Build\ExpressionSolver.o Build\ExpressionGenerator.o Build\AsmGenerator.o Build\Compiler.o -o Build\main.exe