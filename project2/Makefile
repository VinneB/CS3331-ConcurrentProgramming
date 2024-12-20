CC=gcc
FLAGS= -Wall -Wextra -Werror

full: main qsort merge

main: main.o
	$(CC) $(FLAGS) main.o -o main

qsort: qsort.o
	$(CC) $(FLAGS) qsort.o -o qsort

merge: merge.o
	$(CC) $(FLAGS) merge.o -o merge

qsort.o: qsort.c
	$(CC) $(FLAGS) -c qsort.c

merge.o: merge.c
	$(CC) $(FLAGS) -c merge.c

main.o: main.c
	$(CC) $(FLAGS) -c main.c

clean:
	rm *.o qsort main merge