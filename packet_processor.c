#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "packet_processor.h"
#include "queue.h"
#include "packetsource.h"
#include "fingerprint.h"

void process_serial(int nsources, int npackets, int packet_type, long expected_work) {
  // TODO make id parameter modifiable
  PacketSource_t * source = createPacketSource(expected_work, nsources, 0); 
  volatile Packet_t * packet;

  for(int j=0; j < npackets; j++) {
    for(int k=0; k < nsources; k++) {
       switch(packet_type) {
          case 1:
            packet = getUniformPacket(source, k); 
            break;
          case 2:
            packet = getExponentialPacket(source, k);
            break;
          case 0:
          default:
            packet = getConstantPacket(source, expected_work, k);
            break;
       } 
       //printf("Calculating checksum of packet with iterations %ld\n", packet->iterations); 
       getFingerprint(packet->iterations, packet->seed);
    } 
  } 

  deletePacketSource(source);
}

void * thread_func(void* arg) {
  int id = ((thread_arg*) arg)->id; 
  int npackets = ((thread_arg*) arg)->npackets; 
  queue * q = ((thread_arg*) arg)->q; 
  int i=0;
  printf("Created thread %d\n", id);

  Packet_t ** packet_p = malloc(sizeof(Packet_t *));
  while(i < npackets) {
    if(dequeue(q, packet_p) != -1) {
			//printf("Thread %d dequeued packet %d\n", id, i);
      getFingerprint((*packet_p)->iterations, (*packet_p)->seed);
      i++;
    }
  }

  return NULL;
}

void process_parallel(int nworkers, int npackets, int packet_type, int queue_depth, long expected_work) {
  pthread_t threads[nworkers];
  thread_arg args[nworkers];
  int rc;

  for(int i=0; i<nworkers; i++) {
    args[i].id = i;
    args[i].q = create_queue(queue_depth);
    args[i].npackets = npackets;
    rc = pthread_create(&threads[i], NULL, thread_func, (void*) &args[i]); 
    if (rc){
      printf("ERROR; pthread_create() returned %d\n", rc);
      exit(-1);
    }
  }
  
  // TODO make id modifiable
  PacketSource_t * source = createPacketSource(expected_work, nworkers, 0); 
  Packet_t * packet;

  for(int j=0; j < npackets; j++) {
    for(int k=0; k < nworkers; k++) {
       switch(packet_type) {
          case 1:
            packet = (Packet_t *) getUniformPacket(source, k); 
            break;
          case 2:
            packet = (Packet_t *) getExponentialPacket(source, k);
            break;
          case 0:
          default:
            packet = (Packet_t *) getConstantPacket(source, expected_work, k);
            break;
       } 
			 //printf("Enqueuing source %d, packet %d\n", k, j);
			 //fflush(stdout);
       while(enqueue(args[k].q, packet) == -1) {}
    } 
  } 

  for(int m=0; m<nworkers; m++) {
    pthread_join(threads[m], NULL);
  }

  deletePacketSource(source);
}

void process_serial_queue(int nsources, int npackets, int packet_type, int queue_depth, long expected_work) {
  queue * queues[nsources];

  for(int i=0; i<nsources; i++) {
    queues[i] = create_queue(queue_depth);
  }
  
  // TODO make id modifiable
  PacketSource_t * source = createPacketSource(expected_work, nsources, 0); 
  Packet_t * packet;
  Packet_t ** packet_p = malloc(sizeof(Packet_t *));

  for(int j=0; j < npackets; j++) {
    for(int k=0; k < nsources; k++) {
       switch(packet_type) {
          case 1:
            packet = (Packet_t *) getUniformPacket(source, k); 
            break;
          case 2:
            packet = (Packet_t *) getExponentialPacket(source, k);
            break;
          case 0:
          default:
            packet = (Packet_t *) getConstantPacket(source, expected_work, k);
            break;
       } 
       enqueue(queues[k], packet);
       dequeue(queues[k], packet_p); 
       getFingerprint((*packet_p)->iterations, (*packet_p)->seed);
    } 
  } 

  deletePacketSource(source);
}


