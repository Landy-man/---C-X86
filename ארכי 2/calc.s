%define newline 10
%define nullTerminator 0
%define nodeSize 5

; *** Node in list *** ;
%define val 0
%define next 1
%define prev 5

%macro malloc_macro 0
     push    9
     call    malloc
     add esp,4      
%endmacro

%macro print_macro 3
        push    %1
        push    %2
        push    dword [%3]
        call    fprintf
        add esp,12
%endmacro

section	.rodata
    formatInt:
	DB	"%d", newline, nullTerminator	; Format int
    formatStringNewLine:
	DB	"%s", newline, nullTerminator	; Format string new line
    formatString:
	DB	"%s", 0	; Format string

    format2DigHexa:
	DB	"%02x", nullTerminator	; Format hexa print
    formatHexa:
	DB	">>%x", nullTerminator	; Format full hexa
    newLine:
        DB      newline, nullTerminator      ;Format new line
    
    startOfline:
	DB	">>calc:" , 0	; Format prompt


    overflow:
        DB      ">>Error: Operand Stack Overflow",nullTerminator
    underflow:
        DB      ">>Error: Insufficient Number of Arguments on Stack",nullTerminator
    illegal:
        DB      ">>Error: Illegal Input",nullTerminator
        
    shiftError:
        DB      ">>Error: Exponent too large", nullTerminator

section .data    
    stackSize EQU 6
    counter DD 0
    stackCounter DD 0;
    flag DB 0;
    inDebug DB 0
        


section .bss
    opStack:
        RESD stackSize;
    buffer:
        RESD	10
    dupHere:
        RESD    1
    count:
        RESD    1
    countInc:
        RESD    1
    sum:
        RESD    1
    devider:
        RESD    1

section .text
	align 16
	global main
        extern exit
        extern printf
        extern fprintf
        extern malloc
        extern free
        extern fgets
        extern stderr
        extern stdin
        extern stdout 

main:              ; check for _-d_ in argv
	pop ebx	       ; return address
	pop ebx        ; argc 
	cmp ebx , 1
	je my_calc     ;if only 1 argument then no debug flag
	pop ebx
	mov ebx, [ebx+4]
	cmp byte [ebx], '-'
	jne my_calc
	cmp byte [ebx+1], 'd'
	jne my_calc
	cmp byte [ebx+2],0
	jne my_calc
	mov byte[inDebug],1        ; set _inDebug_ flag

my_calc:
        print_macro startOfline, formatString, stdout   
        call inputFromUser

        ; *** check user input *** ;
        cmp byte[buffer], '+'
        je addition
        cmp byte[buffer], 'p'     
        je popandprint        ;
        cmp byte[buffer], 'd'     
        je duplicate          ;
        cmp byte[buffer], 'l'
        je shiftleft
        cmp byte[buffer], 'r'
        je shiftright
        cmp byte[buffer], 'q'     
        je quit               ;
        cmp byte[buffer], '0'
        jl promptIllegalError         ;
        cmp byte [buffer], '9'
        jg promptIllegalError        ;
        cmp byte [stackCounter],stackSize-1
        je promptOverflowError       ;
        call createlist    
        jmp my_calc                   ;



    
;               addition function
addition:
        cmp byte [stackCounter],1
        jle promptunderflowError
        inc dword [counter]
       
        mov esi,[stackCounter]
        mov esi,[opStack+esi*4]
        push esi
        dec byte [stackCounter]
        mov edi,[stackCounter]
        mov edi,[opStack+edi*4]
        push edi
        call addition_f
        add esp,8
        jmp my_calc

addition_f:
        push ebp
        mov ebp, esp
        mov esi, [ebp + 4 + 2*4]    ; second number
        mov edi, [ebp + 4 + 1*4]    ; first number
        push esi        ;save the memory address of the second number in order to free its memory

.addloop:
        cmp dword [esi+next],0
        je  .fillcarrydest
        cmp dword [edi+next],0
        je  .initdest
        mov al,[edi]
        adc al,[esi]
        daa
        mov [edi],al
        mov esi,[esi+1]
        mov edi,[edi+1]
        adc byte [edi],0
        jmp .addloop
