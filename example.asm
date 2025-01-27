.386
.model flat,stdcall
.data

    GetStdHandle PROTO, nStdHandle:DWORD
    WriteConsoleA PROTO, handle:DWORD, lpBuffer:PTR BYTE, nNumberOfCharsToWrite:DWORD, lpNumberOfCharsWritten:PTR DWORD, lpReserved:PTR DWORD

    returnAddress DWORD 0
    bytesWritten DWORD 0
.code
main PROC
    MOV returnAddress, ESP

    PUSH EBP
    MOV EBP, ESP


;;INT DECLARATION
    MOV EAX, 8
    PUSH EAX


;;STRING DECLARATION
    MOV AL, 0
    MOV AH, 0
    PUSH AX
    MOV AL, 10
    MOV AH, 0
    PUSH AX
    MOV AL, 108
    MOV AH, 108
    PUSH AX
    MOV AL, 72
    MOV AH, 101
    PUSH AX
    MOV EDX, ESP
    PUSH EDX



;;FOR LOOP

;;INT DECLARATION
    MOV EAX, 5
    PUSH EAX
FOR_LOOP0_S:
    MOV EAX, DWORD PTR [EBP - 20]
    PUSH EAX
    TEST EAX, EAX
    JZ FOR_LOOP0_E

;;BEGIN_SCOPE
        MOV EDX, [EBP - 16]

        CALL StringLength
        MOV ECX, EAX
        INVOKE GetStdHandle, -11
        INVOKE WriteConsoleA, EAX, EDX, ECX, bytesWritten, 0


    ADD ESP, 0

;;END_SCOPE


;;INT ASSIGNMENT
    MOV EAX, DWORD PTR [EBP - 20]
    PUSH EAX
    MOV EAX, 1
    PUSH EAX
    POP EBX
    POP EAX
    SUB EAX, EBX
    PUSH EAX
    POP EAX
    MOV DWORD PTR [EBP - 20], EAX
    JMP FOR_LOOP0_S
    FOR_LOOP0_E:


;;EXIT
    MOV EAX, DWORD PTR [EBP - 4]
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

end main