#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "packet_processor.h"
#include "utils/stopwatch.h"

int doublecmp(const void *a, const void *b) {
  double result = *((double*) a) - *((double*) b);
  if (result == 0) return 0;
  else return (result > 0) ? 1 : -1;
}

double get_median(double arr[], int n) {
  qsort(arr, n, sizeof(double), doublecmp); 

  if ((n%2) == 1) 
    return arr[n/2];
  else 
    return (arr[(n/2)-1] + arr[n/2])/2; 
}

void print_2d_matrix(double ** m, int rows, int cols) {
  for(int i=0; i<rows; i++) {
    for(int j=0; j<cols; j++) {
      printf("%f\t", m[i][j]);
    }
    printf("\n");
  }
}

void parallel_overhead_test() {
  int n_values[3] = {2, 9, 14};
  long w_values[6] = {25, 50, 100, 200, 400, 800};
	int num_trials = 11;

  double ** serial_queue_output = malloc(6 * sizeof(double*));
  double ** ratio_output = malloc(6 * sizeof(double*));
  for(int i=0; i<6; i++) {
    serial_queue_output[i] = malloc(3 * sizeof(double));
    ratio_output[i] = malloc(3 * sizeof(double));
  }

  int t;
  double * serial_trials = malloc(num_trials * sizeof(double));
  double * serial_queue_trials = malloc(num_trials*sizeof(double));
  double * ratio_trials = malloc(num_trials*sizeof(double));
  StopWatch_t *stopWatch = malloc(sizeof(StopWatch_t));

  for(int i=0; i<6; i++) {
    for (int j=0; j<3; j++) {
     	t = pow(2, 24)/(n_values[j]*w_values[i]);
			for(int k=0; k<num_trials; k++) {
				startTimer(stopWatch);
				process_serial(n_values[j], t, UNIFORM, w_values[i], i*j*k, 0, NULL);
				stopTimer(stopWatch);
				serial_trials[k] = getElapsedTime(stopWatch); 

				startTimer(stopWatch);
				process_serial_queue(n_values[j], t, UNIFORM, 32, w_values[i], i*j*k, 0, NULL);
				stopTimer(stopWatch);
				serial_queue_trials[k] = getElapsedTime(stopWatch); 

        ratio_trials[k] = serial_queue_trials[k]/serial_trials[k];
			}
      serial_queue_output[i][j] = get_median(serial_queue_trials, num_trials);
      ratio_output[i][j] = get_median(ratio_trials, num_trials);

    }
  }
  printf("Parallel overhead test results (serial queue times):\n");
  print_2d_matrix(serial_queue_output, 6, 3);

  printf("Parallel overhead test results (ratio times):\n");
  print_2d_matrix(ratio_output, 6, 3);
}

void dispatcher_rate_test() {
  int n_values[6] = {2, 3, 5, 9, 14, 28};
  int num_trials = 5;

  int t;
  double * trials = malloc(num_trials * sizeof(double));
  double output[6];
  StopWatch_t *stopWatch = malloc(sizeof(StopWatch_t));
  for(int i=0; i<6; i++) {
    t = pow(2,20)/(n_values[i] - 1);
    for(int j=0; j<num_trials; j++) {
      startTimer(stopWatch);
      process_parallel(n_values[i], t, UNIFORM, 32, 1, i*num_trials + j, 0, NULL); 
      stopTimer(stopWatch);
      trials[j] = getElapsedTime(stopWatch);
    }
    output[i] = ((n_values[i]-1)*t)/get_median(trials, num_trials);
  }

  printf("Dispatcher rate test results:\n");
  for(int i=0; i<6; i++) {
    printf("%f\n", output[i]);
  }
}

void speedup_test(Packet_type ptype, int num_trials) {
  int n_values[6] = {2, 3, 5, 9, 14, 28};
  int w_values[4] = {1, 2, 4, 8};
  int t = pow(2, 17);
  printf("Value of t is: %d\n", t);

  double * serial_trials = malloc(num_trials * sizeof(double));
  double * parallel_trials = malloc(num_trials * sizeof(double));
  double * ratio_trials = malloc(num_trials * sizeof(double));

  double ** output = malloc(6 * sizeof(double*));
  for(int i=0; i<6; i++) {
    output[i] = malloc(4 * sizeof(double));
  }

  
  StopWatch_t *stopWatch = malloc(sizeof(StopWatch_t));
  for(int i=0; i<6; i++) {
    for(int j=0; j<4; j++) {
      printf("Starting experiment: T=%d, W=%d\n", n_values[i], w_values[j]);
      for(int k=0; k<num_trials; k++) {
        startTimer(stopWatch);
        process_serial(n_values[i], t, ptype, w_values[j], i*j*k, 0, NULL); 
        stopTimer(stopWatch);
        serial_trials[k] = getElapsedTime(stopWatch);
        
        startTimer(stopWatch);
        process_parallel(n_values[i], t, ptype, 32, w_values[j], i*j*k, 0, NULL); 
        stopTimer(stopWatch);
        parallel_trials[k] = getElapsedTime(stopWatch);
 
        ratio_trials[k] = serial_trials[k]/parallel_trials[k];
      }
      output[i][j] = get_median(ratio_trials, num_trials);
    }
  }

  printf("Speedup test (packet type %d) results:\n", ptype);
  print_2d_matrix(output, 6, 4);
}



int main(int argc, char* argv[]) {
  printf("Running parallel overhead test...\n");
  parallel_overhead_test();

  printf("Running dispatcher rate test...\n");
  dispatcher_rate_test();

  printf("Running speedup test for CONSTANT packets...\n");
  speedup_test(CONSTANT, 5);

  printf("Running speedup test for UNIFORM packets...\n");
  speedup_test(UNIFORM, 5);

  printf("Running speedup test for EXPONENTIAL packets...\n");
  speedup_test(EXPONENTIAL, 11);

  return 0;
}
