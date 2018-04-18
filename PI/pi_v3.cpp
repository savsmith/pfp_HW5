#include <pthread.h> /*used in other parts of the assignment */
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>  /* for uint64  */
#include <time.h>    /* for clock_gettime */
#include <atomic>    /*used in other parts of the assignment */
#include <cassert>
#include <iostream>

#define MAX_THREADS 512 

/* THREAD VARS */
pthread_t handles[MAX_THREADS];
int shortNames[MAX_THREADS];

/* PI VARS */
std::atomic<double> pi{0.0};

int numPoints = 1000000000;
double step = 0.5/numPoints;
int numThreads = 1;

void *compute_pi(void *threadIdPtr);

void add_to_pi(double bar) {
  auto current = pi.load();
  while (!pi.compare_exchange_weak(current, current + bar));
} 

double f(double x) {
  return (6.0/sqrt(1-x*x));
}

int main(int argc, char *argv[]) {
  uint64_t execTime; /*time in nanoseconds */
  struct timespec tick, tock;


  numThreads = atoi(argv[1]);

  pthread_attr_t attr;
  pthread_attr_init (&attr);
  
  clock_gettime(CLOCK_MONOTONIC_RAW, &tick);
 
  
  for(int i=0; i < numThreads; i++){
    shortNames[i] = i;
    int rc = pthread_create(&handles[i], NULL, compute_pi, &shortNames[i]);
    if (rc) {
      std::cout << "Error:unable to create thread," << rc << std::endl;
      exit(-1);
    }
  }

  for(int i=0; i < numThreads; i++){
    pthread_join(handles[i], NULL);
  }
  
  clock_gettime(CLOCK_MONOTONIC_RAW, &tock);
 
  execTime = 1000000000 * (tock.tv_sec - tick.tv_sec) + tock.tv_nsec - tick.tv_nsec;
  
  //assert(abs(pi-3.14159265359) < .0001);
  printf("%llu, %.20f\n", (long long unsigned int) execTime, pi.load());

  return 0;
}

void *compute_pi(void *threadIdPtr) {
  int myId = *(int*)threadIdPtr;

  for (int i = myId; i < numPoints; i+=numThreads) {
    double x = step*((double)i);  // next x
    add_to_pi(step*f(x));
  }
  pthread_exit(NULL);
}