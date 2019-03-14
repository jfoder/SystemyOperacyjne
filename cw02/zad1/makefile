CC = gcc -Wall -std=c11 -ggdb
timeResults = wyniki.txt

all: clean compile clean

compile:
	$(CC) main.c -o main
	make test byte=1 largesort=8000 smallsort=4000 largecopy=10000000 smallcopy=2000000
	make clean
	echo 1 byte completed
	make test byte=2 largesort=8000 smallsort=4000 largecopy=5000000 smallcopy=1000000
	make clean
	echo 2 byte completed
	make test byte=4 largesort=8000 smallsort=4000 largecopy=2000000 smallcopy=500000
	make clean
	echo 4 byte completed
	make test byte=8 largesort=8000 smallsort=4000 largecopy=1000000 smallcopy=300000
	make clean
	echo 8 byte completed
	make test byte=16 largesort=8000 smallsort=4000 largecopy=500000 smallcopy=200000
	make clean
	echo 16 byte completed
	make test byte=32 largesort=8000 smallsort=4000 largecopy=300000 smallcopy=150000
	make clean
	echo 32 byte completed
	make test byte=64 largesort=7000 smallsort=3500 largecopy=200000 smallcopy=100000
	make clean
	echo 64 byte completed
	make test byte=128 largesort=6500 smallsort=3300 largecopy=100000 smallcopy=50000
	make clean
	echo 128 byte completed
	make test byte=256 largesort=6000 smallsort=3000 largecopy=50000 smallcopy=25000
	make clean
	echo 256 byte completed
	make test byte=512 largesort=5800 smallsort=2900 largecopy=30000 smallcopy=15000
	make clean
	echo 512 byte completed
	make test byte=1024 largesort=5500 smallsort=2500 largecopy=15000 smallcopy=10000
	make clean
	echo 1024 byte completed
	make test byte=2048 largesort=5000 smallsort=2300 largecopy=8000 smallcopy=4000
	make clean
	echo 2048 byte completed
	make test byte=4096 largesort=4500 smallsort=2000 largecopy=5000 smallcopy=2000
	make clean
	echo 4096 byte completed
	make test byte=8192 largesort=4000 smallsort=2000 largecopy=3000 smallcopy=1500
	make clean
	echo 8192 byte completed

test:
	./main generate randoms_sys.tmp $(largesort) $(byte)
	./main copy_sys randoms_sys.tmp randoms_lib.tmp $(byte) $(largesort)
	./main test $(timeResults) SORT_SYS_LARGE_$(byte)_BYTE sort_sys randoms_sys.tmp $(byte)
	./main test $(timeResults) SORT_LIB_LARGE_$(byte)_BYTE sort_lib randoms_lib.tmp $(byte)
	./main generate randoms_sys.tmp $(smallsort) $(byte)
	./main copy_sys randoms_sys.tmp randoms_lib.tmp $(byte) $(smallsort)
	./main test $(timeResults) SORT_SYS_SMALL_$(byte)_BYTE sort_sys randoms_sys.tmp $(byte)
	./main test $(timeResults) SORT_LIB_SMALL_$(byte)_BYTE sort_lib randoms_lib.tmp $(byte)
	./main generate randoms.tmp $(largecopy) $(byte)
	./main test $(timeResults) COPY_SYS_LARGE_$(byte)_BYTE copy_sys randoms.tmp randoms_sys.tmp $(byte) $(largecopy)
	./main test $(timeResults) COPY_LIB_LARGE_$(byte)_BYTE copy_lib randoms randoms_lib.tmp $(byte) $(largecopy)
	./main generate randoms.tmp $(smallcopy) $(byte)
	./main test $(timeResults) COPY_SYS_SMALL_$(byte)_BYTE copy_sys randoms.tmp randoms_sys.tmp $(byte) $(smallcopy)
	./main test $(timeResults) COPY_LIB_SMALL_$(byte)_BYTE copy_lib randoms randoms_lib.tmp $(byte) $(smallcopy)
clean:
	rm -f $(randomResults)_lib
	rm -f $(randomResults)_sys
	rm -f *.tmp
