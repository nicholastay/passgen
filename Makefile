NAME = passgen
PREFIX = $(HOME)/.local

CC = gcc
CFLAGS += -std=c99 -Wall -Wextra -Wshadow -Werror -pedantic

default: $(NAME)

$(NAME): $(NAME).c config.h
	$(CC) $(CFLAGS) -o $(NAME) $(NAME).c

install: $(NAME)
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 755 $(NAME) $(DESTDIR)$(PREFIX)/bin/

clean: 
	rm -f *.o
	rm -f $(NAME)
