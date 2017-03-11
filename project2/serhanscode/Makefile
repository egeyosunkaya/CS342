
CC	:= gcc
CFLAGS := -g -Wall

TARGETS :=  libtlib.a  app1

# Make sure that 'all' is the first target
all: $(TARGETS)

clean:
	rm -rf core  *.o *.out *~ $(TARGETS)

TLIB_SRC :=  tlib.c
TLIB_OBJS := $(TLIB_SRC:.c=.o)

libtlib.a: $(TLIB_OBJS)
	ar rcs $@ $(TLIB_OBJS)

TLIB_LIB :=  -L . -l tlib

tlib.o: tlib.c tlib.h
	gcc -c $(CFLAGS) -o $@ tlib.c

app1.o: app1.c  tlib.h
	gcc -c $(CFLAGS) -o $@ app1.c

app1: app1.o libtlib.a
	gcc $(CFLAGS) -o $@ app1.o  $(TLIB_LIB)