#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "packet_processor.h"
#include "queue.h"
#include "utils/packetsource.h"
#include "utils/fingerprint.h"

void process_serial(int nsources, int npackets, Packet_type ptype, long expected_work, int seed, int debug, long *** debug_output) {
  PacketSource_t * source = createPacketSource(expected_work, nsources, seed); 
  volatile Packet_t * packet;

  long ** debug_matrix = NULL;
  if(debug) {
    debug_matrix = malloc(nsources * sizeof(long *));
    for (int i=0; i<nsources; i++) {
      debug_matrix[i] = malloc(npackets * sizeof(long));
    }
  }

  long result;
  for(int i=0; i < npackets; i++) {
    for(int j=0; j < nsources; j++) {
       switch(ptype) {
          case 1:
            packet = getUniformPacket(source, j); 
            break;
          case 2:
            packet = getExponentialPacket(source, j);
            break;
          case 0:
          default:
            packet = getConstantPacket(source, j);
            break;
       } 
       result = getFingerprint(packet->iterations, packet->seed);
       if (debug) debug_matrix[j][i] = result;
    } 
  } 

  if (debug) *debug_output = debug_matrix;
  deletePacketSource(source);
}

void * thread_func(void* arg) {
  thread_arg* myarg = (thread_arg*) arg;

  int i=0;
  long result;
  Packet_t * packet_p;
  while(i < myarg->npackets) {
    if(dequeue(myarg->q, &packet_p) != -1) {
      result = getFingerprint(packet_p->iterations, packet_p->seed);
      if (myarg->debug) myarg->debug_matrix[myarg->id][i] = result;
      i++;
    }
  }

  return NULL;
}

void process_parallel(int nworkers, int npackets, Packet_type ptype, int queue_depth, long expected_work, int seed, int debug, long *** debug_output) {
  pthread_t threads[nworkers];
  thread_arg args[nworkers];
  int rc;

  long ** debug_matrix = NULL;
  if(debug) {
    debug_matrix = malloc(nworkers * sizeof(long *));
    for (int i=0; i<nworkers; i++) {
      debug_matrix[i] = malloc(npackets * sizeof(long));
    }
  }

  for(int i=0; i<nworkers; i++) {
    args[i].id = i;
    args[i].q = create_queue(queue_depth);
    args[i].npackets = npackets;
    args[i].debug = debug;
    args[i].debug_matrix = debug_matrix;
    rc = pthread_create(&threads[i], NULL, thread_func, (void*) &args[i]); 
    if (rc){
      printf("ERROR; pthread_create() returned %d\n", rc);
      exit(-1);
    }
  }
  
  PacketSource_t * source = createPacketSource(expected_work, nworkers, seed); 
  Packet_t * packet;

  for(int j=0; j < npackets; j++) {
    for(int k=0; k < nworkers; k++) {
       switch(ptype) {
          case 1:
            packet = (Packet_t *) getUniformPacket(source, k); 
            break;
          case 2:
            packet = (Packet_t *) getExponentialPacket(source, k);
            break;
          case 0:
          default:
            packet = (Packet_t *) getConstantPacket(source, k);
            break;
       } 
       while(enqueue(args[k].q, packet) == -1) {}
    } 
  } 

  for(int m=0; m<nworkers; m++) {
    pthread_join(threads[m], NULL);
  }

  if (debug) *debug_output = debug_matrix;
  deletePacketSource(source);
}

void process_serial_queue(int nsources, int npackets, Packet_type ptype, int queue_depth, long expected_work, int seed, int debug, long *** debug_output) {
  queue * queues[nsources];

  for(int i=0; i<nsources; i++) {
    queues[i] = create_queue(queue_depth);
  }
  
  long ** debug_matrix = NULL;
  if(debug) {
    debug_matrix = malloc(nsources * sizeof(long *));
    for (int i=0; i<nsources; i++) {
      debug_matrix[i] = malloc(npackets * sizeof(long));
    }
  }
  
  PacketSource_t * source = createPacketSource(expected_work, nsources, seed); 
  Packet_t *in_packet, *out_packet;

  long result;
  for(int i=0; i < npackets; i++) {
    for(int j=0; j < nsources; j++) {
       switch(ptype) {
          case 1:
            in_packet = (Packet_t *) getUniformPacket(source, j); 
            break;
          case 2:
            in_packet = (Packet_t *) getExponentialPacket(source, j);
            break;
          case 0:
          default:
            in_packet = (Packet_t *) getConstantPacket(source, j);
            break;
       } 
       enqueue(queues[j], in_packet);
       dequeue(queues[j], &out_packet); 
       result = getFingerprint(out_packet->iterations, out_packet->seed);
       if (debug) debug_matrix[j][i] = result;
    } 
  } 

  if (debug) *debug_output = debug_matrix;
  deletePacketSource(source);
}


