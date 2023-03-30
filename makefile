
CFLAGS = -g -Wall -Wextra -Werror

PROGS = server client

all: $(PROGS)


server.o: utile.h
client.o: utile.h

clean:
	rm -f *.o $(PROGS)