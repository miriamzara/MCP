#include <iostream>
#include <cmath>
#include <cuda_runtime.h>
using namespace std;

#define WIDTH 1031

// Kernel to transpose matrix M
__global__ void matrixTransposition(float *M, float *M_T, int width) {
    int thread_col = blockIdx.x * blockDim.x + threadIdx.x; // x matches to col
    int thread_row = blockIdx.y * blockDim.y + threadIdx.y; // y matches to row
    if(thread_row < width && thread_col < width){
        M_T[thread_row * width + thread_col] = M[thread_col * width + thread_row];
    }
}

__global__ void matrixMultiplication_naif(const float* M, const float* N, float* P, const int width) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    if (row < width && col < width) {
        float sum = 0.;
        for (int k = 0; k < width; ++k) {
            sum += M[row * width + k] * N[k * width + col];
        }
        P[row * width + col] = sum;
    }
}

// Kernel to multiply matrices M_T (transposed M) and N
__global__ void matrixMultiplication_coalesced(float *A_T, float *B, float *C, int N) {
    int thread_col = blockIdx.x * blockDim.x + threadIdx.x; // matches to col
    int thread_row = blockIdx.y * blockDim.y + threadIdx.y; // matches to row
    if(thread_col < N && thread_row < N){
        float sum = 0.;
        for(int k=0; k < N; k++){
            // sum += A[thread_row * N + k] * B[k * N + thread_col];
            sum += A_T[k * N + thread_row] * B[k * N + thread_col];
        }
        C[thread_row * N + thread_col] = sum;
    }
}

int main() {
    // Size in bytes 
    int size = WIDTH * WIDTH * sizeof(float);  

    // Host memory allocation
    float *h_M = (float*)malloc(size);
    float *h_N = (float*)malloc(size);
    float *h_P = (float*)malloc(size);

    // Initialize matrices M and N
    for (int i = 0; i < WIDTH * WIDTH; i++) {
        h_M[i] = 1.0 + (float)rand()/RAND_MAX;
        h_N[i] = 2.0 + (float)rand()/RAND_MAX;
    }

    cout << "CPU result:" << endl;
    for(int i=0; i< 4; i++){
        cout << endl;
        for(int j=0; j<4; j++){
            float sum = 0;
            for(int k=0; k<WIDTH; k++){
                sum += h_M[i*WIDTH + k]*h_N[k*WIDTH + j];
            }
            cout << sum << "\t";
        }
    }
    cout << endl << endl;
    // CudaEvents definition
    cudaEvent_t start_kernel_naif, stop_kernel_naif;
    cudaEvent_t start_kernel_transpose, stop_kernel_transpose;
    cudaEvent_t start_kernel_coalesced, stop_kernel_coalesced;
    cudaEventCreate(&start_kernel_naif);
    cudaEventCreate(&stop_kernel_naif);
    cudaEventCreate(&start_kernel_transpose);
    cudaEventCreate(&stop_kernel_transpose);
    cudaEventCreate(&start_kernel_coalesced);
    cudaEventCreate(&stop_kernel_coalesced);


    // Device memory allocation
    float* d_M;
    float* d_M_T;
    float* d_N;
    float* d_P;
    cudaMalloc((void **) &d_M, size);
    cudaMalloc((void **) &d_M_T, size);
    cudaMalloc((void **) &d_N, size);
    cudaMalloc((void **) &d_P, size);

    
    // Copy matrices M and N from host to device
    cudaMemcpy(d_M, h_M, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_N, h_N, size, cudaMemcpyHostToDevice);
    

    // Define block and grid sizes
    int length = 32; // 2^5 * 2^5 = 1024 max threads per block
    int n_blocks = ceil(float(WIDTH)/length);
    dim3 gridSize(n_blocks, n_blocks);
    dim3 blockSize(length, length);


    // Launch the matrixMultiplication kernel
    cudaEventRecord(start_kernel_naif);
    matrixMultiplication_naif<<<gridSize, blockSize>>>(d_M, d_N, d_P, WIDTH);
    cudaEventRecord(stop_kernel_naif);
    cudaDeviceSynchronize();

    // Copy the result matrix P from device to host
    cudaMemcpy(h_P, d_P, size, cudaMemcpyDeviceToHost);
    // Print part of the result matrix P for verification
    cout << "GPU result (naif):" << endl;
    for(int i=0; i< 4; i++){
        cout << endl;
        for(int j=0; j<4; j++){
            cout << h_P[i*WIDTH + j] << "\t";
        }
    }
    cout << endl << endl;


    // Launch the matrixTransposition kernel (do NOT copy back the resulting M_T from Device to Host)
    cudaEventRecord(start_kernel_transpose);
    matrixTransposition<<<gridSize, blockSize>>>(d_M, d_M_T, WIDTH);

    // Synch the device to wait for the previous kernel to be completed
    cudaEventRecord(stop_kernel_transpose);
    cudaDeviceSynchronize();

    // Launch the matrixMultiplication kernel
    cudaEventRecord(start_kernel_coalesced);
    matrixMultiplication_coalesced<<<gridSize, blockSize>>>(d_M_T, d_N, d_P, WIDTH);
    cudaEventRecord(stop_kernel_coalesced);
    cudaDeviceSynchronize();

    // Copy the result matrix P from device to host
    cudaMemcpy(h_P, d_P, size, cudaMemcpyDeviceToHost);
    // Print part of the result matrix P for verification
    cout << "GPU result (coalesced):" << endl;
    for(int i=0; i< 4; i++){
        cout << endl;
        for(int j=0; j<4; j++){
            cout << h_P[i*WIDTH + j] << "\t";
        }
    }
    cout << endl << endl;

    // Print times
    float elapsed_kernel_naif, elapsed_kernel_transpose, elapsed_kernel_coalesced;

    cudaEventElapsedTime(&elapsed_kernel_naif,start_kernel_naif, stop_kernel_naif); 
    cudaEventElapsedTime(&elapsed_kernel_transpose, start_kernel_transpose, stop_kernel_transpose); 
    cudaEventElapsedTime(&elapsed_kernel_coalesced,start_kernel_coalesced, stop_kernel_coalesced); 

    printf("Elapsed time (kernel naif):                 %.1f us\n", elapsed_kernel_naif*1000);
    printf("Elapsed time (kernel transpose):                 %.1f us\n", elapsed_kernel_transpose*1000);
    printf("Elapsed time (kernel coalesced):                 %.1f us\n", elapsed_kernel_coalesced*1000);


    // Free device memory
    cudaFree(d_M);
    cudaFree(d_M_T);
    cudaFree(d_N);
    cudaFree(d_P);


    // Free host memory
    free(h_M);
    free(h_N);
    free(h_P);

    //Destroy cudaEvents
    cudaEventDestroy(start_kernel_naif);
    cudaEventDestroy(stop_kernel_naif);
    cudaEventDestroy(start_kernel_transpose);
    cudaEventDestroy(stop_kernel_transpose);
    cudaEventDestroy(start_kernel_coalesced);
    cudaEventDestroy(stop_kernel_coalesced);

    return 0;
}
