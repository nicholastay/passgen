NAME = passgen
PREFIX = $(HOME)/.local

CC = gcc
CFLAGS += -std=c99 -Wall -Wextra -Wshadow -pedantic

CC_WIN32 = i686-w64-mingw32-gcc

default: $(NAME)

$(NAME): $(NAME).c config.h
	$(CC) $(CFLAGS) -o $(NAME) $(NAME).c

$(NAME).exe: $(NAME).c config.h
	$(CC_WIN32) $(CFLAGS) -o $(NAME).exe $(NAME).c

install: $(NAME)
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 755 $(NAME) $(DESTDIR)$(PREFIX)/bin/

clean: 
	rm -f *.o
	rm -f $(NAME)
