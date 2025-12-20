// Finally our favourite program of all: MATRIX MULTIPLICATION

#include<omp.h>
#include<stdio.h>


// --- --- --- --- --- --- --- --- ---
// --- Basic version, with OMP ---
// --- --- --- --- --- --- --- --- ---

void matrix_multiply_v00(int M, int K, int N, const double *A, const double *B, double *C) {
  // Initialize C to zero
  #pragma omp parallel for
  for (int i = 0; i < M * N; i++) {
    C[i] = 0.0;
  }
  // Perform multiplication: C[i][j] = sum(A[i][k] * B[k][j]) for k=0..K-1
  #pragma omp parallel for collapse(3)
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
      for (int k = 0; k < K; k++) {
        #pragma omp atomic
        C[i * N + j] += A[i * K + k] * B[k * N + j];
      }
    }
  }
}

// --- --- --- --- --- --- --- --- ---
// --- Cache optimization + openMP
// --- --- --- --- --- --- --- --- ---


void matrix_multiply_v00(int M, int K, int N, const double *A, const double *B, double *C) {
  // Initialize C to zero
  #pragma omp parallel for
  for (int i = 0; i < M * N; i++) {
    C[i] = 0.0;
  }
  // Perform multiplication: C[i][j] = sum(A[i][k] * B[k][j]) for k=0..K-1
  #pragma omp parallel for collapse(2)
  for (int k = 0; k < K; k++) {
    for (int i = 0; i < M; i++) {
      double aik = A[i * K + k];
      for (int j = 0; j < N; j++) {
        #pragma omp atomic
        C[i * N + j] += aik * B[k * N + j];
      }
    }
  }
}

// --- --- --- --- --- --- --- --- ---
// --- openMP reduction
// --- --- --- --- --- --- --- --- ---

void matrix_multiply_v00(int M, int K, int N, const double *A, const double *B, double *C) {
  // Initialize C to zero
  //#pragma omp parallel for
  //for (int i = 0; i < M * N; i++) {
  //  #pragma omp atomic write
  //  C[i] = 0.0;
  //}
  
  double sum = 0.0;
  // Perform multiplication: C[i][j] = sum(A[i][k] * B[k][j]) for k=0..K-1
  #pragma omp parallel for collapse(2) reduction(+:sum)
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
      sum = 0.0;
      for (int k = 0; k < K; k++) {
        sum += A[i * K + k] * B[k * N + j];
      }
      //#pragma omp atomic write
      C[i * N + j] = sum;
    }
  }
}