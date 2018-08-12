        global scheduler
        extern resume, end_co, WorldWidth, WorldLength, enc_co

section .data
printCounter dd 0
maxgen dd 0
genNum dd 0
printfre dd 0
section .text

%macro decAndPrint 0
        decPrintCounter
        checkPrint
%endmacro


%macro checkPrint 0
        
        cmp dword [printCounter], 0
        jne %%done
        push eax
        push ebx
        mov ebx, 1
        call resume
        pop ebx
        mov eax, dword [printfre]
        mov dword [printCounter], eax 
        pop eax
%%done:
%endmacro

%macro decPrintCounter 0
        push eax
        mov eax, dword [printCounter]
        dec eax
        mov dword [printCounter], eax
        pop eax
%endmacro

%macro loopOnCells 0
%%callCells1:
        mov eax,dword [WorldWidth]
        imul dword [WorldLength] 
        inc eax 
        inc eax;;;;;

        cmp eax,ebx
        jne %%callCell1
        mov ebx, 2
        jmp %%firstRunDone
%%callCell1:

        call resume
        decAndPrint
        inc ebx
        jmp %%callCells1
%%firstRunDone:
%endmacro

scheduler:
        mov ebx, 2
        mov  ecx, dword [esp + 4]
        mov dword [printfre], ecx
        mov ecx, dword [esp]
        mov dword [maxgen], ecx

.mainLoop:
        checkPrint
        mov eax, dword [genNum]
        cmp eax, dword [maxgen]
        je .byebye
        inc eax
        mov dword [genNum], eax
        loopOnCells
        loopOnCells
        jmp .mainLoop
.byebye:
        mov ebx, 1
        call resume
        xor ebx, ebx
        call end_co






