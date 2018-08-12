%define newline 10
%define nullTerminator 0
%define stderr 2


        ;; /usr/include/asm/unistd_32.h
sys_exit:       equ   1
maxSize: equ 100*100


; %1- input buffer, %2 num of bytes, eax->fd
%macro readFromFile_non 2
		pusha
		mov ebx,eax
		mov ecx,%1
		mov edx,%2
		mov eax,3
		int 0x80
		popa
%endmacro



%macro openFile_a 1
		push ebx
		push ecx
		push edx
		mov eax,5
		mov ebx,[%1]
		mov ecx,0
		int 0x80
		pop edx
		pop ecx
		pop ebx
%endmacro

;system call 'close'
%macro closeFile_non 1
	pusha
	mov eax, 6
	mov ebx, %1
	popa
%endmacro


; assumes $edx points to argv[this - 1] the ADDRESS of the variable is in argv[this]
%macro fillField_bd 1
    add edx,4
	mov ebx,[edx]
	mov [%1],ebx     
%endmacro


%macro atoi_a 1
	push dword [%1]
	call atoi
	mov dword [%1], eax
	add esp, 4
%endmacro

;%1 = variable address
;puts in ecx the length of the string.
;also adds '\n' to string
%macro getFieldLength_c 1
		push edx
		mov ecx, 1
		mov edx, [%1]
	%%loop:
		cmp byte [edx + ecx], 0
		je %%foundLength
		inc ecx
		jmp %%loop
	%%foundLength:
		pop edx
%endmacro

;%1 = message to print
;%2 = length of message
;%3 = variable to print
%macro printVariable_c 3
	cmp byte [debug], 1
	jne %%nodebug
	write_non %1,%2
	getFieldLength_c %3
	write_non [%3],ecx
	write_non new_line,1
	%%nodebug:
%endmacro

%macro setFieldVals_abd 0
fillField_bd filename
	fillField_bd WorldLength
	fillField_bd WorldWidth
	fillField_bd maxgen
	fillField_bd printfre
	printVariable_c msg_len,len_len,WorldLength
	printVariable_c msg_width,len_width,WorldWidth
	printVariable_c msg_maxgen, len_maxgen, maxgen
	printVariable_c msg_printfre, len_printfre, printfre

	atoi_a WorldLength
	atoi_a WorldWidth
	atoi_a maxgen
	atoi_a printfre
%endmacro


; pre-conditions: eax contains 'fd' to input file.
; method fills gameTable with the values 1,0 according to input file
%macro fillGameTable_abcd 0
		mov ebx,0				
		mov edx,state		; our int array
	%%loop:
		cmp ebx,[WorldLength]
		je %%done
		readFromFile_non edx,[WorldWidth]

		add edx,[WorldWidth]
		mov ecx, temp
		readFromFile_non ecx, 1
		inc ebx
		jmp %%loop
	%%done:

		mov eax,dword [WorldWidth]
		imul dword [WorldLength]

		mov ebx,state
	%%init_loop:
		cmp eax,0
		jl %%finish
		
		cmp byte [ebx + eax], 0x31
		je %%keepGoing
		mov byte [ebx + eax], '0'
%%keepGoing:
		dec eax
		jmp %%init_loop
	%%finish:
%endmacro

;system call - 'write'
;%1 = pointer to first byte to write
;%2 = number of bytes to write
;ignores return value
%macro write_non 2
	pusha
	mov eax, 4
	mov ebx, stderr
	mov edx, %2
	mov ecx, %1
	int 0x80
	popa
%endmacro


section .rodata
msg_len: db 'length=',nullTerminator
len_len: equ $-msg_len

msg_width: db 'width=',nullTerminator
len_width: equ $-msg_width

msg_maxgen: db 'number of generations=',nullTerminator
len_maxgen: equ $-msg_maxgen

msg_printfre: db 'print frequency=',nullTerminator
len_printfre: equ $-msg_printfre
new_line: db 0x0a
space: db ' '
one: db '1'
zero: db '0'

section .bss
state: resb maxSize
temp: resb 2 


section .data

debug DB 0
ten dd 10
charHolder dd 0
WorldWidth dd 0
WorldLength dd 0
filename dd 0
printfre dd 0
maxgen dd 0


section .text
align 16
global main
global state
global WorldLength
global WorldWidth
extern cell_calc
extern scheduler
extern debugPrint
extern printer
extern start_co
extern init_co
extern init_scheduler


main:
	pop ebx	       ; return address
	pop ebx        ; argc 
	
	cmp ebx , 6		;if argc == 6 there is no debug
	pop ebx		
	je .nodebug
	mov byte [debug], 1
	add ebx, 4		; jump over the debug flag
.nodebug:
	
	mov edx, ebx		
	setFieldVals_abd	;sets all program arguments into respective variables.


	openFile_a filename 	; opens the file
	push eax				; backup fileDiscriptor
	fillGameTable_abcd		; update state array to initial values


	pop eax					; eax = fileDiscriptor
	closeFile_non eax		

    xor ebx, ebx                 ; scheduler i co-routine 0
	mov edx, scheduler
	push dword [printfre]
	push dword [maxgen]
    call init_scheduler
    add esp, 8

    inc ebx						; printer i co-routine 1
    mov edx, printer
    call init_co

    mov eax, dword [WorldLength]
    imul dword [WorldWidth]  	; eax = WorldWidth*WorldLength

.loopInit:				; cell routines initial
	cmp eax,0			;check if finsihed
	je .doneInit
	inc ebx				
	mov edx, cell_calc
	call init_co
	dec eax
	jmp .loopInit

.doneInit:		
	cmp byte [debug], 0
	je .startGame

	call debugPrint
.startGame:
	xor ebx,ebx
	call start_co

        mov eax, 1
        mov ebx, 0
        int 0x80








atoi:
        push    ebp
        mov     ebp, esp        ; Entry code - set up ebp and esp
        push ecx
        push edx
        push ebx
        mov ecx, dword [ebp+8]  ; Get argument (pointer to string)
        xor eax,eax
        xor ebx,ebx
atoi_loop:
        xor edx,edx
        cmp byte[ecx],0
        jz  atoi_end
        imul dword[ten]
        mov bl,byte[ecx]
        sub bl,'0'
        add eax,ebx
        inc ecx
        jmp atoi_loop
atoi_end:
        pop ebx                 ; Restore registers
        pop edx
        pop ecx
        mov     esp, ebp        ; Function exit code
        pop     ebp
        ret
