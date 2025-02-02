#include"../Headers/WinApiGenerator.hpp"

WinApiGenerator::WinApiGenerator(std::ofstream& outStream) : outStream(outStream)
{

}
void WinApiGenerator::Prototypes() const
{
    outStream << '\n';
    outStream << "    GetStdHandle PROTO, nStdHandle:DWORD\n";
    outStream << "    WriteConsoleA PROTO, handle:DWORD, lpBuffer:PTR BYTE, nNumberOfCharsToWrite:DWORD, lpNumberOfCharsWritten:PTR DWORD, lpReserved:PTR DWORD\n";
    outStream << "    ReadConsoleA PROTO, handle:DWORD, lpBuffer:PTR BYTE, nNumberOfCharsToRead:DWORD, lpNumberOfCharsRead:PTR DWORD, lpReserved:PTR DWORD\n";
    outStream << '\n';

}
void WinApiGenerator::StrCpyPROC() const
{
    outStream << "    StrCpy PROC USES ESI EDX EBX EAX\n";
    outStream << "    MOV ECX, 0\n";
    outStream << "    L1:\n";
    outStream << "        CMP Byte Ptr [ESI], BL\n";
    outStream << "        JE _return\n";
    outStream << "        CMP Byte Ptr [ESI], 0\n";
    outStream << "        JE _return\n";
    outStream << "        CMP Byte Ptr [ESI], 0dh\n";
    outStream << "        JE _return\n";
    outStream << "        CMP Byte Ptr [ESI], 0ah\n";
    outStream << "        JE _return\n";
    outStream << "\n";
    outStream << "        MOV AL, Byte Ptr[ESI]\n";
    outStream << "        MOV Byte Ptr[EDX], AL\n";
    outStream << "        INC ESI\n";
    outStream << "        INC EDX\n";
    outStream << "        INC ECX\n";
    outStream << "        JMP L1\n";
    outStream << "    _return:\n";
    outStream << "        ret\n";    
    outStream << "    StrCpy ENDP\n";    
}
void WinApiGenerator::CallWriteConsole(const std::string& indent) const
{
    outStream << '\n';
    outStream << indent << "    CALL StringLength\n";
    outStream << indent << "    MOV ECX, EAX\n";
    outStream << indent << "    INVOKE GetStdHandle, -11\n";
    outStream << indent << "    INVOKE WriteConsoleA, EAX, EDX, ECX, bytesWritten, 0\n";
    outStream << '\n';
}
void WinApiGenerator::StringLengthPROC() const
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