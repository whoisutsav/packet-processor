#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "packet_processor.h"
#include "queue.h"
#include "utils/packetsource.h"
#include "utils/fingerprint.h"

volatile Packet_t* (*getSourceFunction(Packet_type ptype))(PacketSource_t *, int) {
  switch(ptype) {
     case UNIFORM:
       return getUniformPacket; 
       break;
     case EXPONENTIAL:
       return getExponentialPacket;
       break;
     case CONSTANT:
     default:
       return getConstantPacket;
       break;
  } 
}

void process_serial(int n, int t, Packet_type ptype, long w, int seed, int debug, long *** debug_output) {
  int nsources = n-1;
  PacketSource_t * source = createPacketSource(w, nsources, seed); 
  Packet_t * packet;
  volatile Packet_t* (*sourceFunc)(PacketSource_t*, int) = getSourceFunction(ptype);

  long ** debug_matrix = NULL;
  if(debug) {
    debug_matrix = malloc(nsources * sizeof(long *));
    for (int i=0; i<nsources; i++) {
      debug_matrix[i] = malloc(t * sizeof(long));
    }
  }

  long result;
  for(int i=0; i < t; i++) {
    for(int j=0; j < nsources; j++) {
       packet = (Packet_t *) (*sourceFunc)(source, j);
       result = getFingerprint(packet->iterations, packet->seed);
       free(packet);
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
  Packet_t * packet;
  while(i < myarg->t) {
    if(dequeue(myarg->q, &packet) != -1) {
      result = getFingerprint(packet->iterations, packet->seed);
      free(packet);
      if (myarg->debug) myarg->debug_matrix[myarg->id][i] = result;
      i++;
    }
  }

  return NULL;
}

void process_parallel(int n, int t, Packet_type ptype, int queue_depth, long w, int seed, int debug, long *** debug_output) {
  int nsources = n - 1;
  pthread_t threads[nsources];
  thread_arg args[nsources];

  long ** debug_matrix = NULL;
  if(debug) {
    debug_matrix = malloc(nsources * sizeof(long *));
    for (int i=0; i<nsources; i++) {
      debug_matrix[i] = malloc(t * sizeof(long));
    }
  }

  int rc;
  for(int i=0; i<nsources; i++) {
    args[i].id = i;
    args[i].q = create_queue(queue_depth);
    args[i].t = t;
    args[i].debug = debug;
    args[i].debug_matrix = debug_matrix;
    rc = pthread_create(&threads[i], NULL, thread_func, (void*) &args[i]); 
    if (rc){
      printf("ERROR; pthread_create() returned %d\n", rc);
      exit(-1);
    }
  }
  
  PacketSource_t * source = createPacketSource(w, nsources, seed); 
  Packet_t * packet;
  volatile Packet_t* (*sourceFunc)(PacketSource_t*, int) = getSourceFunction(ptype);

  for(int j=0; j < t; j++) {
    for(int k=0; k < nsources; k++) {
       packet = (Packet_t*) (*sourceFunc)(source, k);
       while(enqueue(args[k].q, packet) == -1) {}
    } 
  } 

  for(int m=0; m<nsources; m++) {
    pthread_join(threads[m], NULL);
  }

  if (debug) *debug_output = debug_matrix;
  deletePacketSource(source);
}

void process_serial_queue(int n, int t, Packet_type ptype, int queue_depth, long w, int seed, int debug, long *** debug_output) {
  int nsources = n-1;
  queue * queues[nsources];

  for(int i=0; i<nsources; i++) {
    queues[i] = create_queue(queue_depth);
  }
  
  long ** debug_matrix = NULL;
  if(debug) {
    debug_matrix = malloc(nsources * sizeof(long *));
    for (int i=0; i<nsources; i++) {
      debug_matrix[i] = malloc(t * sizeof(long));
    }
  }
  
  PacketSource_t * source = createPacketSource(w, nsources, seed); 
  Packet_t *in_packet, *out_packet;
  volatile Packet_t* (*sourceFunc)(PacketSource_t*, int) = getSourceFunction(ptype);

  long result;
  for(int i=0; i < t; i++) {
    for(int j=0; j < nsources; j++) {
       in_packet = (Packet_t *) (*sourceFunc)(source, j);
       enqueue(queues[j], in_packet);
       dequeue(queues[j], &out_packet); 
       result = getFingerprint(out_packet->iterations, out_packet->seed);
       free(out_packet);
       if (debug) debug_matrix[j][i] = result;
    } 
  } 

  if (debug) *debug_output = debug_matrix;
  deletePacketSource(source);
}


