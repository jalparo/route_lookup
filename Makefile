SRC = my_route_lookup.c io.c io.h
CFLAGS = -Wall -O3

all: my_route_lookup

my_route_lookup: $(SRC)
	gcc $(CFLAGS) $(SRC) -o my_route_lookup -lm

.PHONY: clean

clean:
	rm -f my_route_lookup