.initdest:
        mov al,[edi]
        adc al,[esi]
        daa
        mov [edi],al
        jnc .copy
        cmp dword [esi+1],0
        je  .addnode  
        malloc_macro
        mov dword [eax+1],0 
        mov [edi+1],eax
        mov [eax + prev], edi
        mov edi,[edi+1]
        mov esi,[esi+1]
        mov byte [edi],1
        jmp .initdest
.copy:
        cmp dword [esi+1],0
        je  .end
        malloc_macro
        mov dword [eax+1],0 
        mov [edi+1],eax
        mov [eax + prev], edi
        mov edi,[edi+1]
        mov esi,[esi+1]
        mov al,[esi]
        mov [edi],al
        jmp .copy
  
.fillcarrydest:
        mov al,[edi]
        adc al,[esi]
.fillloop:
        daa
        mov [edi],al
        jnc .end
        cmp dword [edi+1],0
        je  .addnode
        mov edi,[edi+1]
        mov al,[edi]
        add al,1
        jmp .fillcarrydest
  
.addnode:
        malloc_macro

        mov [edi+next],eax
        mov [eax + prev], edi
        mov  byte [eax], 0
        mov dword [eax+1],0
        inc byte [eax]
.end:
        pop esi
.loop:
        cmp byte [flag], 1
        je .keepgoing
        cmp     esi,0
        je      .keepgoing
        push    esi
        mov     esi,[esi+1]
        call    free
        add esp,4
        jmp .loop
.keepgoing:

        cmp byte [inDebug],0
        je  .nodebug
        push    1
        call    printHead
        add esp, 4
.nodebug:
        pop ebp
        ret












;               Duplicate top of stack
duplicate:
        cmp byte [stackCounter],0
        je  promptunderflowError
        mov eax,stackSize-1
        cmp eax,[stackCounter]
        je  promptOverflowError
       
        inc dword [counter]

        mov edi,[stackCounter]
        mov edi,[opStack+edi*4]
        push edi
        inc byte [stackCounter]
        mov edi, [stackCounter]
        shl edi, 2
        mov eax, opStack
        add eax, edi
        push eax
        ;mov edi, [opStack + edi*4]
        ;push edi
        call duplicate_f
        add esp, 8
        jmp my_calc

duplicate_f:
        push ebp
        mov ebp, esp

     
        malloc_macro

        mov esi, [ebp + 4  + 4]      ; where to duplicate to
        mov edi, [ebp + 4 + 2*4]    ; location of number to duplicate

        mov [esi],eax
        mov esi,eax
        mov dword [esi + prev],0

.loop:
        mov dl,[edi + val]                ; dl contains val of edi
        mov [esi],dl                      ; put val in new node
        mov edi,[edi+next]
        cmp edi,0
        je  .end
        push edi
        push esi
        malloc_macro
        pop esi
        pop edi
        mov [esi+next],eax
        mov dword [eax + prev], esi
        mov esi,eax
        jmp .loop
.end:
        mov dword [esi+1],0
        cmp byte [inDebug],0
        je  .nodebug
        push    1
        call    printHead
        add esp, 4
.nodebug:
        pop ebp
        ret

;              Create a list containing the number the user just entered 
createlist:
        push    ebp
        mov ebp, esp  
        mov     esi, buffer
        cmp byte [inDebug],0
        je  .nodebug
        print_macro buffer, formatString, stderr
.nodebug:
        inc byte [stackCounter]
        mov     ecx, -1
ignore_zeroes:
        cmp byte [esi], '0'
        jne length_check
        inc esi
        jmp ignore_zeroes
length_check:
        inc ecx
        cmp byte [esi + ecx], newline;
        jne length_check
        dec ecx
        push    ecx
       
        malloc_macro
       
        pop ecx
        mov edx,[stackCounter]
        mov [opStack+edx*4],eax
        mov bl,[esi+ecx]
        sub bl,'0'
        dec ecx
        cmp ecx,0
        jl  assign
        mov bh,[esi+ecx]
        sub bh,'0'
        shl bh,4
        add bl,bh
