#pragma once
#include<iostream>
#include<fstream>

class WinApiGenerator
{
private:
    std::ofstream& outStream;
public:
    WinApiGenerator(std::ofstream& outStream);
    void Prototypes() const;
    void CallWriteConsole(const std::string& indent) const;
    void StrCpyPROC() const;
    void StringLengthPROC() const;
};
