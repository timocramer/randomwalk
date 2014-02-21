CFLAGS=-O3 -Wall -Wextra -pipe

.PHONY: clean

randomwalk: randomwalk.c
	$(CC) -o $@ $(CFLAGS) $<

clean:
	$(RM) randomwalk
