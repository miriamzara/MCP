#include <iostream>
using namespace std;

#define WIDTH 1031

// Kernel to transpose matrix M
__global__ void matrixTransposition(float *M, float *M_T, int width) {
    int thread_col = blockIdx.x * blockDim.x + threadIdx.x; // x matches to col
    int thread_row = blockIdx.y * blockDim.y + threadIdx.y; // y matches to row
    if(thread_row < width && thread_col < width){
        // M_T[idx_y, idx_x] = M[idx_x, idx_y]
        M_T[thread_row * width + thread_col] = M[thread_col * width + thread_row];
    }
}

// Kernel to multiply matrices M_T (transposed M) and N
__global__ void matrixMultiplication(float *A_T, float *B, float *C, int N) {
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
    cout << "Program started" << endl;
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


    // Device memory allocation
    float* d_M;
    float* d_M_T;
    float* d_N;
    float* d_P;
    cudaMalloc((void **) &d_M, size);
    cudaMalloc((void **) &d_M_T, size);
    cudaMalloc((void **) &d_N, size);
    cudaMalloc((void **) &d_P, size);

    cout << "Device memory allocated" << endl;
    // Copy matrices M and N from host to device
    cudaMemcpy(d_M, h_M, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_N, h_N, size, cudaMemcpyHostToDevice);
    

    // Define block and grid sizes
    int length = 32;
    int n_blocks = ceil(float(WIDTH)/length);
    dim3 gridSize(n_blocks, n_blocks);
    dim3 blockSize(length, length);

    // Launch the matrixTransposition kernel (do NOT copy back the resulting M_T from Device to Host)
    matrixTransposition<<<gridSize, blockSize>>>(d_M, d_M_T, WIDTH);

    // Synch the device to wait for the previous kernel to be completed
    cudaDeviceSynchronize();
    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess) {
        cerr << "CUDA error: " << cudaGetErrorString(err) << endl;
        return 1;
    }
    // Launch the matrixMultiplication kernel
    matrixMultiplication<<<gridSize, blockSize>>>(d_M_T, d_N, d_P, WIDTH);

    // Copy the result matrix P from device to host
    cudaMemcpy(h_P, d_P, size, cudaMemcpyDeviceToHost);

    // Print part of the result matrix P for verification
    cout << "CPU result:" << endl;
    for(int i=0; i< 10; i++){
        cout << endl;
        for(int j=0; j<10; j++){
            float sum = 0;
            for(int k=0; k<WIDTH; k++){
                sum += h_M[i*WIDTH + k]*h_N[k*WIDTH + j];
            }
            cout << sum << "\t";
        }
    }

    cout << "GPU result:" << endl;
    for(int i=0; i< 10; i++){
        cout << endl;
        for(int j=0; j<10; j++){
            cout << h_P[i*WIDTH + j] << "\t";
        }
    }
    // Free device memory
    cudaFree(d_M);
    cudaFree(d_M_T);
    cudaFree(d_N);
    cudaFree(d_P);

    // Free host memory
    free(h_M);
    free(h_N);
    free(h_P);

    return 0;
}
