all: serial parallel serial_queue

serial: packetsource.c packetsource.h fingerprint.c fingerprint.h generators.c generators.h crc32.c crc32.h queue.c queue.h packet_processor.c packet_processor.h main_serial.c
	gcc -Wall -Werror -O3 -pthread main_serial.c packetsource.c fingerprint.c generators.c crc32.c queue.c packet_processor.c -lm -o main_serial

parallel: packetsource.c packetsource.h fingerprint.c fingerprint.h generators.c generators.h crc32.c crc32.h queue.c queue.h packet_processor.c packet_processor.h main_parallel.c 
	gcc -Wall -Werror -O3 -pthread main_parallel.c packetsource.c fingerprint.c generators.c crc32.c queue.c packet_processor.c -lm -o main_parallel

serial_queue: packetsource.c packetsource.h fingerprint.c fingerprint.h generators.c generators.h crc32.c crc32.h queue.c queue.h packet_processor.c packet_processor.h main_serial_queue.c 
	gcc -Wall -Werror -O3 -pthread main_serial_queue.c packetsource.c fingerprint.c generators.c crc32.c queue.c packet_processor.c -lm -o main_serial_queue

clean:
	rm -f main_serial main_parallel
