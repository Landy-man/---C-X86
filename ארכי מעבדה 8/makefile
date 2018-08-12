 
all: task3 task2 task1

task3:   task3.o
	gcc -m32 task3.o -o task3
	
task3.o: task3.c
	gcc -m32 -Wall -c task3.c -o task3.o
	
task2:   task2.o
	gcc -m32 task2.o -o task2
	
task2.o: task2.c
	gcc -m32 -Wall -c task2.c -o task2.o
	
task1:   task1.o
	gcc -m32 task1.o -o task1
	
task1.o: task1.c
	gcc -m32 -Wall -c task1.c -o task1.o
	


	
.PHONY: clean
clean:
	rm -f *.o task1 task2 task3
