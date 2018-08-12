CC              :=      gcc
CC_FLAGS        :=      -m32 -Wall -g
ASM             :=      nasm
ASM_FLAGS       :=      -f elf -g



all: ass3

ass3:	cell.o scheduler.o printer.o cell2.o coroutines.o ass3.o
	$(CC) $(CC_FLAGS) cell.o scheduler.o printer.o cell2.o coroutines.o ass3.o -o ass3

ass3.o: ass3.s
	$(ASM) $(ASM_FLAGS)  ass3.s -o ass3.o

scheduler.o: scheduler.s
	$(ASM) $(ASM_FLAGS)  scheduler.s -o scheduler.o

printer.o: printer.s
	$(ASM) $(ASM_FLAGS)  printer.s -o printer.o

cell2.o: cell.s
	$(ASM) $(ASM_FLAGS)  cell.s -o cell2.o

coroutines.o: coroutines.s
	$(ASM) $(ASM_FLAGS)  coroutines.s -o coroutines.o

cell.o: cell.c
	gcc -m32 -g -ansi -Wall -c -o cell.o cell.c 

clean:
	rm -f *.o ass3
