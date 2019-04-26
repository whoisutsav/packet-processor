#ifndef _QUEUE_H
#define _QUEUE_H

#include "packetsource.h"

typedef struct _queue {
  volatile long head;
  volatile long tail;
  int depth;
  Packet_t ** items;
} queue;

queue * create_queue(int depth); 
int enqueue(queue * q, Packet_t* item); 
int dequeue(queue * q, Packet_t ** p); 

#endif /* _QUEUE_H */
