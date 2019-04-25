all: serial

serial: serial_main.c packetsource.c packetsource.h fingerprint.c fingerprint.h generators.c generators.h crc32.c crc32.h 
	gcc -Wall -Werror -O3 -pthread serial_main.c packetsource.c fingerprint.c generators.c crc32.c -lm -o serial_main

#parallel: parallel_main.c packetsource.c packetsource.h fingerprint.c fingerprint.h generators.c generators.h crc32.c crc32.h queue.c queue.h 
#	gcc -Wall -Werror -O3 -pthread serial_main.c packetsource.c fingerprint.c generators.c crc32.c queue.c -lm -o serial_main

clean:
	rm -f serial_main
