// When to use atomic and when to use critical?

#include<omp.h>
#include<stdio.h>
#include <time.h>
#include 


void axpy(size_t n, double a, const double *x, double *y) {
    #pragma omp parallel for
    for (size_t i = 0; i < n; i++) {
        y[i] = a * x[i] + y[i];
    }
}

// Function to verify the result - Version 02
int verify_result_v02(size_t n, double a, double value_of_x, double value_of_y, const double *x, const double *y) {

  double tolerance = 1e-15;

  //Plain stupid check
  int success = 1;
  #pragma omp parallel for
  for (size_t i = 0; i < n; i++) {
    double res = fabs(y[i] - (a*value_of_x + value_of_y));
    if (res > tolerance) {

      //FAILED VERIFICATION!
      success = 0;
      printf("Failed verification, i = %d, y[%d] = %e, res = %e\n", i, i, y[i], res);
      //break;

    }

  }

  if (!success)
    return success;

  printf("Verification 1... OK!!!\n\n");

  //For the properties of summation (where X and Y are constant values) \sum_n a*X+Y = a*\sum_n X + \sum_n Y = a*value_of_x*n+value_of_y*n

  //Check the reduction!
  double sum = 0.0;
  #pragma omp parallel for
  for (size_t i = 0; i < n; i++) {
    sum += y[i];
  }

  double res = fabs(sum - (a*value_of_x+value_of_y)*n);

  //is it ok the tolerance???
  if ( res  > tolerance) {
    success = 0;
    printf("Failed verification, Reduction error = %e\n", res);
  }

  return success;

}


// Function to get elapsed time in seconds
double get_elapsed_time(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main(int argc, char *argv[]) {

  /*
   * Program main internal variables
   */
  struct timespec start, t0, t1;
  double elapsed = 0.0;


  clock_gettime(CLOCK_MONOTONIC, &start);

  /*
   * Inputs
   */

  // Check command line arguments
  if (argc != 5) {
      printf("Usage: %s <vector_size> <scalar_a> <value_of_x> <value_of_y>\n", argv[0]);
      printf("Example: %s 1000000 2.5 1.0 2.0\n", argv[0]);
      return 1;
  }

  // Parse command line arguments
  size_t n = atoi(argv[1]);
  double a = atof(argv[2]);
  double value_of_x = atof(argv[3]);
  double value_of_y = atof(argv[4]);

  // validate input n
  if (n <= 0) {
      printf("error: vector size must be positive\n");
      return 1;
  }

  printf("=== AXPY Operation ===\n");
  printf("Vector size: %d\n", n);
  printf("Scalar a: %.6f\n\n", a);

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
  #pragma omp parallel for
  for (size_t i = 0; i < n; i++) {
      x[i] = value_of_x;
      y[i] = value_of_y;
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
  int success = verify_result_v02(n, a, value_of_x, value_of_y, x, y);

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