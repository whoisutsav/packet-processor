#include <stdio.h>
#include <stdlib.h>
#include "packetsource.h"
#include "fingerprint.h"

int main(int argc, char* argv[]) {
  int num_sources, num_packets, packet_type;
  short id;
  long work;

  if (argc < 5) {
    printf("Usage: serial_main [NUM_SOURCES] [NUM_PACKETS] [EXPECTED_WORK] [PACKET_TYPE] [EXECUTION_ID]\n");
  }

  num_sources = atoi(argv[1]);
  num_packets = atoi(argv[2]);
  work = atoi(argv[3]);
  packet_type = atoi(argv[4]);
  id = (argc == 6) ? (short) atoi(argv[5]) : 0; 

  PacketSource_t * source = createPacketSource(work, num_sources, id); 

  Packet_t * packet;
  for(int j=0; j < num_packets; j++) {
    for(int k=0; k < num_sources; k++) {
       switch(packet_type) {
          case 1:
            packet = getUniformPacket(source, k); 
            break;
          case 2:
            packet = getExponentialPacket(source, k);
            break;
          case 0:
          default:
            packet = getConstantPacket(source, work, k);
            break;
       } 
       printf("Calculating checksum of packet with iterations %dl\n", packet->iterations); 
       getFingerprint(packet->iterations, packet->seed);
    } 
  } 

  deletePacketSource(source);
  return 0;
}
