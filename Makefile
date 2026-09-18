CC      = gcc
CFLAGS  = -Wall -Wextra -O2
LDLIBS  = -lcurl -lcjson
PREFIX  = /usr/local

prayer: main.c
	$(CC) $(CFLAGS) main.c -o prayer $(LDLIBS)

install: prayer
	install -Dm755 prayer $(PREFIX)/bin/prayer

uninstall:
	rm -f $(PREFIX)/bin/prayer

clean:
	rm -f prayer

.PHONY: install uninstall clean
