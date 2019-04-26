#include <stdio.h>
#include "packet_processor.h"

int main(int argc, char* argv[]) {
  if (argc < 5) {
    printf("Usage: main_serial [NUM_SOURCES] [NUM_PACKETS] [PACKET_TYPE_ID] [EXPECTED_WORK]\n");
    return 1;
  }

  process_serial(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atol(argv[4]), 1, 0, NULL); 

  return 0;
}
