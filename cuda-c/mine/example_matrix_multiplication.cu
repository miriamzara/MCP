#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <vector>
#include <assert.h>
using namespace std;

#define WIDTH 128              // Define the matrix width number
#define THREADS_PER_BLOCK_X 32  // Define the number of threads in a block
#define THREADS_PER_BLOCK_Y 32  // Define the number of threads in a block

inline cudaError_t checkCuda(cudaError_t result) {
  if (result != cudaSuccess) {
    fprintf(stderr, "CUDA Runtime Error: %s\n", cudaGetErrorString(result));
    assert(result == cudaSuccess);
  }
  return result;
}

__global__ void matrixMultiplication1D(const float* M, const float* N, float* P, const int width) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < width * width) {
        int row = idx / width;
        int col = idx % width;

        float sum = 0.;
        for (int k = 0; k < width; ++k) {
            sum += M[row * width + k] * N[k * width + col];
        }
        P[idx] = sum;
    }
}

__global__ void matrixMultiplication2D(const float* M, const float* N, float* P, const int width) {
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

__global__ void matrixMultiplication2D_p2a_casting(const float (*M)[WIDTH], // Pointer to Array conversion
                                     const float (*N)[WIDTH],
                                     float (*P)[WIDTH],
                                     int width) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    if (row < width && col < width) {
        float sum = 0.;
        for (int k = 0; k < width; ++k) {
            sum += M[row][k] * N[k][col];
        }
        P[row][col] = sum;
    }
}

// Function to generate a random number between 0 and 1
float random_number() {
    return (std::rand()*1./RAND_MAX);
}
// Function to printout the matrix
void print_matrix(const float* M, int rows, int cols) {
    if (WIDTH < rows)
        rows = WIDTH;
    if (WIDTH < cols)
        cols = WIDTH;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%8.2f", M[i * WIDTH + j]);
        }
        printf("\n");
    }
}

int main(int argc, char** argv) {

    srand(time(NULL));
    std::vector<float> M(WIDTH * WIDTH), N(WIDTH * WIDTH), P(WIDTH * WIDTH);
    std::vector<float> Pcpu(WIDTH * WIDTH, 0.);       
    std::generate(M.begin(), M.end(), random_number); 
    std::generate(N.begin(), N.end(), random_number); 
    printf("\nMatrix M\n");
    print_matrix(M.data(), 10, 10);
    printf("\nMatrix N\n");
    print_matrix(N.data(), 10, 10);

    // Device matrices
    float* d_M;
    float* d_N;
    float* d_P;
    size_t matrixSize = WIDTH * WIDTH * sizeof(float);
    cudaMalloc((void**)&d_M, matrixSize);
    cudaMalloc((void**)&d_N, matrixSize);
    cudaMalloc((void**)&d_P, matrixSize);
    cudaMemcpy(d_M, M.data(), matrixSize, cudaMemcpyHostToDevice);
    cudaMemcpy(d_N, N.data(), matrixSize, cudaMemcpyHostToDevice);

    // 1D grid
    cout << "1D grid matrix multiplication: "<< endl;
    dim3 blockSize(THREADS_PER_BLOCK_X);
    dim3 gridSize(ceil(float(WIDTH*WIDTH)/blockSize.x));
    matrixMultiplication1D<<<gridSize, blockSize>>>(d_M, d_N, d_P, WIDTH);
    checkCuda(
        cudaMemcpy(P.data(), d_P, matrixSize, cudaMemcpyDeviceToHost)
    );
    printf("\nMatrix P\n");
    print_matrix(P.data(), 10, 10);
    cout << endl;


    // 2D grid
    /*  x dimension maps to matrix rows
        y dimension maps to matrix cols
    */
    cout << "2D grid matrix multiplication: "<< endl;
    dim3 blockSize_2D(THREADS_PER_BLOCK_X,THREADS_PER_BLOCK_Y);
    dim3 gridSize_2D(ceil(float(WIDTH)/blockSize.x),ceil(float(WIDTH)/blockSize.y));
    matrixMultiplication2D<<<gridSize_2D, blockSize_2D>>>(d_M, d_N, d_P, WIDTH);
    checkCuda(
        cudaMemcpy(P.data(), d_P, matrixSize, cudaMemcpyDeviceToHost)
    );
    printf("\nMatrix P\n");
    print_matrix(P.data(), 10, 10);

    // 2D grid with pointer-casting and matrix access notation
    /*
    cast d_M, d_N, d_P (pointers to 1 float) 
    into pointers to an array of WIDTH floats (one matrix row)
    then, inside the kernel you can access elements in matrix-style notation
    
    d_M[i][j] == (*(d_M + i))[j]
                 |__________|
                    array
                                        
    */
    cout << "2D grid matrix multiplication with pointer casting and matrix access notation: "<< endl;
    matrixMultiplication2D_p2a_casting<<<gridSize, blockSize>>>((const float (*)[WIDTH])d_M,
                                                  (const float (*)[WIDTH])d_N,
                                                  (float (*)[WIDTH])d_P,
                                                  WIDTH);
    checkCuda(
        cudaMemcpy(P.data(), d_P, matrixSize, cudaMemcpyDeviceToHost)
    );
    printf("\nMatrix P\n");
    print_matrix(P.data(), 10, 10);
    // Cleanup by freeing the allocated GPU memory
    cudaFree(d_M);
    cudaFree(d_N);
    cudaFree(d_P);

    return 0;
}

