section	.rodata
LC0:
	DB	"%d", 10, 0	; Format string
	
illegal:
        DB      "x or k, or both are off range", 10,0

section .bss


section .text
	align 16
	global calc_div
	extern printf
	extern check

calc_div:
	push	ebp
	mov	ebp, esp	; Entry code - set up ebp and esp
	pushad			; Save registers
        
        mov ecx, dword [ebp+12] ; Get argument (k variable, int) - positive and smaller than 31
	mov edx, dword [ebp+8]	; Get argument (x variable, int) - non-negetive

        
        push ecx
        push edx
        call check
        add esp,8

        cmp eax, 0              ; Checking for int correctness
        je done

;       Dividint by 2 (sar comand) ecx
my_loop:
        sar edx,1
        loop my_loop, ecx
        
              
        
;       Printing the result
	push	edx		; Call printf with 2 arguments: pointer to str
	push	LC0		; and pointer to format string.
	call	printf
	add 	esp,8 		; Clean up stack after call	

break:
	popad			; Restore registers
	mov	esp, ebp	; Function exit code
	pop	ebp
	ret

done:
        push illegal
        call printf
        add esp,4
        jmp break