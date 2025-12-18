#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/*
To compile:        
(MacOS) g++omp exercise_1.cpp
(Jetson) g++ -fopenmp ex_00.cpp -o ex_00.out
*/

// Matrix multiplication: C = A * B
// A is M x K, B is K x N, C is M x N

// --- Possible ways ---

void matrix_multiply_cache_optimization(int M, int K, int N, const double *A, const double *B, double *C) {
 
  // Perform multiplication: C[i][j] = sum(A[i][k] * B[k][j]) for k=0..K-1
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++)
      C[i * N + j] = 0.0;
    for (int k = 0; k < K; k++) {
      double A_ik = A[i * K + k];
      for (int j = 0; j < N; j++) {
        C[i * N + j] += A_ik* B[k * N + j];
      }
    }
  }
}
void matrix_multiply_v00(int M, int K, int N, const double *A, const double *B, double *C) {
 
  double sum = 0.0;
  // Perform multiplication: C[i][j] = sum(A[i][k] * B[k][j]) for k=0..K-1
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
      sum = 0.0;
      for (int k = 0; k < K; k++) {
        sum += A[i * K + k] * B[k * N + j];
      }
      C[i * N + j] = sum;
    }
  }
}

// --- Helper functions ---

// Function to print a small portion of a matrix
void print_matrix_sample(const char *name, const double *matrix, int rows, int cols, int sample_size) {
  printf("\n%s (showing %dx%d from top-left corner):\n", name, sample_size, sample_size);
  for (int i = 0; i < sample_size && i < rows; i++) {
    for (int j = 0; j < sample_size && j < cols; j++) {
      printf("%8.4f ", matrix[i * cols + j]);
    }
    printf("\n");
  }
}


// Generate an integer random number in the interval [a,b] 
size_t uniform_random_range(size_t a, size_t b) {
    return a + rand() % (b - a + 1);
}