assign:
        mov byte [eax + val],bl
        mov dword [eax+next],0
        mov dword [eax + prev],0
        mov edx,eax
        cmp ecx,0
        jl  endloop

loop1:
        dec ecx
        cmp ecx,0
        jl  endloop
        push    ecx
        push    edx
        
        malloc_macro
       
        pop edx
        pop ecx
        mov bl,[esi+ecx]
        sub bl,'0'
        dec ecx
        cmp ecx,0
        jl  .assign
        mov bh,[esi+ecx]
        sub bh,'0'
        shl bh,4
        add bl,bh
.assign:
        mov byte [eax],bl
        mov [edx + next],eax
        mov [eax + prev], edx
        mov dword [eax + next],0
        mov edx,eax
        cmp ecx,0
        jl  endloop
        jmp loop1
endloop:
        pop ebp
        ret




;               Error handling
promptIllegalError:
        push    illegal
        push    formatStringNewLine
        call    print
        jmp     my_calc
promptOverflowError:
        push    overflow
        push    formatStringNewLine
        call    print
        jmp     my_calc
promptunderflowError:
        push    underflow
        push    formatStringNewLine
        call    print
        jmp     my_calc
promptshiftError:
        push    shiftError
        push    formatStringNewLine
        call    print
        jmp     my_calc

;               reads input from user
inputFromUser:
        mov ebp, esp
        
        push    dword [stdin]
        push    dword 80
        push    dword buffer
        call    fgets
        mov esp, ebp
        ret
        
        
;               print function to stderr
printToErrorLog:
        push    ebp
        mov ebp, esp
        push    dword [ebp+12]
        push    dword [ebp+8]
        push    dword [stderr]
        call    fprintf
        add     esp, 12     ; Clean up stack after call 
        pop ebp
        ret

;               print to console
print:
        push    ebp
        mov ebp, esp
        push    dword [ebp+12]
        push    dword [ebp+8]
        push    dword [stdout]
        call    fprintf
        add     esp, 12     ; Clean up stack after call 
        pop ebp
        ret



;               pop and print function
popandprint:
        inc dword [counter]
        push    0
        call    printHead
        add esp, 4
        call    pop
        jmp my_calc
printHead:
        push    ebp
        mov ebp, esp
        cmp byte [stackCounter],0
        je  promptunderflowError
        mov esi,[stackCounter]
        mov esi,[opStack+esi*4]
        mov ecx,-1
.getsize:
        inc ecx
        cmp dword [esi+1],0
        je  .printLoop
        mov esi,[esi+1]
        jmp .getsize
.printLoop: 
        mov esi,[stackCounter]
        mov esi,[opStack+esi*4]
        cmp ecx,-1
        jle .end
        mov edx,ecx
.printTail:
        cmp edx,0
        je  .printlist
        dec edx
        mov esi,[esi+1]
        jmp .printTail
.printlist:
        xor eax,eax
        mov al,[esi]
        push    ecx
        push    eax
        cmp dword [esi+1],0
        je  .first
        push    format2DigHexa
        jmp .continue
.first:
        push    formatHexa
.continue:
        cmp dword [ebp+8],1
        je  .error
        call    print
        jmp .finish
.error:
        call    printToErrorLog
.finish:
        add esp,8
        pop ecx
        dec ecx
        jmp .printLoop
.end:
        push    newLine
        call    printf
        add esp,4
        pop ebp
        ret



pop:
        push    ebp
        mov     esi,[stackCounter]

        mov dword esi, [opStack + esi*4]
.loop:
        cmp esi,0
        je .done
        push    esi
        mov esi,[esi+1]
        call    free
        add esp,4
        jmp .loop
.done:
        dec byte [stackCounter]
        pop ebp
        ret 

quit:

        mov edi ,[stackCounter]
        cmp edi,0
        jl .continue
        mov edi ,[opStack+edi*4]
