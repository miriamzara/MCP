#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

size_t uniform_random_range(size_t, size_t);
int verify_result(size_t, size_t, int, const double *);
double get_elapsed_time(struct timespec, struct timespec);
double gaussian_random();


void serial_matmul(int M, int K, int N, const double *A, const double *B, double *C) {
  double sum = 0.0;
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

void parallel_matmul_naif(int M, int K, int N, const double *A, const double *B, double *C) {
    int k;
    #pragma omp parallel for collapse(2) default(none) shared(M,K,N,A,B,C) private(k)
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (k = 0; k < K; k++) {
                sum += A[i * K + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
}



int main(int argc, char *argv[]) {
  
  struct timespec start, t0, t1;
  double elapsed = 0.0;
  int internal_seed0 = 3759;
  
  clock_gettime(CLOCK_MONOTONIC, &start);
       
  /*
    Memory allocation
  */
  if (argc != 5) {
    printf("Usage: %s <M> <K> <N> <random_seed>\n", argv[0]);
    printf("  Computes C = A * B where:\n");
    printf("  A is M x K matrix\n");
    printf("  B is K x N matrix\n");
    printf("  C is M x N matrix\n");
    return 1;
  }
  size_t M = atoi(argv[1]);
  size_t K = atoi(argv[2]);
  size_t N = atoi(argv[3]);
  int seed0 = atoi(argv[4]);
  if (M <= 0 || K <= 0 || N <= 0) {
    printf("Error: matrix dimensions must be positive\n");
    return 1;
  }
  if (seed0 <= 0) {
      printf("Error: random seed must be positive\n");
      return 1;
  }
  printf("=== Matrix Multiplication ===\n");
  printf("Matrix A: %lu x %lu\n", M, K);
  printf("Matrix B: %lu x %lu\n", K, N);
  printf("Matrix C: %lu x %lu\n\n", M, N);
  printf("Initial random seed: %d\n\n", seed0);

  size_t size_A = M * K * sizeof(double);
  size_t size_B = K * N * sizeof(double);
  size_t size_C = M * N * sizeof(double);
  size_t total_memory = size_A + size_B + size_C;
  
  printf("Memory requirements:\n");
  printf("  Matrix A: %.2f GB\n", size_A / (1024.0 * 1024.0* 1024.0));
  printf("  Matrix B: %.2f GB\n", size_B / (1024.0 * 1024.0 * 1024.0));
  printf("  Matrix C: %.2f GB\n", size_C / (1024.0 * 1024.0 * 1024.0));
  printf("  Total: %.2f GB\n\n", total_memory / (1024.0 * 1024.0 * 1024.0));

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
  

  /*  
   * Initialization of main structures
   */
  
  printf("Initializing matrices...\n");
  srand(seed0);
  //WARNING: same rng stream for A and B!
  for (int i = 0; i < M * K; i++) {
      A[i] = gaussian_random();
  }
  for (int i = 0; i < K * N; i++) {
      B[i] = gaussian_random();
  }
  /*  
   * Multiplication - serial
   */
 
  clock_gettime(CLOCK_MONOTONIC, &t0);
  serial_matmul(M, K, N, A, B, C);
  clock_gettime(CLOCK_MONOTONIC, &t1);
  elapsed = get_elapsed_time(t0, t1);
  printf("Execution time (serial): %.6f seconds\n", elapsed);


  printf("Verifying result...\n");
  int success = verify_result(M, N, internal_seed0, C);
  if (success) {
      printf("✓ Verification PASSED\n\n");
  } else {
      printf("✗ Verification FAILED\n\n");
  }

/*  
   * Multiplication - parallel
   */
 
  clock_gettime(CLOCK_MONOTONIC, &t0);
  parallel_matmul_naif(M, K, N, A, B, C);
  clock_gettime(CLOCK_MONOTONIC, &t1);
  elapsed = get_elapsed_time(t0, t1);
  printf("Execution time (parallel): %.6f seconds\n", elapsed);


  printf("Verifying result...\n");
  success = verify_result(M, N, internal_seed0, C);
  if (success) {
      printf("✓ Verification PASSED\n\n");
  } else {
      printf("✗ Verification FAILED\n\n");
  }
 
  free(A);
  free(B);
  free(C);
  return 0;
}



size_t uniform_random_range(size_t a, size_t b) {
    return a + rand() % (b - a + 1);
}


int verify_result(size_t M, size_t N, int seed0, const double *C) {
    size_t m = 1000;
    size_t start, end;
    double sum = 0.0, sum_sq = 0.0;
    double mean = 0.0, std_dev = 0.0;
    srand(seed0); 
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
    //printf("\nSample mean %e with respect to \\mu parameter 0.0\n", mean);
    //printf("Standard error of the mean %e\n", std_dev/sqrt(m));
    if (fabs(mean)/(std_dev/sqrt(m)) < 1.0 ) //1-sigma... this check should depends on the size of m
      return 1;
    else
      return 0;
}

double get_elapsed_time(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

double gaussian_random() {
    double u1, u2;
    u1 = (double)rand() / RAND_MAX;
    u2 = (double)rand() / RAND_MAX;
    return sqrt(-2 * log(u1)) * cos(2 * M_PI * u2);
}