// Function to verify the result - Version 01
int verify_result(size_t M, size_t N, int seed0, const double *C) {

    //Testing the product of two normal variables is quite complicated: https://www1.up.poznan.pl/cb48/prezentacje/Oliveira.pdf
    //
    //However, if K dimension is large enough (100 elements sohuld be already fine) even Cij, obtained from the product of two
    //normal distributed variables Aik and Bkj, is approximatively Gaussian!
    //
    //NOTE: Expected variance: K (since Var(X·Y) = 1 for X,Y ~ N(0,1))


    //double tolerance = 1e-15;
  

    size_t m = 1000;

    size_t start, end;

    double sum = 0.0, sum_sq = 0.0;
    double mean = 0.0, std_dev = 0.0;
  
    //WARNING: note the problem with this seed initialziation: what happens to the previous initialization?
    srand(seed0); // Fixed seed for reproducibility

    size_t rnd_i = uniform_random_range(0, M-1);

    do {

      start = uniform_random_range(0, M-1); //NOTE: assume N <= M
      end = start + m;

    } while (end <= M);


    for (size_t j = start; j < end; j++) {
      sum += C[rnd_i*N + j];
      sum_sq += C[rnd_i*N + j]*C[rnd_i*N + j];
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

// --- MAIN ---
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
  if (argc != 5) {
    printf("Usage: %s <M> <K> <N> <random_seed>\n", argv[0]);
    printf("  Computes C = A * B where:\n");
    printf("  A is M x K matrix\n");
    printf("  B is K x N matrix\n");
    printf("  C is M x N matrix\n");
    return 1;
  }
  
  // Parse command line arguments
  size_t M = atoi(argv[1]);
  size_t K = atoi(argv[2]);
  size_t N = atoi(argv[3]);
  int seed0 = atoi(argv[4]);
  
  // validate input n
  if (M <= 0 || K <= 0 || N <= 0) {
    printf("Error: matrix dimensions must be positive\n");
    return 1;
  }
  
  // validate input random seed
  if (seed0 <= 0) {
      printf("Error: random seed must be positive\n");
      return 1;
  }

  printf("=== Matrix Multiplication ===\n");
  printf("Matrix A: %d x %d\n", M, K);
  printf("Matrix B: %d x %d\n", K, N);
  printf("Matrix C: %d x %d\n\n", M, N);
  printf("Initial random seed: %d\n\n", seed0);

  // Calculate memory requirements
  size_t size_A = M * K * sizeof(double);
  size_t size_B = K * N * sizeof(double);
  size_t size_C = M * N * sizeof(double);
  size_t total_memory = size_A + size_B + size_C;
  
  printf("Memory requirements:\n");
  printf("  Matrix A: %.2f GB\n", size_A / (1024.0 * 1024.0* 1024.0));
  printf("  Matrix B: %.2f GB\n", size_B / (1024.0 * 1024.0 * 1024.0));
  printf("  Matrix C: %.2f GB\n", size_C / (1024.0 * 1024.0 * 1024.0));
  printf("  Total: %.2f GB\n\n", total_memory / (1024.0 * 1024.0 * 1024.0));
 
  /*  
   * Allocate main structures
   */
  
  // Allocate memory for matrices
  double *A = (double *)malloc(size_A);
  double *B = (double *)malloc(size_B);
  double *C = (double *)malloc(size_C);
  
  if (A == NULL || B == NULL || C == NULL) {
      printf("Error: memory allocation failed\n");
      free(A);
      free(B);
      free(C);
      return 1;
  }
  
  clock_gettime(CLOCK_MONOTONIC, &t1);
  elapsed = get_elapsed_time(start, t1);
  printf("Execution time (Inputs): %.6f seconds\n", elapsed);

  
  /*  
   * Initialization of main structures
   */
  
  clock_gettime(CLOCK_MONOTONIC, &t0);

 // Initialize matrices with sample data
  printf("Initializing matrices...\n");
  srand(seed0); // Fixed seed for reproducibility
  //WARNING: same rng stream for A and B!
  for (int i = 0; i < M * K; i++) {
      A[i] = gaussian_random();
  }
  for (int i = 0; i < K * N; i++) {
      B[i] = gaussian_random();
  }
   
  clock_gettime(CLOCK_MONOTONIC, &t1);
  elapsed = get_elapsed_time(t0, t1);
  printf("Execution time (Initialization): %.6f seconds\n", elapsed);

  /*  
   * Objective of the code: sum of two vectors
   */
 
  clock_gettime(CLOCK_MONOTONIC, &t0);
  
  //axpy(n, a, x, y);
  matrix_multiply_cache_optimization(M, K, N, A, B, C);
  
  clock_gettime(CLOCK_MONOTONIC, &t1);
  elapsed = get_elapsed_time(t0, t1);
  printf("Execution time (Sum of two vectors): %.6f seconds\n", elapsed);
  double time_matmul = elapsed;
  
  clock_gettime(CLOCK_MONOTONIC, &t0);

  // Verify the result
  printf("Verifying result...\n");
  int success = verify_result(M, N, internal_seed0, C);

  if (success) {
      printf("✓ Verification PASSED\n\n");
  } else {
      printf("✗ Verification FAILED\n\n");
  }

  clock_gettime(CLOCK_MONOTONIC, &t1);
  elapsed = get_elapsed_time(t0, t1);
  printf("Execution time (Code verification): %.6f seconds\n", elapsed);
 
  /*
  // Print sample of input matrices
  int sample_size = (M < 4 || N < 4 || K < 4) ? 3 : 4;
  print_matrix_sample("Matrix A", A, M, K, sample_size);
  print_matrix_sample("Matrix B", B, K, N, sample_size);
  print_matrix_sample("Matrix C (Result)", C, M, N, sample_size);
  */
  
  // Free allocated memory
  free(A);
  free(B);
  free(C);
  
  clock_gettime(CLOCK_MONOTONIC, &t1);
  elapsed = get_elapsed_time(start, t1);
  printf("\n=== Performance ===\n");
  printf("Execution time (Total time): %.6f seconds\n", elapsed);
  printf("Operations: %d Mops\n", M*K*N / 1000000); //matmul
  printf("GFLOPS: %.2e Mops/sec\n", M*K*N / time_matmul / 1e6);
 
  return 0;

}