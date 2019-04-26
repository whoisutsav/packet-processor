#include <stdio.h>
#include "packet_processor.h"

int main(int argc, char* argv[]) {
  if (argc < 6) {
    printf("Usage: main_parallel [NUM_THREADS] [NUM_PACKETS] [PACKET_TYPE_ID] [QUEUE_DEPTH] [EXPECTED_WORK]\n");
    return 1;
  }

  process_parallel((atoi(argv[1]) - 1), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atol(argv[5]), 1, 0, NULL); 

  return 0;
}
