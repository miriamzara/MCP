#include <iostream>
#include <cmath>
#include <cstdlib> 
using namespace std;

#define ROWS 4000  // Number of rows in the matrices
#define COLS 6000  // Number of columns in the matrices

__global__ void matrixAdd(float* A, float* B, float* C, int rows, int cols) {
    // ...
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if(idx < (rows*cols)){
        C[idx] = A[idx] + B[idx];
    }
}

int main() {
    // Size in bytes for the ROWS x COLS matrix
    int size = ROWS * COLS * sizeof(float);  
    // Host memory allocation
    float *h_A = (float*)malloc(size); // 1d array (row-wise)
    float *h_B = (float*)malloc(size);
    float *h_C = (float*)malloc(size);
    // Initialize matrices A and B
    for (int i = 0; i < ROWS * COLS; i++) {
        h_A[i] = 1.0 + (float)rand()/RAND_MAX;
        h_B[i] = 2.0 + (float)rand()/RAND_MAX;
    }
    // Device memory allocation
    float *d_A, *d_B, *d_C; 
    cudaMalloc((void**) &d_A, size);
    cudaMalloc((void**) &d_B, size);
    cudaMalloc((void**) &d_C, size);
    // Copy matrices A and B from host to device
    cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice);
    // Define block and grid sizes
    int threads_per_block = 256; // 2^8 ... maybe change later
    float n_elements = ROWS * COLS;
    int n_blocks = int(ceil(n_elements / threads_per_block));
    dim3 gridSize(n_blocks,1,1);
    dim3 blockSize(threads_per_block, 1, 1);
    // Launch the kernel
    matrixAdd<<<gridSize,blockSize>>>(d_A, d_B, d_C, ROWS, COLS);
    cudaDeviceSynchronize();
    // Copy the result matrix C from device to host
    cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost);
    // Print part of the result matrix C for verification
    for (int i = 0; i < 10; i++) {
        cout << h_A[i] << "\t" << h_B[i] << "\t" << h_C[i] << endl;
    }
    // Free device memory
    cudaFree(d_A); cudaFree(d_B); cudaFree(d_C);
    // Free host memory
    free(h_A); free(h_B); free(h_C);
    return 0;
}
