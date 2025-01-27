#include<iostream>
#include<fstream>

class WinApiGenerator
{
private:
    std::ofstream& outStream;
public:
    WinApiGenerator(std::ofstream& outStream) : outStream(outStream)
    {

    }
    void Prototypes() const
    {
        outStream << '\n';
        outStream << "    GetStdHandle PROTO, nStdHandle:DWORD\n";
        outStream << "    WriteConsoleA PROTO, handle:DWORD, lpBuffer:PTR BYTE, nNumberOfCharsToWrite:DWORD, lpNumberOfCharsWritten:PTR DWORD, lpReserved:PTR DWORD\n";
        outStream << '\n';

    }

    void CallWriteConsole(const std::string& indent) const
    {
        outStream << '\n';
        outStream << indent << "    CALL StringLength\n";
        outStream << indent << "    MOV ECX, EAX\n";
        outStream << indent << "    INVOKE GetStdHandle, -11\n";
        outStream << indent << "    INVOKE WriteConsoleA, EAX, EDX, ECX, bytesWritten, 0\n";
        outStream << '\n';
    }

    void StringLengthPROC() const
    {
        outStream << '\n';
        outStream << "StringLength PROC USES EDX\n";
        outStream << "    MOV EAX, 0\n";
        outStream << "        StartL:\n";
        outStream << "        CMP Byte Ptr [EDX + EAX], 0\n";
        outStream << "        JE EndL\n";
        outStream << "            INC EAX\n";
        outStream << "        JMP StartL\n";
        outStream << "        EndL:\n";
        outStream << "        ret\n";
        outStream << "StringLength ENDP\n";
        outStream << '\n';
    }
};
