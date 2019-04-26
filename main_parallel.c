#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "packetsource.h"
#include "fingerprint.h"
#include "queue.h"


typedef struct _t_arg {
  int id;
  int num_packets;
  queue * q;
} t_arg;

void * t_func(void* arg) {
  int id = ((t_arg*) arg)->id; 
  int num_packets = ((t_arg*) arg)->num_packets; 
  queue * q = ((t_arg*) arg)->q; 
  int i=0;
  printf("Created thread %d\n", id);

  Packet_t ** packet_p = malloc(sizeof(Packet_t *));
  while(i < num_packets) {
    if(dequeue(q, packet_p) != -1) {
			//printf("Thread %d dequeued packet %d\n", id, i);
      getFingerprint((*packet_p)->iterations, (*packet_p)->seed);
      i++;
    }
  }

  return NULL;
}

int main(int argc, char* argv[]) {
  int num_worker_threads, num_packets, packet_type, queue_depth;
  short id;
  long work;

  if (argc < 6) {
    printf("Usage: parallel_main [NUM_THREADS] [NUM_PACKETS] [QUEUE_DEPTH] [EXPECTED_WORK] [PACKET_TYPE] [EXECUTION_ID]\n");
    return 1;
  }

  num_worker_threads = atoi(argv[1]) - 1;
  num_packets = atoi(argv[2]);
  queue_depth = atoi(argv[3]);
  work = atoi(argv[4]);
  packet_type = atoi(argv[5]);
  id = (argc == 7) ? (short) atoi(argv[6]) : 0; 

  pthread_t threads[num_worker_threads];
  t_arg args[num_worker_threads];
  int rc;

  for(int i=0; i<num_worker_threads; i++) {
    args[i].id = i;
    args[i].q = create_queue(queue_depth);
    args[i].num_packets = num_packets;
    rc = pthread_create(&threads[i], NULL, t_func, (void*) &args[i]); 
    if (rc){
      printf("ERROR; pthread_create() returned %d\n", rc);
      exit(-1);
    }
  }

  PacketSource_t * source = createPacketSource(work, num_worker_threads, id); 
  Packet_t * packet;

  for(int j=0; j < num_packets; j++) {
    for(int k=0; k < num_worker_threads; k++) {
       switch(packet_type) {
          case 1:
            packet = (Packet_t *) getUniformPacket(source, k); 
            break;
          case 2:
            packet = (Packet_t *) getExponentialPacket(source, k);
            break;
          case 0:
          default:
            packet = (Packet_t *) getConstantPacket(source, work, k);
            break;
       } 
			 //printf("Enqueuing source %d, packet %d\n", k, j);
			 //fflush(stdout);
       while(enqueue(args[k].q, packet) == -1) {}
    } 
  } 

	printf("Done dispatching all packets\n");

  for(int m=0; m<num_worker_threads; m++) {
    pthread_join(threads[m], NULL);
  }

  deletePacketSource(source);
  return 0;
}
