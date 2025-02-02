.386
.model flat,stdcall
.data

    GetStdHandle PROTO, nStdHandle:DWORD
    WriteConsoleA PROTO, handle:DWORD, lpBuffer:PTR BYTE, nNumberOfCharsToWrite:DWORD, lpNumberOfCharsWritten:PTR DWORD, lpReserved:PTR DWORD
    ReadConsoleA PROTO, handle:DWORD, lpBuffer:PTR BYTE, nNumberOfCharsToRead:DWORD, lpNumberOfCharsRead:PTR DWORD, lpReserved:PTR DWORD

    ConsoleInput DB 1000 DUP(0)
    console_pointer DWORD 0
    charsRead DWORD 0
    returnAddress DWORD 0
    bytesWritten DWORD 0
.code
main PROC
    MOV returnAddress, ESP

    PUSH EBP
    MOV EBP, ESP


;;STRING DECLARATION
    MOV AL, 0
    MOV AH, 0
    PUSH AX
    MOV AL, 35
    MOV AH, 35
    PUSH AX
    MOV AL, 35
    MOV AH, 35
    PUSH AX
    MOV AL, 35
    MOV AH, 35
    PUSH AX
    MOV AL, 35
    MOV AH, 35
    PUSH AX
    MOV AL, 35
    MOV AH, 35
    PUSH AX
    MOV EDX, ESP
    PUSH EDX


    MOV EAX, console_pointer
    CMP EAX, 0
    JNE DONT_Call_function
        MOV ECX, LENGTHOF ConsoleInput
        MOV EDX, OFFSET ConsoleInput
        INVOKE GetStdHandle, -10
        INVOKE ReadConsoleA, Eax, Edx, Ecx, OFFSET charsRead, 0
    DONT_CALL_function:
    MOV EDX, [EBP - 16]
    LEA ESI, ConsoleInput
    ADD ESI, console_pointer
    MOV EBX, ' '
    CALL StrCpy
    ADD console_pointer, ECX
    MOV EAX, console_pointer
    CMP EAX, charsRead
    JL DONT_RESET
        MOV console_pointer, 0
    DONT_RESET:

    MOV EDX, [EBP - 16]

    CALL StringLength
    MOV ECX, EAX
    INVOKE GetStdHandle, -11
    INVOKE WriteConsoleA, EAX, EDX, ECX, bytesWritten, 0



;;EXIT
    MOV EAX, 0
    PUSH EAX
    POP EAX
    MOV ESP, returnAddress
    RET


    MOV EAX, 0
    MOV ESP, returnAddress
    RET
main ENDP

StringLength PROC USES EDX
    MOV EAX, 0
        StartL:
        CMP Byte Ptr [EDX + EAX], 0
        JE EndL
            INC EAX
        JMP StartL
        EndL:
        ret
StringLength ENDP

    StrCpy PROC USES ESI EDX EBX EAX
    MOV ECX, 0
    L1:
        CMP Byte Ptr [ESI], BL
        JE _return
        CMP Byte Ptr [ESI], 0
        JE _return
        MOV AL, Byte Ptr[ESI]
        MOV Byte Ptr[EDX], AL
        INC ESI
        INC EDX
        INC ECX
        JMP L1
    _return:
        ret
    StrCpy ENDP
end main