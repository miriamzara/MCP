#include <iostream>
using namespace std;

#define N 1024  // Length of input vector
#define KERNEL_RADIUS 3  // Radius of the smoothing kernel (kernel size = 2 * KERNEL_RADIUS + 1)
#define THREADS_PER_BLOCK 256 // Number of threads per block

// CUDA kernel for performing naive 1D convolution
__global__ void convolve1D(float* d_input, float* d_output, float* d_kernel, int input_size, int radius) {
    int g_idx = threadIdx.x + blockIdx.x * blockDim.x; // global thread idx
    if (g_idx < input_size){
        //compute d_output[g_idx]
        float sum = 0.;
        for(int i = 0; i <= (2*radius); i++){
            int s_idx = g_idx - radius + i;
            if(0<= s_idx && input_size>= s_idx){
                sum += d_input[s_idx] * d_kernel[i]; // each thread performs 7 memory accesses
            }
            else{
                sum += 0;
            };
        }
        d_output[g_idx] = sum;
    }
}

// CUDA kernel for performing 1D convolution using shared memory
__global__ void convolve1D_sharedMemory(float* d_input, float* d_output, float* d_kernel, int input_size, int radius) {
    int g_idx = threadIdx.x + blockIdx.x * blockDim.x; // global thread idx
    if (g_idx < input_size){
        __shared__ float tmp[2*radius + 1]; // block-shared
        
    }
}


int main() {
    // Size in bytes for input and output vectors
    int size = N * sizeof(float);  

    // Host memory allocation
    float *h_input = (float*)malloc(size);
    float *h_output = (float*)malloc(size);

    // Initialize input vector with input data
    // In this example: a sine wave + noise
    for (int i = 0; i < N; i++) {
        h_input[i] = 2.*sinf(i * 0.1) + (float)rand()/RAND_MAX;  
    }
    
    cout << "Input: " << endl;
    for(int i = 0; i < 4 + KERNEL_RADIUS; i++){
        cout << h_input[i] << "\t";
    }
    cout << endl;
    // Define a 1D smoothing kernel (e.g., a simple averaging kernel or Gaussian-like kernel)
    int kernel_size = 2 * KERNEL_RADIUS + 1;
    float h_kernel[kernel_size] = {0.004, 0.054, 0.242, 0.399, 0.242, 0.054, 0.004};  // Example: Gaussian-like kernel

    // Device memory allocation
    float *d_input;
    float *d_output;
    float *d_kernel;
    cudaMalloc((void **)&d_input, size);
    cudaMalloc((void **)&d_output, size);
    cudaMalloc((void **)&d_kernel, kernel_size * sizeof(float));

    // Copy input data and kernel from host to device
    cudaMemcpy(d_input, h_input, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_output, h_output, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_kernel, h_kernel, kernel_size * sizeof(float), cudaMemcpyHostToDevice);

    // Define block and grid sizes
    int n_blocks = ceil(float(N)/THREADS_PER_BLOCK);
    dim3 gridSize(n_blocks);
    dim3 blockSize(THREADS_PER_BLOCK);

    // Launch the 1D convolution kernel
    convolve1D<<<gridSize, blockSize>>>(d_input, d_output, d_kernel, N, KERNEL_RADIUS);

    // Copy the result back from device to host
    cudaMemcpy(h_output, d_output, size, cudaMemcpyDeviceToHost);

    // Print a few output values for verification
    cout << "Output: " << endl;
    for(int i = 0; i < 4; i++){
        cout << h_output[i] << "\t";
    }
    cout << endl;
    // Free device memory
    cudaFree(d_input);
    cudaFree(d_output);

    // Free host memory
    free(h_input);
    free(h_output);

    return 0;
}
