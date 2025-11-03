#include<iostream>
using namespace std;

// The number of elements is reduced to avoid running out of memory 
#define M_ROWS 200      // Number of rows in the matrix M
#define M_COLS 300      // Number of cols in the matrix M
#define N_ROWS (M_COLS)  // Number of rows in the matrix N (matching the number of columns of matrix)
#define N_COLS 250     // Number of cols in the matrix N

__global__ void matrixMultiplication(float* M, float* N, float* P, int rows_M, int cols_M, int rows_N, int cols_N) {
    // compute P = MxN
    // P is rows_M x cols_N
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    if (row <= rows_M && col <= cols_N){
        float sum = 0.;
        for(int k = 0; k < cols_M; k++){
            sum += M[row * cols_M + k] * N[k * cols_N + col];
        }
        P[row * cols_N + col] = sum;
    }
}

int main() {
    // Size in bytes for the ROWS x COLS matrix
    int size_M = M_ROWS * M_COLS * sizeof(float);  
    int size_N = N_ROWS * N_COLS * sizeof(float);  
    int size_P = M_ROWS * N_COLS * sizeof(float);  

    // Host memory allocation
    float *h_M = (float*)malloc(size_M);
    float *h_N = (float*)malloc(size_N);
    float *h_P = (float*)malloc(size_P);

    // Initialize matrix M
    for (int i = 0; i < M_ROWS * M_COLS; i++) {
        h_M[i] = 1.0 + (float)rand()/RAND_MAX;
    }
    // Initialize matrix N
    for (int i = 0; i < N_ROWS * N_COLS; i++) {
        h_N[i] = 1.0 + (float)rand()/RAND_MAX;
    };

    // Device memory allocation
    float* d_M;
    float *d_N;
    float *d_P;
    cudaMalloc((void**)&d_M, size_M);
    cudaMalloc((void**)&d_N, size_N);
    cudaMalloc((void**)&d_P, size_P);

    // Copy matrices M and N from host to device
    cudaMemcpy(d_M, h_M, size_M, cudaMemcpyHostToDevice);
    cudaMemcpy(d_N, h_N, size_N, cudaMemcpyHostToDevice);
    cudaMemcpy(d_P, h_P, size_P, cudaMemcpyHostToDevice);

    // Define block and grid sizes
    dim3 blockSize(32,32);
    // rows maps to dimension y
    dim3 gridSize(ceil(float(N_COLS) / blockSize.x), ceil(float(M_ROWS)/ blockSize.y));
    // Launch the kernel
    matrixMultiplication<<<gridSize, blockSize>>>(d_M, d_N, d_P, M_ROWS, M_COLS, N_ROWS, N_COLS);
    // Copy the result matrix P from device to host
    cudaMemcpy(h_P, d_P, size_P, cudaMemcpyDeviceToHost);
    // Print part of the result matrix P for verification
    for(int i =0; i< 2; i++){
        for(int j =0; j< 10; j++){
            float sum = 0.;
            for(int k =0; k <= M_COLS; k++){
                sum += h_M[i*M_COLS + k] * h_N[k*N_COLS + j];
            };
            cout << "h_P["<<i<<","<< j<<"]= "<< h_P[i* N_COLS + j] << "\t" << "sum= "<< sum << endl;
        };
    };
    // Free device memory
    cudaFree(d_M);
    cudaFree(d_N);
    cudaFree(d_P);
    // Free host memory
    free(h_M);
    free(h_N);
    free(h_P);
    return 0;
}
