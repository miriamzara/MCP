#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// AXPY operation: y = a*x + y
// where a is a scalar, x and y are vectors

void axpy(size_t n, double a, const double *x, double *y) {
    for (size_t i = 0; i < n; i++) {
        y[i] = a * x[i] + y[i];
    }
}

// Generate an integer random number in the interval [a,b]
size_t uniform_random_range(size_t a, size_t b) {
    return a + rand() % (b - a + 1);
}

// Function to verify the result - Version 01
int verify_result(size_t n, int seed0, double a, const double *x, const double *y) {

    //double tolerance = 1e-15;

    size_t m = 1000;

    size_t start, end;

    double sum = 0.0, sum_sq = 0.0;
    double mean = 0.0, std_dev = 0.0;

    //WARNING: note the problem with this seed initialziation: what happens to the previous initialization?
    srand(seed0); // Fixed seed for reproducibility

    do {

      start = uniform_random_range(0, n-1);
      end = start + m;

    } while (end <= n);


    for (size_t i = start; i < end; i++) {
        sum += y[i];
        sum_sq += y[i]*y[i];
    }

    mean = sum / m;
    double variance = (sum_sq /m) - (mean*mean);
    std_dev = sqrt(variance);

    printf("\nSample mean %e with respect to \\mu parameter 0.0\n", mean);
    printf("Standard error of the mean %e\n", std_dev/sqrt(m));

    if (fabs(mean)/(std_dev/sqrt(m)) < 1.0 ) //1-sigma... this check should depends on the size of m
      return 1;
    else
      return 0;

}


// Function to get elapsed time in seconds
double get_elapsed_time(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

//Gaussian variable with Box-Muller algorithm
double gaussian_random() {
    double u1, u2;

    // Generate two uniform random numbers between 0 and 1
    u1 = (double)rand() / RAND_MAX;
    u2 = (double)rand() / RAND_MAX;

    // Apply Box-Muller Transform
    return sqrt(-2 * log(u1)) * cos(2 * M_PI * u2);
}


int main(int argc, char *argv[]) {

  /*
   * Program main internal variables
   */
  struct timespec start, t0, t1;
  double elapsed = 0.0;
  int internal_seed0 = 3759;

  clock_gettime(CLOCK_MONOTONIC, &start);

  /*
   * Inputs
   */

  // Check command line arguments
  if (argc != 4) {
      printf("Usage: %s <vector_size> <scalar_a> <random_seed>\n", argv[0]);
      printf("Example: %s 1000000 2.5 57391\n", argv[0]);
      return 1;
  }

  // Parse command line arguments
  size_t n = atoi(argv[1]);
  double a = atof(argv[2]);
  int seed0 = atoi(argv[3]);

  // validate input n
  if (n <= 0) {
      printf("error: vector size must be positive\n");
      return 1;
  }

  // validate input random seed
  if (seed0 <= 0) {
      printf("error: random seed must be positive\n");
      return 1;
  }

  printf("=== AXPY Operation ===\n");
  printf("Vector size: %d\n", n);
  printf("Scalar a: %.6f\n\n", a);
  printf("Initial random seed: %d\n\n", seed0);

  /*
   * Allocate main structures
   */

  // Allocate memory for vectors
  double *x = (double *)malloc(n * sizeof(double));
  double *y = (double *)malloc(n * sizeof(double));

  if (x == NULL || y == NULL) {
      printf("Error: memory allocation failed\n");
      free(x);
      free(y);
      return 1;
  }

  clock_gettime(CLOCK_MONOTONIC, &t1);
  elapsed = get_elapsed_time(start, t1);
  printf("Execution time (Inputs): %.6f seconds\n", elapsed);


  /*
   * Initialization of main structures
   */

  clock_gettime(CLOCK_MONOTONIC, &t0);

  // Initialize vectors with sample data
  srand(seed0); // Fixed seed for reproducibility
  for (size_t i = 0; i < n; i++) {
      x[i] = gaussian_random();
      y[i] = gaussian_random();
  }

  clock_gettime(CLOCK_MONOTONIC, &t1);
  elapsed = get_elapsed_time(t0, t1);
  printf("Execution time (Initialization): %.6f seconds\n", elapsed);

  /*
   * Objective of the code: sum of two vectors
   */

  clock_gettime(CLOCK_MONOTONIC, &t0);

  axpy(n, a, x, y);

  clock_gettime(CLOCK_MONOTONIC, &t1);
  elapsed = get_elapsed_time(t0, t1);
  printf("Execution time (Sum of two vectors): %.6f seconds\n", elapsed);

  clock_gettime(CLOCK_MONOTONIC, &t0);

  // Verify the result
  printf("Verifying result...\n");
  int success = verify_result(n, internal_seed0, a, x, y);

  if (success) {
      printf("✓ Verification PASSED\n\n");
  } else {
      printf("✗ Verification FAILED\n\n");
  }

  clock_gettime(CLOCK_MONOTONIC, &t1);
  elapsed = get_elapsed_time(t0, t1);
  printf("Execution time (Code verification): %.6f seconds\n", elapsed);

  // Print sample results (first and last few elements)
  printf("\n=== Sample Results ===\n");
  int sample_size = (n < 5) ? n : 5;
  printf("First %d elements:\n", sample_size);
  for (size_t i = 0; i < sample_size; i++) {
      printf("y[%d] = %.6f; a = %.6f x = %.6f\n", i, y[i], a, x[i]);
  }

  if (n > 10) {
      printf("\nLast %d elements:\n", sample_size);
      for (size_t i = n - sample_size; i < n; i++) {
          printf("y[%d] = %.6f; a = %.6f x = %.6f\n", i, y[i], a, x[i]);
      }
  }

  // Free allocated memory
  free(x);
  free(y);

  clock_gettime(CLOCK_MONOTONIC, &t1);
  elapsed = get_elapsed_time(start, t1);
  printf("\n=== Performance ===\n");
  printf("Execution time (Total time): %.6f seconds\n", elapsed);
  printf("Operations: %d\n", 2 * n); // n multiplications + n additions
  printf("FLOPS: %.2e ops/sec\n", (2.0 * n) / elapsed);

  return 0;

}