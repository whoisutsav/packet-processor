#include <stdlib.h>
#include "queue.h"
#include "utils/packetsource.h"

queue * create_queue(int depth) {
  queue * q = malloc(sizeof(queue));
  q->head = 0;
  q->tail = 0;
  q->depth = depth;
  q->items = (Packet_t**) malloc(depth * sizeof(Packet_t *)); 

  return q;
}

int enqueue(queue * q, Packet_t* item) {
  if((q->tail-q->head) == q->depth)
    return -1; 

  q->items[q->tail % q->depth] = item;
	__sync_synchronize();

  q->tail++;
	return 0;
}

int dequeue(queue * q, Packet_t ** p) {
  if((q->tail - q->head) == 0)
    return -1;

  *p = q->items[q->head % q->depth]; 
	__sync_synchronize();

  q->head++;

  return 0;
} 

