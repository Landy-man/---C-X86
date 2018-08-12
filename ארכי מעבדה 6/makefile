all:task4 task3 task2 task1

task4:   JobControl.o LineParser.o task4.o
	gcc -m32 task4.o LineParser.o JobControl.o -o task4
	
task4.o: task4.c
	gcc -m32 -Wall -c task4.c -o task4.o
	
LineParser.o: LineParser.h LineParser.c
	gcc -m32 -Wall -c LineParser.c -o LineParser.o

JobControl.o: JobControl.h JobControl.c
	gcc -m32 -Wall -c JobControl.c -o JobControl.o

	
task3:   JobControl.o LineParser.o task3.o
	gcc -m32 task3.o LineParser.o JobControl.o -o task3
	
task3.o: task3.c
	gcc -m32 -Wall -c task3.c -o task3.o

task2:   task2.o
	gcc -m32 task2.o -o task2
	
task2.o: task2.c
	gcc -m32 -Wall -ansi -c task2.c -o task2.o
	
task1:   JobControl.o LineParser.o task1.o
	gcc -m32 task1.o LineParser.o JobControl.o -o task1
	
task1.o: task1.c
	gcc -m32 -Wall -c task1.c -o task1.o

	
.PHONY: clean
clean:
	rm -f *.o task4 task3 task2 task1