.loop:
        cmp edi,0
        je .nextElement
        push edi
        mov dword edi,[edi+1]
        call free
        add esp,4
        jmp .loop
.nextElement:
        dec dword [stackCounter]
        jmp quit
.continue:        
        push dword [counter]
        push formatInt
        push dword [stdout]
        call fprintf
        push 0
        call exit


; calculates ans = x*2^k
shiftleft:
    mov dword edi,[stackCounter]
    cmp edi,2
    jl promptunderflowError
    mov edi, [opStack+edi*4]
    cmp dword [edi+1],0
    jne promptshiftError
    inc dword [counter]

    push edi        ; pointer to k list

    mov edi, 0
    mov dword edi, [stackCounter]
    dec edi
    mov edi, [opStack + edi*4]
    push edi            ; pointer to x

    call shiftleft_f
    add esp, 8
    jmp my_calc

shiftleft_f:
    push ebp
    mov ebp, esp

    mov edi, [ebp + 4 + 2*4]    ; get k address
    mov ebx, [ebp + 4 + 1*4]    ; get x address
    push ebx
    push edi
    call calc_k
    add esp, 4

    pop ebx

    ;right now eax contains k decimal value.
    ; and ebx contains the address of x

.myLoop:
    cmp eax, 0
    je .end
    
    push eax
    push ebx

    push ebx
    mov eax, dupHere
    push eax
    call duplicate_f
    add esp, 8
    
    pop ebx
    pop eax

    push ebx
    push eax
    push dword [dupHere]
    push ebx
    call addition_f
    add esp, 8
    pop eax
    pop ebx
    dec eax
    jmp .myLoop
.end:
    cmp byte [inDebug],0
    je  .nodebug
    push    1
    call    printHead
    add esp, 4
.nodebug:
    pop ebp
    ret

; al will contain the value, hence eax will be the decimal value of k
; method deletes the given reference to k
; also decreases stack counter
calc_k:
    push ebp
    mov ebp, esp

    mov edi, [ebp + 4 + 1*4]    ;get k
    mov byte cl, [edi]
    mov eax, 0
    shr cl, 4
    mov al, cl
    mov cl, 10
    mul cl
    mov byte cl,[edi]
    shl cl, 4
    shr cl, 4
    add al, cl

    push eax
    push edi
    call free
    add esp, 4
    pop eax
    dec byte [stackCounter]

    pop ebp
    ret


calc_k_2:
     push ebp
    mov ebp, esp

    mov edi, [ebp + 4 + 1*4]    ;get k
    mov byte cl, byte [edi]
    mov eax, 0
    shr cl, 4
    mov al, cl
    mov cl, 10
    mul cl
    mov byte cl,[edi]
    shl cl, 4
    shr cl, 4
    add al, cl

    pop ebp
    ret




compare:
        cmp byte [stackCounter],1
        jle promptunderflowError
        
        inc dword [counter]
       
        mov esi,[stackCounter]
        mov esi,[opStack+esi*4]
        push esi
        
        mov edi,[stackCounter]
        dec edi
        mov edi,[opStack+edi*4]
        push edi
        call compare_f
        add esp,8
        jmp my_calc

compare_f:
    push ebp
    mov ebp, esp
    mov esi, [ebp + 4 + 2*4]    ; second number // ebx
    mov edi, [ebp + 4 + 1*4]    ; first Number  // eax

    push esi
    push edi

    push esi
    call length
    add esp, 4
    
    pop edi
    pop esi

    mov ebx, eax

    push esi
    push edi

    push edi
    call length
    add esp,4

    pop edi
    pop esi

    cmp eax, ebx
    je .getLast
    cmp eax, ebx
    jg .greater
    cmp eax, ebx
    jl .less

.greater:
    mov eax, 1
    jmp .end
.less:
    mov eax, 0

.end:
    pop ebp
    ret

.getLast:
    cmp eax, 1
    jle .calc
    mov esi, [esi + next]
    mov edi, [edi + next]
    dec eax
    jmp .getLast

