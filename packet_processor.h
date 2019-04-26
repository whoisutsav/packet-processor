#ifndef _PACKET_PROCESSOR_H
#define _PACKET_PROCESSOR_H

#include "queue.h"

typedef struct _thread_arg {
  int id;
  int npackets;
  queue * q;
} thread_arg;

void process_serial(int nsources, int npackets, int packet_type, long expected_work);
void process_parallel(int nworkers, int npackets, int packet_type, int queue_depth, long expected_work);
void process_serial_queue(int nsources, int npackets, int packet_type, int queue_depth, long expected_work);

#endif /* _PACKET_PROCESSOR_H */
