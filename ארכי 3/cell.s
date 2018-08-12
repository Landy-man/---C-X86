global cell_calc
extern resume, WorldWidth, cell, state


section .text

;eax->row (x) ; edx-> line (y)
%macro calc_xy_ad 0
        push ebx        ; (ebx-2)%WorldWidth=y , (ebx-2)/WorldWidth=x
        mov dword eax,0
        dec ebx
        dec ebx
        mov edx, 0     
        mov eax, ebx
        mov ebx, [WorldWidth]
        div ebx
        pop ebx
%endmacro
        

cell_calc:
        pop ebx         ; ebx is cell index in co_routines array
        calc_xy_ad      ; get the x,y values of this cells location in state array
        push ebx        ; save ebx
        push edx        ; y coordinates
        push eax        ; x coordinates
        call cell       ; returns cells next lifecycle
        add esp, 8

        xor ebx,ebx
        call resume

        pop ebx         ; ebx = cell index in co_routines array
        dec ebx 
        dec ebx         ; to get the index in state array
        mov edx, state          
        mov byte [edx+ebx], al  ; writes the char representation of cells lifecycle
        inc ebx         
        inc ebx                 ; restore ebx to index of cell in co_routines
        push ebx
        xor ebx,ebx
        call resume
        jmp cell_calc
