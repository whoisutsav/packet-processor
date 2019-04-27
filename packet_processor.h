#ifndef _PACKET_PROCESSOR_H
#define _PACKET_PROCESSOR_H

#include "queue.h"

typedef struct _thread_arg {
  int id;
  int t;
  int debug;
  long ** debug_matrix;
  queue * q;
} thread_arg;

typedef enum _ptype {CONSTANT, UNIFORM, EXPONENTIAL} Packet_type;

void process_serial(int nsources, int npackets, Packet_type ptype, long expected_work, int seed, int debug, long *** debug_output);
void process_parallel(int nworkers, int npackets, Packet_type ptype, int queue_depth, long expected_work, int seed, int debug, long *** debug_output);
void process_serial_queue(int nsources, int npackets, Packet_type ptype, int queue_depth, long expected_work, int seed, int debug, long *** debug_output);

#endif /* _PACKET_PROCESSOR_H */
