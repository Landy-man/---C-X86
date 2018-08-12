        global printer, debugPrint
        extern resume, state , WorldWidth, WorldLength

        ;; /usr/include/asm/unistd_32.h
sys_write:      equ   4
stdout:         equ   1
stderr:         equ   2

;system call - 'write'
;%1 = pointer to first byte to write
;%2 = number of bytes to write
;ignores return value
%macro write_non 3   
        pusha
        mov eax, sys_write
        mov ebx, %3
        mov edx, %2
        mov ecx, %1
        int 0x80
        popa
%endmacro


%macro printStates_all 1    ;prints the current lifecycle of state array
        mov edx, 0          ; edx = number of rows
        mov eax, state      ; eax = pointer to the first cell in a certin row
%%loop1:
        cmp edx, [WorldLength] ; checks if we finished printing
        je %%finish
        mov ebx, 0             ; ebx = number of cols
%%loop2:
        mov ecx, eax            
        add ecx, ebx            ; ecx = pointer to the cell to be printed
        mov dword [charHolder], ecx     ; charHolder = pointer to the buffer to be printed
        write_non [charHolder], 1 , %1     ; %1 buffer to be printed %2 length to write


%%cellPrinted:
        inc ebx                 ; moves to the next cell in a row
        cmp ebx, [WorldWidth]   ; check if should start new row
        je %%nextIteration      ; new Row jmp
        jmp %%loop2             ; jmp next cell in the row
%%nextIteration:
        inc edx                 ; inc "row Counter"
        add eax, [WorldWidth]   ; moves eax to point to the start on a new row
        write_non new_line,1, %1    ; prints new new_line
        jmp %%loop1             ; start over
%%finish:

%endmacro

section .rodata
new_line: db 0x0a
space: db ' '

section .data
charHolder dd 0


section .text

printer:
        printStates_all stdout
        xor ebx, ebx
        call resume             ; resume scheduler
        jmp printer

debugPrint:
        printStates_all stderr
        ret
