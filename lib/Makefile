CC      = gcc
CFLAGS  = -Wall -g -std=c11 -pedantic
LIB     = libstart_menu.a
LIBOBJS = start_menu.o
LDLIBS  = -lSDL2 -lSDL2_ttf -lSDL2_mixer
BUILD   = $(LIB)

.SUFFIXES: .c .o

all: $(BUILD)

$(LIB): $(LIBOBJS)
	ar rcs $(LIB) $(LIBOBJS)

start_menu.o: start_menu.h pong.h

clean:
	rm -f $(wildcard *.o *.a)
	rm -f $(BUILD)
	rm -f core
	rm -f a.out

