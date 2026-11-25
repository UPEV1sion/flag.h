#/usr/bin/env sh

set -xe

tcc test.c -o test && ./test

gcc -Wall -Wextra -Werror -std=c99 -pedantic -O3 -o test test.c 
./test
rm test

clang -Wall -Wextra -Werror -std=c99 -pedantic -O3 -o test test.c
./test 
rm test
