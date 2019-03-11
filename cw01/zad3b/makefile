optimization = O0
compile = gcc -Wall -std=c11 -ggdb
commands = search_directory tmpFile allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 allocate_memory tmpFile delete_block 0 




all_with_optimization: static shared dynamic
all:
	make all_with_optimization optimization=O0
	make test optimization=O0
	make all_with_optimization optimization=O3
	make test optimization=O3
	make all_with_optimization optimization=Os
	make test optimization=Os
	

static:
	$(compile) -c findLib.c
	ar rcs findLib.a findLib.o
	$(compile) -$(optimization) main.c findLib.a -o main_static

shared:
	$(compile) -c -fPIC findLib.c
	$(compile) -shared -fPIC -o findLib.so findLib.o
	$(compile) -$(optimization) -Wl,-rpath,. -L. findLib.so main.c -o main_shared

dynamic:
	$(compile) -c -fPIC findLib.c
	$(compile) -shared -fPIC -o findLib.so findLib.o
	$(compile) -$(optimization) main.c -o main_dynamic -ldl -D DLL

test:
	./main_static test results3b.txt $(optimization)_STATIC_LARGE_DATA create_array 1000 set_find_parameters / \"*\" $(commands)
	./main_static test results3b.txt $(optimization)_STATIC_MEDIUM_DATA create_array 1000 set_find_parameters /home \"*\" $(commands)
	./main_static test results3b.txt $(optimization)_STATIC_SMALL_DATA create_array 1000 set_find_parameters .. \"*\" $(commands)
	./main_shared test results3b.txt $(optimization)_SHARED_LARGE_DATA create_array 1000 set_find_parameters / \"*\" $(commands)
	./main_shared test results3b.txt $(optimization)_SHARED_MEDIUM_DATA create_array 1000 set_find_parameters /home \"*\" $(commands)
	./main_shared test results3b.txt $(optimization)_SHARED_SMALL_DATA create_array 1000 set_find_parameters .. \"*\" $(commands)
	./main_dynamic test results3b.txt $(optimization)_DYNAMIC_LARGE_DATA create_array 1000 set_find_parameters / \"*\" $(commands)
	./main_dynamic test results3b.txt $(optimization)_DYNAMIC_MEDIUM_DATA create_array 1000 set_find_parameters /home \"*\" $(commands)
	./main_dynamic test results3b.txt $(optimization)_DYNAMIC_SMALL_DATA create_array 1000 set_find_parameters .. \"*\" $(commands)


clean:
	rm -f *.o
	rm -f *.so
	rm -f *.a
	rm -f tmpFile
	rm -f main_shared
	rm -f main_static
	rm -f main_dynamic
