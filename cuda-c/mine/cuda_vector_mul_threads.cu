#include <iostream>
using namespace std;

__global__  void vec_x_scal(float* array, int* length, float* scalar, float* result);

int main(){
    // step 0: define variables
    float a[] = {1., 3.5, 2.7};
    float coeff = 2.;
    int N = sizeof(a) / sizeof(float); // length of array
    float res[N]; // declared array that will contain the result
    // step 1: allocate memory on device, for input AND for output
    float* d_a; // to fill with device address
    float* d_coeff;
    float* d_res;
    int* d_N;
    cudaMalloc((void**) &d_a, N * sizeof(float));
    cudaMalloc((void**) &d_coeff, sizeof(float));
    cudaMalloc((void**) &d_N, sizeof(int));
    cudaMalloc((void**) &d_res, N * sizeof(float));
    /* copy data
    !!!!!!
    when you define
    float a[] = {1., 3.5, 2.7};
    a is actually automatically treated as a POINTER to the first element
    when used in expressions
    so...
    you must pass cudaMemcpy(destination_address, a)
    a is already treated as an address.
    !!!!!!
    !!!!!!
    */
    cudaMemcpy(d_a, a, N * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_coeff, &coeff, sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_N, &N, sizeof(int), cudaMemcpyHostToDevice);
    /* step 2: launch kernel
    pass-by-pointer method, so you provide addresses
    */
    int num_threads = N;
    vec_x_scal<<<1, N>>>(d_a, d_N, d_coeff, d_res); // <<<n_blocks (gridSize), n_threads (blockSize)>>>
    // step 3: transfer data back
    cudaMemcpy(res, d_res, N * sizeof(float), cudaMemcpyDeviceToHost);
    // step 4: print result for check
    cout << "Result is: ";
    for(int i = 0; i< N; i++){
        cout << res[i] << "\t";
    }
    cout<<endl;
    cudaFree(d_a);
    cudaFree(d_coeff);
    cudaFree(d_res);
    return 0;
}



__global__ void vec_x_scal(float* input_array, int* length, float* scalar, float* output_array){
    /* step 1: retrieve thread idx
    in this case, since we have only one block, just one dimension is enough to identify the thread
    */
    int i = threadIdx.x;
    if (i < (*length)){
        output_array[i] = input_array[i] * (*scalar);
    }
}