#include <stdio.h>
#include "packet_processor.h"

int main(int argc, char* argv[]) {
  if (argc < 6) {
    printf("Usage: main_serial_queue [NUM_SOURCES] [NUM_PACKETS] [PACKET_TYPE_ID] [QUEUE_DEPTH] [EXPECTED_WORK]\n");
    return 1;
  }

  process_serial_queue(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atol(argv[5])); 

  return 0;
}
