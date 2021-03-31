CC       = gcc
CPPFLAGS =
CFLAGS   = -Wall -Wextra -O2 -std=c11
LDFLAGS  =

OBJ =\
    numbers.o\
    append.o\
    sorts.o\
    allocs.o\
	similar_lines.o
 
.PHONY: all clean
 
all: similar_lines
 
.c.o:
	$(CC) -c $(INCLUDES) $(CFLAGS) $<
 
similar_lines: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o similar_lines

clean:
	rm -f *.o similar_lines