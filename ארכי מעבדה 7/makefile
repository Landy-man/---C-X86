all: task1d dig_counter

task1d:   task1d.o
	gcc -m32 task1d.o -o hexeditplus
	
task1d.o: task1.c
	gcc -m32 -Wall -c task1.c -o task1d.o
	
dig_counter:   dig_counter.o
	gcc -m32 dig_counter.o -o dig_counter
	
dig_counter.o: task4.c
	gcc -m32 -Wall -c task4.c -o dig_counter.o
	


	
.PHONY: clean
clean:
	rm -f *.o hexeditplus dig_counter
