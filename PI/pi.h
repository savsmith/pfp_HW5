#include <pthread.h> /*used in other parts of the assignment */
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>  /* for uint64  */
#include <time.h>    /* for clock_gettime */
#include <atomic>    /*used in other parts of the assignment */

#define MAX_THREADS 512 

/* THREAD VARS */
pthread_t handles[MAX_THREADS];
int shortNames[MAX_THREADS];

/* PI VARS */
double pi = 0.0;
int numPoints = 1000000000;
double step = 0.5/numPoints;
int numThreads = 1;

void *compute_pi(void *threadIdPtr);

double f(double x) {
  return (6.0/sqrt(1-x*x));
}