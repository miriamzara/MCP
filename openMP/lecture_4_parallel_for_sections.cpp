#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/* To compile:        
(MacOS) g++omp exercise_1.cpp
!! before running, comment all the sections you dont need
*/

// --- openMP parallel ---

int main(void) {

  int thread_id0 = omp_get_thread_num();
  int nthreads = omp_get_max_threads();
  printf("Hello from thread %d of %d\n", thread_id0, nthreads);
  printf("Entering parallel section .... ");
  #pragma omp parallel
  {
    // Code here runs on multiple threads
    int thread_id = omp_get_thread_num();
    //printf("Hello from thread %d of %d\n", thread_id, nthreads);
    printf("Hello world");
    printf("from thread  %d of %d\n", thread_id, nthreads);

  }
  return 0;
}


// -- openMP for

/*
int main(int argc, char *argv[]) {

  // Check command line arguments
  if (argc != 2) {
    printf("Usage: %s <loop_size>\n", argv[0]);
    printf("Example: %s 100\n", argv[0]);
    return 1;
  }

  // Parse command line arguments
  int n = atoi(argv[1]);

  int nthreads = omp_get_max_threads();

  #pragma omp parallel for
  for(int i = 0; i < n; i++) {
    int thread_id = omp_get_thread_num();
    // Loop iterations divided among threads
    printf("i = %d; Hello from thread %d of %d\n", i, thread_id, nthreads);
  }

  return 0;

}
*/

// or...

/*
int main(int argc, char *argv[]) {

  // Check command line arguments
  if (argc != 2) {
    printf("Usage: %s <loop_size>\n", argv[0]);
    printf("Example: %s 100\n", argv[0]);
    return 1;
  }

  // Parse command line arguments
  int n = atoi(argv[1]);

  int nthreads = omp_get_max_threads();

  #pragma omp parallel
  {

    #pragma omp for
    for(int i = 0; i < n; i++) {
      int thread_id = omp_get_thread_num();
      // Loop iterations divided among threads
      printf("i = %d; Hello from thread %d of %d\n", i, thread_id, nthreads);
    }

  }

  return 0;

}

// Schedule: static, dynamic, guided

int main(int argc, char *argv[]) {

  // Check command line arguments
  if (argc != 2) {
    printf("Usage: %s <loop_size>\n", argv[0]);
    printf("Example: %s 100\n", argv[0]);
    return 1;
  }

  // Parse command line arguments
  int n = atoi(argv[1]);

  int nthreads = omp_get_max_threads();

  //// Static: Good when iterations take equal time
  //#pragma omp parallel for schedule(static)

  //// Dynamic: Good when iteration times vary
  //#pragma omp parallel for schedule(dynamic, chunk_size)

  //// Guided: Start large, decrease chunk size
  //#pragma omp parallel for schedule(guided)

  #pragma omp parallel for schedule(dynamic,1)
  for(int i = 0; i < n; i++) {
    int thread_id = omp_get_thread_num();
    // Loop iterations divided among threads
    printf("i = %d; Hello from thread %d of %d\n", i, thread_id, nthreads);
  }

  return 0;

}


// Is the code faster with respect to the serial version? If not, why?

void axpy(size_t n, double a, const double *x, double *y) {

  #pragma omp parallel for
  for (size_t i = 0; i < n; i++) {
    y[i] = a * x[i] + y[i];
  }
}

*/