GCC_FLAGS=-std=c99 -pthread -Wno-int-conversion
# TCC_FLAGS=-std=c99 -pthread -latomic
TCC_FLAGS=-std=c99 -pthread

.PHONY: all clean

all: gcc-tests tcc-tests

clean:
	rm gcc-test1 gcc-test2
	rm tcc-test1 tcc-test2

gcc-tests: test1.c
	gcc ${GCC_FLAGS} test1.c -o gcc-test1
	gcc ${GCC_FLAGS} test2.c -o gcc-test2

tcc-tests: test1.c
	# echo "-latomic requires package gcc-aarch64-linux-gnu on Ubuntu"
	# tcc test1.c -o test1
	tcc ${TCC_FLAGS} test1.c -o tcc-test1
	tcc ${TCC_FLAGS} test2.c -o tcc-test2
