compile = gcc -Wall -std=c11 -ggdb


all: static shared dynamic

static:
	$(compile) -c findLib.c
	ar rcs findLib.a findLib.o
	$(compile) main.c findLib.a -o main_static

shared:
	$(compile) -c -fPIC findLib.c
	$(compile) -shared -fPIC -o findLib.so findLib.o
	$(compile) -Wl,-rpath,. -L. findLib.so main.c -o main_shared

dynamic:
	$(compile) -c -fPIC findLib.c
	$(compile) -shared -fPIC -o findLib.so findLib.o
	$(compile) main.c -o main_dynamic -ldl -D DLL

test:
	./main_static test results3a.txt STATIC_LARGE_DATA create_array 1000 set_find_parameters / \"*\" search_directory tmpFile allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 
	./main_static test results3a.txt STATIC_MEDIUM_DATA create_array 1000 set_find_parameters /home \"*\" search_directory tmpFile allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 
	./main_static test results3a.txt STATIC_SMALL_DATA create_array 1000 set_find_parameters .. \"*\" search_directory tmpFile allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 
	./main_shared test results3a.txt SHARED_LARGE_DATA create_array 1000 set_find_parameters / \"*\" search_directory tmpFile allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 
	./main_shared test results3a.txt SHARED_MEDIUM_DATA create_array 1000 set_find_parameters /home \"*\" search_directory tmpFile allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 
	./main_shared test results3a.txt SHARED_SMALL_DATA create_array 1000 set_find_parameters .. \"*\" search_directory tmpFile allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 
	./main_dynamic test results3a.txt DYNAMIC_LARGE_DATA create_array 1000 set_find_parameters / \"*\" search_directory tmpFile allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 
	./main_dynamic test results3a.txt DYNAMIC_MEDIUM_DATA create_array 1000 set_find_parameters /home \"*\" search_directory tmpFile allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 
	./main_dynamic test results3a.txt DYNAMIC_SMALL_DATA create_array 1000 set_find_parameters .. \"*\" search_directory tmpFile allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 

clean:
	rm -f *.o
	rm -f *.so
	rm -f *.a
	rm -f tmpFile
	rm -f main_shared
	rm -f main_static
	rm -f main_dynamic
