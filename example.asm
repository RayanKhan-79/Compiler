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
    MOV EDX, 0
    PUSH EDX



;;STRING ASSIGNMENT
    MOV AL, 0
    MOV AH, 0
    PUSH AX
    MOV AL, 111
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
    MOV DWORD PTR [EBP - 4], EDX

    MOV EDX, [EBP - 4]

    CALL StringLength
    MOV ECX, EAX
    INVOKE GetStdHandle, -11
    INVOKE WriteConsoleA, EAX, EDX, ECX, bytesWritten, 0



    MOV EAX, 0
    MOV ESP, returnAddress
    RET
main ENDP

StringLength PROC USES EDX EBX
    MOV EAX, 0
        StartL:
        CMP Byte Ptr [EDX + EAX], 0
        JE EndL
        CMP Byte Ptr [EDX + EAX], BL
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
        CMP Byte Ptr [ESI], 0dh
        JE _return
        CMP Byte Ptr [ESI], 0ah
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