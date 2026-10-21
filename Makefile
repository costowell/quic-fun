CC := gcc

# From https://stackoverflow.com/a/3376483
CFLAGS := -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code

all: server client

server: server.o
	$(CC) -o $@ $?

client: client.o
	$(CC) -o $@ $?

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $?

clean:
	-rm -f server client *.o
