 
CC		:=	gcc 
CC_FLAGS	:=	-m32 -Wall -g
ASM		:=	nasm
ASM_FLAGS	:=	-f elf -g



all: clean ass2

ass2:	
	$(ASM) $(ASM_FLAGS)  calc.s -o calc.o
	$(CC) $(CC_FLAGS) calc.o -o calc
	

clean:
	rm -rf calc.o calc