.equal:
    mov eax, 0
    jmp .end

.calc:
    cmp ebx, 0
    je .equal

    push edi
    push esi
    push ebx

    push edi
    call calc_k_2
    add esp, 4

    mov edx, 0
    mov dl , al

    push edx
    push esi
    call calc_k_2
    add esp, 4
    pop edx

    pop ebx
    pop esi
    pop edi

    cmp dl, al
    jl .less
    cmp dl, al
    jg .greater
    mov edi, [edi + prev]
    mov esi, [esi + prev]
    dec ebx
    jmp .calc



length:
    push ebp
    mov ebp, esp
    mov eax, 0
    mov esi, [ebp + 4 + 1*4]    ; number to check
.counting:
    inc eax
    cmp dword [esi + next],0
    je .end
    mov esi, [esi + next]
    jmp .counting
.end:
    pop ebp
    ret


shiftright:
    mov dword edi,[stackCounter]
    cmp edi,2
    jl promptunderflowError
    mov edi, [opStack+edi*4]
    cmp dword [edi+1],0
    jne promptshiftError
    
    inc dword [counter]

    push edi        ; pointer to k list

    mov edi, 0
    mov dword edi, [stackCounter]
    dec edi
    mov edi, [opStack + edi*4]
    push edi            ; pointer to n
    call shiftright_f
    add esp, 8
    mov byte [flag], 0
    jmp my_calc

shiftright_f:
    push ebp
    mov ebp, esp

    mov esi, [ebp + 4 + 1*4]    ;pointer to n
    mov edi, [ebp + 4 + 2*4]    ; pointer to k
    
    push esi
    push edi

    malloc_macro
    mov byte [eax], 1
    mov dword [eax + next], 0
    mov dword [eax + prev], 0
    mov dword [devider], eax

    pop edi
    
    push dword edi
    push dword [devider]
    call shiftleft_f
    add esp , 8

    malloc_macro
    mov byte [eax], 0
    mov dword [eax + next], 0
    mov dword [eax + prev], 0
    mov dword [count], eax

    malloc_macro
    mov byte [eax], 1
    mov dword [eax + next], 0
    mov dword [eax + prev], 0
    mov dword [countInc], eax

    malloc_macro
    mov byte [eax], 0
    mov dword [eax + next], 0
    mov dword [eax + prev], 0
    mov dword [sum], eax


    mov byte [flag], 1
     pop esi
.loop:
    push esi
    mov edx, 0
    push dword [devider]
    push dword [sum]
    call addition_f
    add esp, 8

    mov edx, 1

    pop esi
    push esi


    push esi
    push dword [sum]
    call compare_f
    add esp, 8

mov edx, 2

    pop esi
    cmp eax, 1
    je .end


    push esi
    mov edx, [countInc]
    push edx
    push dword [count]
    call addition_f
    add esp, 8

mov edx, 3
    pop esi
    jmp .loop

   ;
    ;;;;;;;;;;;;;;
  

.end:
    mov esi, [stackCounter]
    mov dword esi, [opStack+4*esi]
    .looplast:
        cmp esi,0
        je .checknext1
        push    esi
        mov esi,[esi+1]
        call    free
        add esp,4
        jmp .looplast

.checknext1:
    mov dword esi, [sum]
    .loopsum:
        cmp esi,0
        je .checknext2
        push    esi
        mov esi,[esi+1]
        call    free
        add esp,4
        jmp .loopsum

.checknext2:
    mov dword esi, [devider]
    .loop1:
        cmp esi,0
        je .endof
        push    esi
        mov esi,[esi+1]
        call    free
        add esp,4
        jmp .loop1

.endof:
    mov dword esi, [countInc]
    push esi
    call free
    add esp,4
    mov edx, [stackCounter]
    mov ebx, [count]
    mov dword [opStack + 4*edx], ebx

    cmp byte [inDebug],0
    je  .nodebug
    push    1
    call    printHead
    add esp, 4
.nodebug:
    pop ebp
    ret