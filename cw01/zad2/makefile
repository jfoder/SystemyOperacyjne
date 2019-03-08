compile = gcc -Wall -std=c11 -ggdb


all: static shared clean

static:
	$(compile) -c findLib.c
	ar rcs findLib.a findLib.o

shared:
	$(compile) -c -fPIC findLib.c
	$(compile) -shared -fPIC -o findLib.so findLib.o

clean:
	rm -f *.o
