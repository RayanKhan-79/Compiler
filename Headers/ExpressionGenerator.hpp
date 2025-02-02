#pragma once
#include<iostream>
#include<fstream>
#include<string>
#include<functional>

class ExpressionGenerator
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
    ExpressionGenerator(std::ofstream& outStream, int& logicalTagCount, 
    std::function<void(const std::string&)> push, 
    std::function<void(const std::string&)> pop,
    std::function<void(const std::string&, const std::string&)> Comment,
    std::function<std::string()> StartLoop,
    std::function<void(const std::string&)> EndLoop
    );
    void Addition(const std::string& indent);
    void Subtraction(const std::string& indent);
    void Multiplication(const std::string& indent);
    void Division(const std::string& indent);
    void Power(const std::string& indent);
    void LogicalAnd(const std::string& indent);
    void LogicalOr(const std::string& indent);
    void LogicalXor(const std::string& indent);
    void LogicalNot(const std::string& indent);
};
