CC     = gcc
CFLAGS = -Wall -g -std=c11 -pedantic -Ilib
LDLIBS = -Llib -lstart_menu -lSDL2 -lSDL2_ttf -lSDL2_mixer -lm
BUILD  = libs pong 

.SUFFIXES: .c .o

all: $(BUILD)

pong: pong.o

clean:
	rm -f $(wildcard *.o)
	rm -f $(BUILD)
	rm -f core
	rm -f a.out
	cd lib; make clean

libs:
	cd lib; make
