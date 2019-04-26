all: serial parallel serial_queue

serial: utils/packetsource.c utils/packetsource.h utils/fingerprint.c utils/fingerprint.h utils/generators.c utils/generators.h utils/crc32.c utils/crc32.h queue.c queue.h packet_processor.c packet_processor.h main_serial.c
	gcc -Wall -Werror -O3 -pthread main_serial.c utils/packetsource.c utils/fingerprint.c utils/generators.c utils/crc32.c queue.c packet_processor.c -lm -o main_serial

parallel: utils/packetsource.c utils/packetsource.h utils/fingerprint.c utils/fingerprint.h utils/generators.c utils/generators.h utils/crc32.c utils/crc32.h queue.c queue.h packet_processor.c packet_processor.h main_parallel.c 
	gcc -Wall -Werror -O3 -pthread main_parallel.c utils/packetsource.c utils/fingerprint.c utils/generators.c utils/crc32.c queue.c packet_processor.c -lm -o main_parallel

serial_queue: utils/packetsource.c utils/packetsource.h utils/fingerprint.c utils/fingerprint.h utils/generators.c utils/generators.h utils/crc32.c utils/crc32.h queue.c queue.h packet_processor.c packet_processor.h main_serial_queue.c 
	gcc -Wall -Werror -O3 -pthread main_serial_queue.c utils/packetsource.c utils/fingerprint.c utils/generators.c utils/crc32.c queue.c packet_processor.c -lm -o main_serial_queue

clean:
	rm -f main_serial main_parallel main_serial_queue
