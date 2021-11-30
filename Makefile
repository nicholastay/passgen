CC = gcc
CFLAGS = -Wall -Wextra -Wshadow -Werror -pedantic

default: passgen

passgen: passgen.c
	$(CC) $(CFLAGS) -o passgen passgen.c

clean: 
	rm -f *.o
	rm -f passgen
