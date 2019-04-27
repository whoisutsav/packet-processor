#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include "queue.h"
#include "packet_processor.h"
#include "utils/packetsource.h"
#include "utils/fingerprint.h"

void test_dequeue_empty() {
  queue * q = create_queue(5);
  Packet_t * packet_p;

  assert(dequeue(q, &packet_p) == -1);
}

void test_queue_fifo() {
  queue * q = create_queue(5);
  Packet_t packet1;
  Packet_t packet2;

  packet1.iterations = 1;
  packet1.seed = 1;
  packet2.iterations = 2;
  packet2.seed = 2;

  enqueue(q, &packet1);
  enqueue(q, &packet2);

  Packet_t * packet_p;
  assert(dequeue(q, &packet_p) == 0);
  assert(packet_p->iterations == 1);
  assert(packet_p->seed == 1);
}

void test_enqueue_full() {
  queue * q = create_queue(5);
  Packet_t packets[5];

  for(int i=0; i<5; i++) {
    packets[i].iterations = i;
    packets[i].seed = i;
    assert(enqueue(q, &(packets[i])) == 0);
  }

  Packet_t extra_packet;
  extra_packet.iterations = 99;
  extra_packet.seed = 99;

  assert(enqueue(q, &extra_packet) == -1);
}

void * enqueue_work(void* arg) {
  queue * q = (queue *) arg;
  Packet_t * packets = malloc(10000 * sizeof(Packet_t));

  for(int i=0; i<10000; i++) {
    packets[i].iterations = i;
    packets[i].seed = i;
    while(enqueue(q, &(packets[i])) == -1) {};
  }

  return NULL;
}

void* dequeue_work(void* arg) {
  queue * q = (queue *) arg;
  Packet_t * packets[10000];
  int i=0;

  while(i < 10000) {
    if (dequeue(q, &(packets[i])) != -1) {
      i++;
    }
  }

  for(int j=0; j<10000; j++) {
    assert(packets[j]->iterations == j);
    assert(packets[j]->seed == j);
  }

  return NULL;
}

void test_queue_parallel() {
  queue * q = create_queue(1);

  pthread_t enqueue_thread, dequeue_thread;

  pthread_create(&enqueue_thread, NULL, enqueue_work, (void *) q);
  pthread_create(&dequeue_thread, NULL, dequeue_work, (void *) q);

  pthread_join(enqueue_thread, NULL);
  pthread_join(dequeue_thread, NULL);
}

void test_serial_processor() {
  long *** debug_output = malloc(sizeof(long **));
  int nsources = 10, npackets = 10000, seed = 1;
  long expected_work = 1000;

  process_serial(nsources, npackets, CONSTANT, expected_work, seed, 1, debug_output);

  
  for (int i=0; i<nsources; i++) {
    for (int j=0; j<npackets; j++) {
      assert((*debug_output)[i][j] == getFingerprint(expected_work, seed));
    }
  }
}

void test_parallel_processor() {
  long *** debug_output_serial = malloc(sizeof(long **));
  long *** debug_output_parallel = malloc(sizeof(long**));
  int nsources = 10, npackets = 10000, seed = 123;
  long expected_work = 1000;

  process_serial(nsources, npackets, UNIFORM, expected_work, seed, 1, debug_output_serial);
  process_parallel(nsources, npackets, UNIFORM, 1, expected_work, seed, 1, debug_output_parallel);

  
  for (int i=0; i<nsources; i++) {
    for (int j=0; j<npackets; j++) {
      assert((*debug_output_serial)[i][j] == (*debug_output_parallel)[i][j]);
    }
  }
}

void test_serial_queue_processor() {
  long *** debug_output_serial = malloc(sizeof(long **));
  long *** debug_output_serial_queue = malloc(sizeof(long**));
  int nsources = 10, npackets = 10000, seed = 123;
  long expected_work = 1000;

  process_serial(nsources, npackets, UNIFORM, expected_work, seed, 1, debug_output_serial);
  process_serial_queue(nsources, npackets, UNIFORM, 1, expected_work, seed, 1, debug_output_serial_queue);

  
  for (int i=0; i<nsources; i++) {
    for (int j=0; j<npackets; j++) {
      assert((*debug_output_serial)[i][j] == (*debug_output_serial_queue)[i][j]);
    }
  }
}


int main(int argc, char* argv[]) {
  printf("test_dequeue_empty...");
  test_dequeue_empty();
  printf("passed.\n");
  
  printf("test_queue_fifo...");
  test_queue_fifo();
  printf("passed.\n");

  printf("test_enqueue_full...");
  test_enqueue_full();
  printf("passed.\n");

  printf("test_queue_parallel...");
  test_queue_parallel();
  printf("passed.\n");

  printf("test_serial_processor...");
  test_serial_processor();
  printf("passed.\n");

  printf("test_parallel_processor...");
  test_parallel_processor();
  printf("passed.\n");
  
  printf("test_serial_queue_processor...");
  test_serial_queue_processor();
  printf("passed.\n");

  printf("\nAll tests passed.\n");
  return 0;
}
