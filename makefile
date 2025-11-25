CC=gcc
CFLAGS=-I.
DEPS = database.h database_list.h user_interface.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: main.o database.o database_list.o user_interface.o
	$(CC) -o main main.o database.o database_list.o user_interface.o

clean:
	rm -f *.o main