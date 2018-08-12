section	.rodata
LC0:
	DB	"%s", 10, 0	; Format string

section .bss
LC1:
	RESB	256

section .text
	align 16
	global my_func
	extern printf

my_func:
	push	ebp
	mov	ebp, esp	; Entry code - set up ebp and esp
	pushad			; Save registers
	mov ecx, dword [ebp+8]	; Get argument (pointer to string)
	mov edx, LC1

        cmp byte [ecx], 10
        je finish
	
loop:


        mov al, 16
	
        mov bh, 0
	mov bl, byte [ecx]	
	cmp bl, 57
	jg letter
	sub bl, 0x30
	
	jmp second
	
letter:
        cmp bl, 70
        jg lowCase
        sub bl, 55
        
        jmp second
        
lowCase:
        sub bl, 87
        
second:
        mul bl
        inc ecx
        mov bl, [ecx]
        
        cmp bl, 57
	jg letter2
	sub bl, 48

	jmp doneAndLoop
	
letter2:
        cmp bl, 70
        jg lowCase2
        sub bl, 55        
        jmp doneAndLoop
        
lowCase2:
        sub bl, 87
        
doneAndLoop:
        
        add al, bl
        mov [edx], al
        inc edx
        inc ecx
        cmp byte [ecx], 10
        je finish
        jmp loop

finish:
	push	LC1		; Call printf with 2 arguments: pointer to str
	push	LC0		; and pointer to format string.
	call	printf
	add 	esp, 8		; Clean up stack after call

	popad			; Restore registers
	mov	esp, ebp	; Function exit code
	pop	ebp
	ret

