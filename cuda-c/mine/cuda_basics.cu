/* To sync folder in host with folder in device, use:
rsync -avz --delete /Users/miriamzara/MCP/cuda-c/ jetson@192.168.55.1:~/cuda_scripts/
*/

#include <stdio.h>
#include <iostream>
using namespace std;
__global__ void print_int_kernel(int* number);
__global__ void increment_int_kernel(int* number);

// Main function, entry point of the program

/* In general, if you need to use keyboard
arguments, you should define it like:

int main(int argc, char **argv){}

Otherwise, simply use:

int main(){}
*/
int main(){
    cout << "Basic memory allocation on GPU." << endl;
    int* p; // pointer to int (exists on host)
    // Now we want to fill p with a  memory address of the GPU
    cudaMalloc((void**)&p, sizeof(int)); 
    // cudaMalloc expects a valid CPU memory address as first input
    cout << "p = "<< p << endl; //this is a gpu address
    //cout << "*p ="<< *p << endl; // if we try to dereference it, 
    /* it crashes because address does not exist on the host cpu.
    Output:

    (mcp) jetson@jetson:~/cuda_scripts$ ./a.out
    Basic memory allocation on GPU.
    p = 0xf00830000
    Segmentation fault (core dumped)

    */
    cout << "Copying data from host to device:" << endl;
    int a = 10;
    // cudaMemcpy(device_address, host_address, transfer_type)
    cudaMemcpy(p, &a, sizeof(int), cudaMemcpyHostToDevice); 
    print_int_kernel<<<1,1>>>(p);
    cudaDeviceSynchronize();

    cout << "Incrementing number from the GPU..." << endl;
    increment_int_kernel<<<1,1>>>(p);
    cudaDeviceSynchronize();
    cout << "Copying it back to CPU..." << endl;
    int result = 0;
    cudaMemcpy(&result, p, sizeof(int), cudaMemcpyDeviceToHost);
    cout << "Result is "<< result<< endl;
    cudaFree(p);
    return 0;
}

__global__ void print_int_kernel(int* number){
    printf("\033[35m"); // changes to bold magenta
    printf("\n Hello from GPU. I received the number: %d\n", *number); // cout does not work on gpu
    printf("\n\n");
    printf("\033[0m"); // reset to black
}

__global__ void increment_int_kernel(int* number){
    /* pass-by-pointer method
    int* number = & x
    then dereference (*) to act on the value
    */
    *number += 1;
}