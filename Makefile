CC := gcc

# From https://stackoverflow.com/a/3376483
CFLAGS := -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code

quic: main.o
	$(CC) -o quic main.o

main.o: main.c
	$(CC) $(CFLAGS) -c -o main.o main.c

clean:
	-rm -f quic main.o
