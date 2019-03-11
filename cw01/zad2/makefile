compile = gcc -Wall -std=c11 -ggdb


all: static shared clean

static:
	$(compile) -c findLib.c
	ar rcs findLib.a findLib.o
	$(compile) -o main_static main.c findLib.a -I .

shared:
	$(compile) -c -fPIC findLib.c
	$(compile) -shared -fPIC -o findLib.so findLib.o
	$(compile) -Wl,-rpath,. -L. findLib.so main.c -o main_shared

clean:
	rm -f *.o

tests:
	./main_static create_array 10 set_find_parameters /home *build search_directory tmpFile allocate_memory tmpFile allocate_memory tmpFile delete_block 0 delete_block 1
	./main_shared create_array 10 set_find_parameters /home *build search_directory tmpFile allocate_memory tmpFile allocate_memory tmpFile delete_block 0 delete_block 1
