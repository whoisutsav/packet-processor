all: serial_main.c
	gcc -Wall -Werror -O3 -pthreads serial_main.c -o serial_main

clean:
	rm serial_main
