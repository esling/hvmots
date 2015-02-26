CC=/usr/local/bin/gcc
CFLAGS=-Wall -O2 -O3 -fopenmp -static -g
LDFLAGS=-fopenmp -O2 -O3 -g -lm -lz libfftw3.a fpli_hv.a
EXEC=hvMots
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

all: $(EXEC)

hvMots: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean distclean

clean:
	rm -rf *.o

distclean: clean
	rm -rf $(EXEC)
